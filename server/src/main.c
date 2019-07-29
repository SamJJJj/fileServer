#include "../include/factory.h"
int exitFd[2];

void sigExitFunc(int signum)
{
    write(exitFd[1], &signum, 1);
}

int epollInAdd(int epfd, int fd)
{
    struct epoll_event event;
    event.events=EPOLLIN;
    event.data.fd=fd;
    int ret;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl add");
    return 0;
}

int epollInDel(int epfd, int fd)
{
    struct epoll_event event;
    event.events=EPOLLIN;
    event.data.fd=fd;
    int ret;
    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event);
    ERROR_CHECK(ret, -1, "epoll_ctl add");
    return 0;
}

int main(int argc, char **argv)
{
    Factory_t factory;
    int socketFd, ret;
    char username[20]={0};
    char order[128]={0};
    char token[32];
    int logFlag;
    pipe(exitFd);
    while(fork())
    {
        signal(SIGUSR1,sigExitFunc);
        int status;
        wait(&status);
        if(WIFEXITED(status))
        {
            printf("exited\n");
            exit(0);
        }
    }
    close(exitFd[1]);
    socketFd=sysInit(&factory);
    ERROR_CHECK(socketFd,-1,"socketFd");
    printf("pthreadNum=%d,queSize=%d\n",factory.threadNum,factory.que.queCapacity);
    factoryStart(&factory);
    int epfd=epoll_create(1);
    struct epoll_event evs[7];
    epollInAdd(epfd,socketFd);
    epollInAdd(epfd,exitFd[0]);
    int newFd,i,readyCount,j,userCnt=0,k;//userCnt记录已经登录的用户数目
    pUser_t userList=(pUser_t)calloc(factory.que.queCapacity,sizeof(User_t));
    pNode_t pNew;
    pFdNode_t timeOutListHead[30], timeOutListTail[30];
    pFdNode_t pPre;
    bzero(timeOutListHead, sizeof(timeOutListHead));
    bzero(timeOutListTail, sizeof(timeOutListTail));
    for(i=0;i<30;i++)
    {
        timeOutListHead[i]=timeOutListTail[i]=NULL;
    }
    int cur=0;
    while(1)
    {
        readyCount = epoll_wait(epfd, evs, 7, 1000);
        if(readyCount==0)
        {
            cur=(cur+1)%30;
            if(NULL!=*(timeOutListHead+cur))
            {
                while(timeOutListHead[cur])
                {
                    pPre=timeOutListHead[cur];
                    for (j = 0; j < factory.que.queCapacity; j++)
                    {
                        if (userList[j].uFd==timeOutListHead[cur]->fd)
                        {
                            writeLog(userList[j], "timeout!", &factory.logMutex);
                            epollInDel(epfd, userList[j].uFd);
                            close(userList[j].uFd);
                            userList[j].uFd = 0;
                            deleteTokenInfo(userList[j].userName);
                            updateUserInfo(userList[j].userName, "0");
                            bzero(userList + j, sizeof(User_t));
                            break;
                        }
                    }
                    listDelete(timeOutListHead + cur, timeOutListTail + cur, timeOutListHead[cur]->fd);
                    timeOutListHead[cur] = pPre->pNext;
                }
            }
        }
        for(i=0;i<readyCount;i++)
        {
            if(evs[i].data.fd==socketFd)
            {
                //连接时usercheck（要计时），把信息加入到链表， 加入epoll，生成token
                newFd = accept(socketFd, NULL, NULL);
                ret = userCheck(newFd, username);
                if(0==ret)
                {
                    epollInAdd(epfd, newFd);
                    logFlag=1;
                    send(newFd, &logFlag, 4, 0);
                    for(j=0;j<factory.que.queCapacity;j++)
                    {
                        if(userList[j].uFd==0)
                        {
                            userList[j].uFd = newFd;
                            userList[j].pwd[0] = '/';
                            userList[j].usingDir[0] = '0';
                            strcpy(userList[j].userName, username);
                            break;
                        }
                    }
                    //生成token写入数据库并发给client
                    writeLog(userList[userCnt], "login", &factory.logMutex);
                    md5SumStr(token, username);
                    insertTokenInfo(username, token);
                    send(newFd, token, strlen(token), 0);
                    //发送完毕；
                    userCnt++;
                    bzero(username, sizeof(username));
                    listTailInsert(timeOutListHead+cur, timeOutListTail+cur, newFd);
                }
                if(1==ret)
                {
                    //此时获得了username
                    pNew = (pNode_t)calloc(1, sizeof(Node_t));
                    for(j=0;j<factory.que.queCapacity;j++)
                    {
                        if(!strcmp(username, userList[j].userName))
                        {
                            strcpy(pNew->userInfo.pwd, userList[j].pwd);
                            strcpy(pNew->userInfo.userName, userList[j].userName);
                            strcpy(pNew->userInfo.usingDir, userList[j].usingDir);
                        }
                    }
                    pNew->userInfo.uFd = newFd;
                    pthread_mutex_lock(&factory.que.queMutex);
                    queInsert(&factory.que, pNew);
                    pthread_mutex_unlock(&factory.que.queMutex);
                    pthread_cond_signal(&factory.cond);
                }
                if(-1==ret)
                {
                    printf("a user login failed\n");
                    logFlag=0;
                    send(newFd, &logFlag, 4, 0);
                    close(newFd);
                }
            }
            if(evs[i].data.fd==exitFd[0])
            {
                for (j = 0; j < factory.threadNum; j++)
                {
                    pthread_cancel(factory.pthid[j]);
                }
                printf("cancel Ok\n");
                // 释放user
                free(userList);
                userList=NULL;
                deleteTokenInfo(NULL);
                updateUserInfo(NULL, "0");
                for (j = 0; j < factory.threadNum; j++)
                {
                    pthread_join(factory.pthid[j], NULL);
                }
                exit(0);
            }
            for(j=0;j<factory.que.queCapacity;j++)
            {
                if(userList[j].uFd==evs[i].data.fd)
                {
                    printf("user %s send order\n", userList[j].userName);
                    recv(userList[j].uFd, order, sizeof(order), 0);
                    if(0==strlen(order))
                    {
                        writeLog(userList[j], "disconnected", &factory.logMutex);
                        close(userList[j].uFd);
                        epollInDel(epfd, userList[j].uFd);
                        userList[j].uFd=0;
                        deleteTokenInfo(userList[j].userName);
                        updateUserInfo(userList[j].userName, "0");
                        bzero(userList+j, sizeof(User_t));
                    }
                    else
                    {
                        printf("%s\n", order);
                        userDo(order, userList + j, &factory.logMutex, NULL);
                        for(k=0;k<30;k++)
                        {
                            listDelete(timeOutListHead+k, timeOutListTail+k, userList[j].uFd);
                        }
                        listTailInsert(timeOutListHead+cur, timeOutListTail+cur, userList[j].uFd);
                    }                
                    bzero(order, sizeof(order));
                    break;
                }
            }
        }
    }
    return 0;
}