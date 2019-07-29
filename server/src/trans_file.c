#include "../include/factory.h"

int writeResSize(char *fileName, off_t downSize)
{
    char logFile[1128];
    sprintf(logFile, "%s_down", fileName);
    int fd = open(logFile, O_WRONLY | O_CREAT, 0666);
    ERROR_CHECK(fd, -1, "resSize open");
    write(fd, &downSize, sizeof(downSize));
    close(fd);
    return 0;
}

int rmSizeFile(char *fileName)
{
    char logFile[1128];
    int ret;
    sprintf(logFile, "%s_down", fileName);
    ret = unlink(logFile);
    ERROR_CHECK(ret, -1, "rmvSizefile");
    return 0;
}

off_t getResSize(char *fileName)
{
    char logFile[1128];
    sprintf(logFile, "%s_down", fileName);
    printf("getSize:%s\n", logFile);
    int fd = open(logFile, O_RDONLY);
    off_t recvedSize;
    ERROR_CHECK(fd, -1, "getSize open");
    read(fd, &recvedSize, sizeof(off_t));
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

int transFile(pUser_t user, char *args)
{
    train_t train;
    User_t userInfo;
    FileInfo_t fileInfo;
    char fileName[128] = {0};
    char buf[64]={0};
    char md5[33]={0};
    char realName[32]={0};
    char cond[128]={0};
    off_t recvedSize = 0;
    int sendFlag;
    bzero(&userInfo, sizeof(userInfo));
    bzero(&fileInfo, sizeof(fileInfo));
    //检查是否发送
    sprintf(cond, "filename='%s' and owner='%s' and precode=%s", args, user->userName, user->usingDir);
    selectFileInfo("md5", cond, md5);
    printf("md5:%s\n",md5);
    if(strlen(md5)==0||strlen(md5)==1)
    {
        sendFlag=0;
    }
    else
    {
        sendFlag=1;
    }
    if (sendFlag==0)
    {
        int errorLen = 0;
        send(user->uFd, &errorLen, 4, 0);
        return -1;
    }
    bzero(cond,sizeof(cond));
    sprintf(cond, "md5='%s' and exist=%d", md5+1, 1);
    selectFileInfo("filename", cond, realName);
    //发送文件名
    train.transLen = strlen(args);
    send(user->uFd, &train.transLen, sizeof(int), 0);
    send(user->uFd, args, train.transLen, 0);
    sprintf(fileName, "../user_files/%s", realName+1);
    printf("fileName:%s\n", fileName);
    int fd = open(fileName, O_RDWR);
    struct stat staBuf;
    fstat(fd, &staBuf);
    train.transLen = sizeof(staBuf.st_size);
    memcpy(train.buf, &staBuf.st_size, train.transLen);
    send(user->uFd, &train, 4 + train.transLen, 0);
    //接收已经收到了多少
    recvCycle(user->uFd, &recvedSize, sizeof(off_t));
    printf("recvedSize:%ld\n", recvedSize);
    //sendfile
    sendfile(user->uFd, fd, &recvedSize, staBuf.st_size);
    recv(user->uFd, buf, sizeof(buf), 0);
    printf("%s\n", buf);
    return 0;
}

int recvFile(pUser_t user)
{
    int fd, ret, transFlag, nameOverlapFlag;
    int dataLen;
    char res[32]={0};
    char buf[1000] = {0};
    char fileName[1064] = {0};
    char md5[32] = {0};
    char cond[128] = {0};
    char logFileSize[32]={0};
    char nameOverlap[32]={0};
    User_t userInfo;
    FileInfo_t fileInfo;
    bzero(&userInfo, sizeof(userInfo));
    bzero(&fileInfo, sizeof(fileInfo));
    strcpy(userInfo.userName, user->userName);
    strcpy(userInfo.usingDir, user->usingDir);
    //接收md5
    recvCycle(user->uFd, &dataLen, 4);
    recvCycle(user->uFd, md5, dataLen);
    sprintf(cond, "md5='%s'", md5);
    //接收文件名
    recvCycle(user->uFd, &dataLen, 4);
    recvCycle(user->uFd, buf, dataLen);
    //查文件夹中是否有此文件
    selectFileInfo("code", cond, res);
    bzero(cond, sizeof(cond));
    sprintf(cond, "precode=%s and owner='%s' and filename='%s' and filetype='f'", user->usingDir, user->userName, buf);
    //查当前目录是否有同名文件
    selectFileInfo("filename", cond, nameOverlap);
    if(!strcmp(nameOverlap+1, buf))
    {
        nameOverlapFlag=1;//有同名
    }
    else
    {
        nameOverlapFlag=0;//没有同名
    }
    printf("nameOverlapFlag=%d\n", nameOverlapFlag);
    strcpy(fileInfo.fileName, buf);
    strcpy(fileInfo.md5, md5);
    strcpy(fileInfo.fileType,"f");
    if (strlen(res)!=0||nameOverlapFlag)
    {
        transFlag = 0;
    }
    else
    {
        transFlag = 1;
    }
    printf("transFlag=%d\n", transFlag);
    send(user->uFd, &transFlag, 4, 0);
    send(user->uFd, &nameOverlapFlag, 4, 0);
    if (transFlag)
    {
        //接收文件大小
        off_t fileSize, downLoadSize = 0, slice, lastLoadSize = 0, recvedSize = 0;
        fileInfo.exist=1;
        recvCycle(user->uFd, &dataLen, 4);
        if (0 == dataLen)
        {
            printf("no such file\n");
            return -1;
        }
        recvCycle(user->uFd, &fileSize, dataLen);
        fileInfo.fileSize = fileSize;
        //已接收大小
        sprintf(fileName, "../user_files/%s", buf);
        printf("fileName:%s\n", fileName);
        recvedSize = getResSize(fileName);
        if (-1 == recvedSize)
        {
            recvedSize = 0;
        }
        printf("recvedSize:%ld\n", recvedSize);
        send(user->uFd, &recvedSize, sizeof(off_t), 0);
        fd = open(fileName, O_CREAT | O_RDWR, 0666);
        printf("%s\n", fileName);
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
            ret = splice(user->uFd, NULL, fds[1], NULL, 65536, SPLICE_F_MOVE | SPLICE_F_MORE);
            if (0 == ret || -1 == ret)
            {
                printf("recv terminated\n");
                writeResSize(fileName, downLoadSize);
                break;
            }
            ERROR_CHECK(ret, -1, "splice");
            splice(fds[0], NULL, fd, &recvedSize, ret, SPLICE_F_MOVE | SPLICE_F_MORE);
            downLoadSize += ret;
            if (downLoadSize - lastLoadSize >= slice)
            {
                printf("%5.2f%s\r", (float)downLoadSize / fileSize * 100, "%");
                fflush(stdout);
                lastLoadSize = downLoadSize;
                writeResSize(fileName, downLoadSize);
            }
        }
        gettimeofday(&end, NULL);
        printf("use time=%ld\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
        if (downLoadSize >= fileSize)
        {
            rmSizeFile(fileName);
            insertFileInfo(userInfo, fileInfo);
        }
        send(user->uFd, "recv success", 13, 0);
        close(fd);
    }
    else if(!nameOverlapFlag)
    {
        send(user->uFd, "recv success", 13, 0);
        fileInfo.exist=0;
        bzero(cond, sizeof(cond));
        sprintf(cond, "md5='%s'", fileInfo.md5);
        selectFileInfo("filesize", cond, logFileSize);
        fileInfo.fileSize=atol(logFileSize);
        insertFileInfo(userInfo, fileInfo);
    }
    else
    {
        send(user->uFd, "please rename your file", 24, 0);
    }
    
    return 0;
}