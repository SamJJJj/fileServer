#include "../include/factory.h"

int tcpInit(int *sfd,char* ip,char* port)
{
    int socketFd=socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(socketFd,-1,"socket");
    struct sockaddr_in serAddr;
    bzero(&serAddr,sizeof(serAddr));
    serAddr.sin_family=AF_INET;
    serAddr.sin_port=htons(atoi(port));
    serAddr.sin_addr.s_addr=inet_addr(ip);
    int ret;
    int reuse=1;
    ret=setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    ERROR_CHECK(ret,-1,"setsockopt");
    ret=bind(socketFd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    ERROR_CHECK(ret,-1,"bind");
    listen(socketFd,10);
    *sfd=socketFd;
    return 0;
}

int sysInit(pFactory_t pf)
{
    printf("initiating...\n");
    char *path="../conf/server.conf";
    FILE *fp=fopen(path,"r+");
    int ret;
    ERROR_CHECK(fp,NULL,"fopen");
    char buf[100]={0};
    char *res;
    int i=0,sfd;
    char ip[20]={0},port[10]={0};
    int threadNum,queSize;
    while(fgets(buf,sizeof(buf),fp))
    {
        strtok(buf,"=");
        res=strtok(NULL,"=");
        res[strlen(res)-1]=0;
        switch(i)
        {
        case 0:strcpy(ip,res);
               break;
        case 1:strcpy(port,res);
               break;
        case 2:threadNum=atoi(res);
               break;
        case 3:queSize=atoi(res);
               break;
        }
        i++;
    }
    fclose(fp);
    ret=tcpInit(&sfd,ip,port);
    ERROR_CHECK(ret,-1,"tcpInit");
    factoryInit(pf,threadNum,queSize);
    printf("initiated!\n");
    return sfd;
}