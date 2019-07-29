#include "head.h"
#include "md5.h"

typedef struct
{
    int transLen;
    char buf[1000];
} train_t;

int writeResSize(char *fileName, off_t downSize)
{
    char logFile[1128];
    sprintf(logFile, "%s_down", fileName);
    int fd=open(logFile,O_WRONLY|O_CREAT, 0666);
    ERROR_CHECK(fd, -1, "resSize open");
    write(fd,&downSize,sizeof(downSize));
    close(fd);
    return 0;
}

int rmSizeFile(char *fileName)
{
    char logFile[1128];
    int ret;
    sprintf(logFile, "%s_down", fileName);
    ret = unlink(logFile);
    ERROR_CHECK(ret, -1 ,"rmvSizefile");
    return 0;
}

off_t getResSize(char *fileName)
{
    char logFile[1128];
    sprintf(logFile, "%s_down", fileName);
    printf("getSize:%s\n", logFile);
    int fd=open(logFile,O_RDONLY);
    off_t recvedSize;
    ERROR_CHECK(fd, -1, "getSize open");
    read(fd,&recvedSize,sizeof(off_t));
    close(fd);
    return recvedSize;
}

//循环接收
int recvCycle(int sfd, void *buf, int recvLen)
{
    char *p = (char *)buf;
    int total = 0, ret;
    while (total < recvLen)
    {
        ret = recv(sfd, p + total, recvLen - total, 0);
        if (0 == ret)
        {
            return -1;
        }
        total += ret;
    }
    return 0;
}

int transFile(int newFd, char *args, char *pwd)
{
    train_t train;
    char fileName[128] = {0};
    char md5[32]={0};
    char buf[64];
    int i=32;
    int transFlag, nameOverlapFlag;
    off_t recvedSize = 0;
    sprintf(fileName, "%s/%s", pwd, args);
    //发送md5
    md5Sum(md5, fileName);
    send(newFd, &i, 4, 0);
    send(newFd, md5, sizeof(md5), 0);
    //发送文件名
    train.transLen = strlen(args);
    send(newFd, &train.transLen, sizeof(int), 0);
    send(newFd, args, train.transLen, 0);
    //接收transFlag是否要传输
    recvCycle(newFd, &transFlag, 4);
    //接收nameOverlapFlag是否重名文件
    recvCycle(newFd, &nameOverlapFlag, 4);
    if(transFlag)
    {
        int fd = open(fileName, O_RDWR);
        if (-1 == fd)
        {
            int errorLen = 0;
            send(newFd, &errorLen, 4, 0);
            return -1;
        }
        struct stat staBuf;
        fstat(fd, &staBuf);
        train.transLen = sizeof(staBuf.st_size);
        memcpy(train.buf, &staBuf.st_size, train.transLen);
        send(newFd, &train, 4 + train.transLen, 0);
        //接收已经收到了多少
        recvCycle(newFd, &recvedSize, sizeof(off_t));
        printf("recvedSize:%ld\n", recvedSize);
        //sendfile
        sendfile(newFd, fd, &recvedSize, staBuf.st_size);
        recv(newFd, buf, sizeof(buf), 0);
        printf("%s\n", buf);
        close(fd);
    }
    else if(!nameOverlapFlag)
    {
        recv(newFd, buf, sizeof(buf), 0);
        printf("%s\n", buf);
    }
    else
    {
        recv(newFd, buf, sizeof(buf), 0);
        printf("%s\n", buf);
    }
    
    return 0;
}

int recvFile(int socketFd, char *pwd)
{
    int fd, ret;
    int dataLen;
    char buf[1000] = {0};
    char fileName[1064] = {0};
    off_t fileSize, downLoadSize = 0, slice, lastLoadSize = 0, recvedSize = 0;
    recvCycle(socketFd, &dataLen, 4);
    if (0 == dataLen)
    {
        printf("no such file\n");
        return -1;
    }
    recvCycle(socketFd, buf, dataLen); //接收文件名
    recvCycle(socketFd, &dataLen, 4);
    recvCycle(socketFd, &fileSize, dataLen);
    //已接收大小
    sprintf(fileName, "%s/%s", pwd, buf);
    printf("fileName:%s\n",fileName);
    recvedSize=getResSize(fileName);
    if(-1==recvedSize)
    {
        recvedSize = 0;
    }
    printf("recvedSize:%ld\n", recvedSize);
    send(socketFd, &recvedSize, sizeof(off_t), 0);
    fd = open(fileName, O_CREAT | O_RDWR, 0666);
    printf("%s\n",fileName);
    ERROR_CHECK(fd, -1, "open");
    printf("fileSize=%ld\n", fileSize);
    struct timeval start, end;
    int fds[2];
    pipe(fds);
    gettimeofday(&start, NULL);
    slice = fileSize / 1000;
    downLoadSize = recvedSize;
    while (downLoadSize < fileSize)
    {
        ret = splice(socketFd, NULL, fds[1], NULL, 65536, SPLICE_F_MOVE | SPLICE_F_MORE);
        if(0==ret||-1==ret)
        {
            printf("recv terminated\n");
            writeResSize(fileName, downLoadSize);
            break;
        }
        ERROR_CHECK(ret, -1, "splice");
        splice(fds[0], NULL, fd, &recvedSize, ret, SPLICE_F_MOVE | SPLICE_F_MORE);
        downLoadSize += ret;
        if (downLoadSize - lastLoadSize >= 10*slice)
        {
            printf("%5.2f%s\r", (float)downLoadSize / fileSize * 100, "%");
            fflush(stdout);
            lastLoadSize = downLoadSize;
            writeResSize(fileName, downLoadSize);
        }
        // if(downLoadSize - recvedSize > slice)
        // {
        //     recvedSize = downLoadSize;
        //     writeResSize(fileName, recvedSize);
        // }
    }
    gettimeofday(&end, NULL);
    printf("use time=%ld\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
    if(downLoadSize>=fileSize)
        rmSizeFile(fileName);
    send(socketFd, "recv success", 13, 0);
    close(fd);
    return 0;
}