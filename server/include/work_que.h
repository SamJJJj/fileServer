#ifndef __WORK_QUE__
#define __WORK_QUE__
#include "head.h"
typedef struct
{
    int uFd;
    char userName[20];
    char usingDir[10]; //记录正在使用的文件夹的code（初始是0）
    char pwd[80];      //记录当前的路径
} User_t, *pUser_t;

typedef struct tag_node
{
    User_t userInfo;
    struct tag_node *pNext;
} Node_t, *pNode_t;

typedef struct
{
    pNode_t queHead, queTail;
    int queCapacity;
    int queSize;
    pthread_mutex_t queMutex;
} Que_t, *pQue_t;

typedef struct fd_node
{
    int fd;
    struct fd_node *pNext;
} FdNode_t, *pFdNode_t;

void queInit(pQue_t, int);
void queInsert(pQue_t, pNode_t);
void queGet(pQue_t, pNode_t *);
void listTailInsert(pFdNode_t *, pFdNode_t *, int);
void listDelete(pFdNode_t *, pFdNode_t *, int);
#endif
