#ifndef __FACTORY_H__
#define __FACTORY_H__
#define _GNU_SOURCE
#include "head.h"
#include "work_que.h"
#include "md5.h"

typedef struct
{
    Que_t que;
    pthread_cond_t cond;
    int threadNum;
    pthread_t *pthid;
    short startFlag;
    pthread_mutex_t logMutex;
} Factory_t, *pFactory_t;

typedef struct
{
    char fileName[64];
    off_t fileSize;
    char fileType[2];
    char md5[32];
    int exist;
}FileInfo_t;

typedef struct
{
    int transLen;
    char buf[1000];
} train_t;

int sysInit(pFactory_t pf);
int factoryInit(pFactory_t, int, int);
int factoryStart(pFactory_t);
int transFile(pUser_t, char *);
int userCheck(int, char *);
int recvFile(pUser_t);
int writeLog(User_t, char *, pthread_mutex_t *);
void generateStr(char *);
int insertUserInfo(char *, char *, char *);
int selectUserInfo(char *, char *, char *);
int updateUserInfo(char *, char *);
int selectFileInfo(char *, char *, char *);
int insertFileInfo(User_t , FileInfo_t );
int deleteFileInfo(char *);
int updateFileInfo(char *, char *, char *);
int selectTokenInfo(char *, char *);
int insertTokenInfo(char *, char *);
int deleteTokenInfo(char *);
void userDo(char *, User_t *, pthread_mutex_t *, char *);
#endif
