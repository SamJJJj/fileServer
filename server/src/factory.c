#include "../include/factory.h"

int removeFile(pUser_t user, char *args)
{
    char cond[128]={0};
    char owner[20]={0};
    char fileType[32]={0};
    char md5[33]={0};
    char exist[2]={0};
    char filePath[64]={0};
    sprintf(filePath, "../user_files/%s", args);
    sprintf(cond, "owner='%s' and precode=%s and filename='%s'",user->userName, user->usingDir, args);
    selectFileInfo("exist", cond, exist);
    if(strlen(exist)==0)
    {
        return -1;
    }
    if(!strcmp(exist, " 0"))
    {
        selectFileInfo("md5", cond, md5);
        selectFileInfo("filetype", cond, fileType);
        if(!strcmp(fileType," d"))
        {
            return -1;
        }
        else
        {
            deleteFileInfo(cond);
        }
        bzero(cond,sizeof(cond));
    }
    else
    {
        selectFileInfo("md5", cond, md5);
        bzero(cond,sizeof(cond));
        sprintf(cond, "md5='%s'",md5+1);
        selectFileInfo("filetype", cond, fileType);
        if(strlen(fileType)==2)
        {
            deleteFileInfo(cond);
            unlink(filePath);
        }
        else
        {
            bzero(cond, sizeof(cond));
            sprintf(cond, "owner='%s' and precode=%s and filename='%s'",user->userName, user->usingDir, args);
            updateFileInfo("owner", cond, "NULL");
        }
    }
    sprintf(cond, "md5='%s'", md5+1);
    selectFileInfo("owner", cond, owner);
    puts(owner);
    printf("strlen(owner):%ld\n",strlen(owner));
    if(strlen(owner)==7)
    {
        printf("delete realfile\n");
        deleteFileInfo(cond);
        unlink(filePath);
    }
    return 0;
}

void getDirs(char *usingDir, char *dirs, char *username)
{
    char *columns = "filename,filesize,filetype";
    char condition[64];
    sprintf(condition, "owner='%s' and precode = '%s'",username,usingDir);
    selectFileInfo(columns,condition,dirs);
    int i, cnt=0;
    for(i=0;i<strlen(dirs);i++)
    {
        if(dirs[i]==' ')
        {
            if(cnt%3==0&&cnt)
            {
                dirs[i] = '\n';
            }
            cnt++;
        }
    }
}

int createDir(pUser_t user, char *args)
{
    char fileType[32]={0};
    char cond[128]={0};
    User_t userInfo;
    FileInfo_t fileInfo;
    bzero(&userInfo, sizeof(userInfo));
    bzero(&fileInfo, sizeof(fileInfo));
    strcpy(userInfo.userName, user->userName);
    strcpy(userInfo.usingDir, user->usingDir);
    sprintf(cond,"owner='%s' and precode='%s' and filename='%s'", user->userName, user->usingDir, args);
    selectFileInfo("filetype", cond, fileType);
    if(!strlen(fileType))
    {
        fileInfo.exist=0;
        strcpy(fileInfo.fileName,args);
        strcpy(fileInfo.fileType,"d");
        fileInfo.fileSize=4096;
        insertFileInfo(userInfo, fileInfo);
        return 0;
    }
    else
    {
        printf("already has this directory\n");
        return -1;
    }
}

int removeDir(pUser_t user, char *args)
{
    char fileNames[512]={0};
    char cond[128]={0};
    char code[3]={0};
    char *file[24]={0};
    pUser_t userTemp=(pUser_t)calloc(1,sizeof(User_t));
    strcpy(userTemp->userName, user->userName);
    strcpy(userTemp->usingDir, user->usingDir);
    int i=0;
    sprintf(cond, "precode=%s and owner='%s' and filename='%s' and filetype='d'",user->usingDir,user->userName,args);
    selectFileInfo("code", cond, code);
    if(strlen(code)==0)
        return -1;
    bzero(cond, sizeof(cond));
    sprintf(cond,"precode=%s and owner='%s'", code, user->userName);
    selectFileInfo("filename", cond, fileNames);
    strcpy(userTemp->usingDir, code);
    printf("filenames:%s\n", fileNames);
    file[i]=strtok(fileNames, " ");
    printf("file[%d]=%s\n",i,file[i]);
    i++;
    while((file[i]=strtok(NULL, " "))&&i<24)
    {
        i++;
        printf("%s\n", file[i]);
    }
    i--;
    while(i>=0)
    {
        removeFile(userTemp, file[i]);
        i--;
    }
    bzero(cond,sizeof(cond));
    sprintf(cond, "code=%s", code);
    deleteFileInfo(cond);
    free(userTemp);
    userTemp=NULL;
    return 0;
}

void userDo(char *order, User_t *user, pthread_mutex_t *logMutex, char *pwd)
{
    char *action, *args;
    char dirs[1024] = {0};
    char isDir[2]={0}, usingDir[10]={0};
    char cdCond[128]={0};
    int choice = 0, ret, i;
    action = strtok(order, " ");
    args = strtok(NULL, " ");
    //strcpy(user->pwd, user->pwd);
    printf("%s %s\n", action, args);
    if (!strcmp(action, "pwd"))
    {
        choice = 1;
    }
    if (!strcmp(action, "ls"))
    {
        choice = 2;
    }
    if(!strcmp(action, "cd"))
    {
        choice = 3;
        if(args==0)
            choice=0;
    }
    if(!strcmp(action, "puts"))
    {
        choice = 4;
        if(args==0)
            choice=0;
    }
    if(!strcmp(action, "gets"))
    {
        choice = 5;
        if(args==0)
            choice=0;
    }
    if(!strcmp(action, "remove"))
    {
        choice = 6;
        if(args==0)
            choice=0;
    }
    if(!strcmp(action, "mkdir"))
    {
        choice = 7;
        if(args==0)
            choice=0;
    }
    // if(!strcmp(action, "rmdir"))
    // {
    //     choice = 8;
    //     if(args==0)
    //         choice=0;
    // }
    writeLog(*user, order, logMutex);
    switch (choice)
    {
    case 1:
        printf("%s\n",user->pwd);
        send(user->uFd, user->pwd, strlen(user->pwd), 0);
        break;
    case 2:
        printf("%s\n",user->pwd);
        getDirs(user->usingDir, dirs, user->userName);
        printf("strlen dirs %ld\n", strlen(dirs));
        if(strlen(dirs)==0)
        {
            strcpy(dirs, " empty directory");
        }
        ret = send(user->uFd, dirs+1, strlen(dirs)-1, 0);
        break;
    case 3:
        if(!strcmp(args,".."))
        {
            if(!strcmp(user->pwd,"/"))
            {
                send(user->uFd, "already in top-level directory", 31, 0);
            }
            else
            {
                send(user->uFd, "cd success", 11, 0);
                for(i=strlen(user->pwd)-1;i>0;i--)
                {
                    if(user->pwd[i]!='/')
                    {
                        user->pwd[i]=0;
                    }
                    if(user->pwd[i]=='/')
                        break;
                }
                if(i!=0)
                {
                    user->pwd[i]=0;
                }
                sprintf(cdCond, "code=%s", user->usingDir);
                ret=selectFileInfo("precode", cdCond, usingDir);
                strcpy(user->usingDir, usingDir);
            }
        }
        else
        {
            sprintf(cdCond, "owner='%s' and filename='%s' and precode=%s", user->userName, args, user->usingDir);
            ret = selectFileInfo("filetype", cdCond, isDir);
            if (!ret)
            {
                if (!strcmp(isDir, " d"))
                {
                    send(user->uFd, "cd success", 11, 0);
                    if (strcmp(user->pwd, "/"))
                        strcat(user->pwd, "/");
                    strcat(user->pwd, args);
                    selectFileInfo("code", cdCond, usingDir);
                    strcpy(user->usingDir, usingDir);
                }
                else
                {
                    send(user->uFd, "no such directory", 18, 0);
                }
            }
            else
            {
                send(user->uFd, "no such directory", 18, 0);
            }
        }
        bzero(isDir, sizeof(isDir));
        break;
    case 4:
        //ret=recvFile(user);
        printf("receive file\n");
        break;
    case 5:
        //ret=transFile(user, args);
        printf("send file\n");
        break;
    case 6:
        ret=removeFile(user, args);
        if(0==ret)
        {
            send(user->uFd, "remove success", 15, 0);
        }
        else
        {
            send(user->uFd, "no such file or directory", 26, 0);
        }
        break;
    case 7:
        ret = createDir(user, args);
        if(0==ret)
        {
            send(user->uFd, "make success", 13, 0);
        }
        else
        {
            send(user->uFd, "make failed", 12, 0);
        }
        break;
    // case 8:
    //     ret = removeDir(user, args);
    //     if(0==ret)
    //     {
    //         send(user->uFd, "rmdir success", 14, 0);
    //     }
    //     else
    //     {
    //         send(user->uFd, "rmdir failed", 13, 0);
    //     }
    case 0:
        send(user->uFd, "invalid command", 15, 0);
        break;
    }
}

void cleanup(void *p)
{
    pthread_mutex_t *mutex=(pthread_mutex_t*)p;
    pthread_mutex_unlock(mutex);
}

void *pthHandle(void *p)
{
    pFactory_t pf = (pFactory_t)p;
    pNode_t pGet;
    int sendOrRecv;  // 0 接收  1发送
    int ret;
    char fileName[64];
    while (1)
    {
        pthread_mutex_lock(&pf->que.queMutex);
        pthread_cleanup_push(cleanup,&pf->que.queMutex);
        if (!pf->que.queSize)
        {
            pthread_cond_wait(&pf->cond, &pf->que.queMutex);
        }
        queGet(&pf->que, &pGet);
        pthread_cleanup_pop(1);
        if (NULL != pGet)
        {
            recv(pGet->userInfo.uFd, &sendOrRecv, 4, 0);
            if(sendOrRecv)
            {
                recv(pGet->userInfo.uFd, fileName, sizeof(fileName), 0);
                printf("fileName:%s\n", fileName);
                transFile(&pGet->userInfo, fileName);
            }
            else
            {
                recvFile(&pGet->userInfo);
            }
            close(pGet->userInfo.uFd);
        }
        free(pGet);
        pGet=NULL;
    }
}

int factoryInit(pFactory_t p, int threadNum, int fCapacity)
{
    queInit(&p->que, fCapacity);
    p->threadNum = threadNum;
    p->startFlag = 0;
    p->pthid = (pthread_t *)calloc(threadNum, sizeof(pthread_t));
    pthread_cond_init(&p->cond, NULL);
    pthread_mutex_init(&p->logMutex,NULL);
    return 0;
}

int factoryStart(pFactory_t p)
{
    if (!p->startFlag)
    {
        int i;
        for (i = 0; i < p->threadNum; i++)
        {
            pthread_create(p->pthid + i, NULL, pthHandle, p);
        }
        p->startFlag = 1;
        return 0;
    }
    return -1;
}
