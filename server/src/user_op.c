#include "../include/factory.h"

int userRegister(int uFd)
{
    char username[20]={0};
    char salt[12]={0};
    char password[128]={0};
    int ret;
    char result[20];
    recv(uFd, username, sizeof(username), 0);
    //判断用户名是否重复
    ret=selectUserInfo("salt", username, result);
    if(ret)
    {
        generateStr(salt);
        send(uFd, salt, strlen(salt), 0);
        recv(uFd, password, sizeof(password), 0);
        insertUserInfo(username, password, salt);
    }
    else
    {
        //发送用户名重复的信息（盐值为0）
        send(uFd, 0, 1, 0);
    }
    return -1;
    
}
//登录成功返回0,已经登录过发送了token返回1,其他情况返回-1
//username传出参数
int userCheck(int uFd, char *username)
{
    int registerFlag;
    char salt[12]={0};
    char password[128]={0};
    char userPassword[128]={0};
    char isLogin[8]={0};
    char token[32]={0};
    int ret;
    printf("checking...\n");
    //接收token
    recv(uFd, token, sizeof(token), 0);
    printf("recvtoken:%s\n", token);
    if(strlen(token)==32)
    {
        selectTokenInfo(username, token);
        if(username!=NULL)
        {
            return 1;
        }
    }
    recv(uFd, &registerFlag, 4, 0);
    if(registerFlag)
    {
        ret=userRegister(uFd);
        return ret;
    }
    else
    {
        recv(uFd, username, 20, 0);
        printf("username:%s\n",username);
        ret=selectUserInfo("salt", username, salt);
        if(0==ret)
        {
            send(uFd, salt, strlen(salt), 0);
            recv(uFd, userPassword, sizeof(userPassword), 0);
            selectUserInfo("password", username, password);
            if(!strcmp(password,userPassword))
            {
                selectUserInfo("isLogin", username, isLogin);
                if(!strcmp(isLogin, "1"))
                {
                    printf("repeat login\n");
                    return -1;
                }
                if(!strcmp(isLogin, "0"))
                {
                    printf("%s log in\n", username);
                    updateUserInfo(username, "1");
                    return 0;
                }
            }
            else
            {
                printf("log info error\n");
                return -1;
            }
        }
        else
        {
            send(uFd, 0, 1, 0);
            return -1;
        }
    }
}

int writeLog(User_t user, char *op, pthread_mutex_t *logMutex)
{
    time_t now;
    time(&now);
    char *getTime;
    char log[128]={0};
    int fd=open("../conf/operation.log",O_WRONLY|O_APPEND);
    ERROR_CHECK(fd,-1,"log open");
    getTime=ctime(&now);
    getTime[strlen(getTime)-1]=0;
    sprintf(log, "%-20s %30s %20s\n", user.userName, getTime, op);
    pthread_mutex_lock(logMutex);
    write(fd, log, strlen(log));
    pthread_mutex_unlock(logMutex);
    close(fd);
    return 0;
}

