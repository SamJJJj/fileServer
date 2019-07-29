#include "head.h"
int recvCycle(int, void *, int);
int transFile(int, char *, char *);
int recvFile(int, char *);

typedef struct {
    struct sockaddr_in serAddr;
    char filename[64];
    int putsOrGets;//0puts 1gets
    char token[32];
}TransArgs_t;

int logIn(int socketFd, char *token)
{
    char userName[20] = {0};
    char *userPassWord;
    char salt[20]={0};
    char *password;
    int logFlag;
    int choice;
    send(socketFd, "x", 1, 0);
    while(1)
    {
        printf("0----login\n");
        printf("1----register\n");
        scanf("%d", &choice);
        if(choice == 0 || choice == 1)
        {
            break;
        }
        else
        {
            printf("input error ,please input again\n");
            continue;
        }
    }
    send(socketFd, &choice, sizeof(int), 0);
    if(!choice)
    {
        printf("username:");
        fflush(stdout);
        read(STDIN_FILENO, userName, sizeof(userName));
        if (strlen(userName) > 20)
        {
            printf("userName too long\n");
            return -1;
        }
        send(socketFd, userName, strlen(userName)-1, 0);
        recv(socketFd, salt, sizeof(salt), 0);
        if (strlen(salt))
        {
            printf("password: ");
            fflush(stdout);
            userPassWord = getpass("");
            if (strlen(userPassWord) > 20)
            {
                printf("password too long\n");
                return -1;
            }
            password = crypt(userPassWord, salt);
            send(socketFd, password, strlen(password), 0);
            printf("logging...\n");
            recv(socketFd, &logFlag, 4, 0);
            if(logFlag)
            {
                printf("log in success\n");
                recv(socketFd, token, 32, 0);
                return 0;
            }
            else
            {
                printf("password error or already login on other machine\n");
                return -1;
            }
            
        }
        else
        {
            printf("username error\n");
            return -1;
        }
    }
    else
    {
        printf("username:");
        fflush(stdout);
        read(STDIN_FILENO, userName, sizeof(userName));
        send(socketFd, userName, strlen(userName) - 1, 0);
        recv(socketFd, salt, sizeof(salt), 0);
        if(strlen(userName)>20)
        {
            printf("userName too long\n");
            return -1;
        }
        if(strlen(salt))
        {
            printf("password: ");
            fflush(stdout);
            userPassWord = getpass("");
            if(strlen(userPassWord)>20)
            {
                printf("password too long\n");
                return -1;
            }
            password = crypt(userPassWord, salt);
            send(socketFd, password, strlen(password), 0);
            recv(socketFd, &logFlag, 4, 0);
            if(logFlag)
            {
                printf("register success\n");
                return -1;
            }
        }
        else
        {
            printf("username existed\n");
            return -1;
        }
    }
}

void *pthHandle(void *p)
{
    TransArgs_t *args=(TransArgs_t*)p;
    int newFd=socket(AF_INET, SOCK_STREAM, 0);
    int ret;
    ret=connect(newFd, (struct sockaddr *)&(args->serAddr), sizeof(args->serAddr));
    if(ret==-1)
    {
        printf("child connect error\n");
    }
    ret = send(newFd, args->token, strlen(args->token), 0);
    send(newFd, &args->putsOrGets, 4, 0);
    printf("send:%d\n", ret);
    if(args->putsOrGets)
    {
        send(newFd, args->filename, strlen(args->filename), 0);
        recvFile(newFd, ".");
    }
    else
    {
        printf("transFile\n");
        transFile(newFd, args->filename, ".");
    }
}
int main(int argc, char *argv[])
{
    ARGS_CHECK(argc, 3);
    int resultFlag=1;
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    //char *pwd = ".";
    pthread_t pthid;
    char token[32]={0};
    ERROR_CHECK(socketFd, -1, "socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr, sizeof(serAddr));
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(atoi(argv[2]));
    serAddr.sin_addr.s_addr = inet_addr(argv[1]);
    TransArgs_t transArgs;
    bzero(&transArgs, sizeof(TransArgs_t));
    memcpy(&transArgs.serAddr, &serAddr, sizeof(serAddr));
    int ret;
    ret = connect(socketFd, (struct sockaddr *)&serAddr, sizeof(serAddr));
    ERROR_CHECK(ret, -1, "connect");
    char order[128] = {0};
    char result[256] = {0};
    char *fileName;
    if (!logIn(socketFd, token))
    {
        strcpy(transArgs.token, token);
        while (1)
        {
input:
            read(STDIN_FILENO, order, sizeof(order));
            if (0 == strncmp(order, "exit", 4))
            {
                break;
            }
            if(strlen(order)==1)
            {
                printf("empty order\n");
                goto input;
            }
            ret = send(socketFd, order, strlen(order) - 1, 0);
            if(-1 == ret)
            {
                printf("server closed\n");
                return -1;
            }
            if (0 == strncmp(order, "puts", 4))
            {
                strtok(order," ");
                fileName=strtok(NULL," ");
                if(fileName)
                    fileName[strlen(fileName)-1]=0;
                transArgs.putsOrGets=0;
                strcpy(transArgs.filename, fileName);
                printf("filename:%s\n", fileName);
                if(fileName)
                    pthread_create(&pthid, NULL, pthHandle, &transArgs);
                resultFlag=0;
            }
            if (0 == strncmp(order, "gets", 4))
            {
                //recvFile(socketFd, pwd);
                transArgs.putsOrGets = 1;
                strtok(order, " ");
                fileName = strtok(NULL, " ");
                fileName[strlen(fileName) - 1] = 0;
                strcpy(transArgs.filename, fileName);
                if(fileName)
                    pthread_create(&pthid, NULL, pthHandle, &transArgs);
                resultFlag=0;
            }
            bzero(order, sizeof(order));
            if (resultFlag)
            {
                ret = recv(socketFd, result, sizeof(result), 0);
                if (ret)
                {
                    printf("%s\n", result);
                }
            }
            bzero(result, sizeof(result));
            resultFlag=1;
        }
    }
    close(socketFd);
}