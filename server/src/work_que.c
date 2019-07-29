#include "../include/work_que.h"

void queInit(pQue_t p,int fCapacity)
{
    bzero(p,sizeof(Que_t));
    p->queCapacity=fCapacity;
    pthread_mutex_init(&p->queMutex,NULL);
}

void queInsert(pQue_t pq,pNode_t pNew)
{
    if(NULL==pq->queHead)
    {
        pq->queHead=pNew;
        pq->queTail=pNew;
    }
    else
    {
        pq->queTail->pNext=pNew;
        pq->queTail=pNew;
    }
    pq->queSize++;
}

void queGet(pQue_t pq,pNode_t *pGet)
{
    if(0==pq->queSize)
    {
        *pGet=NULL;
    }
    else
    {
        *pGet=pq->queHead;
        pq->queHead=pq->queHead->pNext;
    }
    if(NULL==pq->queHead)
    {
        pq->queTail=NULL;
    }
    pq->queSize--;
}

void listTailInsert(pFdNode_t *ppHead, pFdNode_t *ppTail, int val)
{
    pFdNode_t pNew=(pFdNode_t)calloc(1, sizeof(FdNode_t));
    pNew->fd=val;
    if(NULL==*ppTail)
    {
        *ppHead=pNew;
        *ppTail=pNew;
    }
    else
    {
        (*ppTail)->pNext=pNew;
        *ppTail=pNew;
    }
    printf("insert success\n");
}

void listDelete(pFdNode_t *ppHead, pFdNode_t *ppTail, int deleteNum)
{
    pFdNode_t pCur=*ppHead, pPre;
    pPre=pCur;
    if(NULL==pCur)
    {
        printf("list is empty\n");
        return;
    }
    else if(pCur->fd==deleteNum)
    {
        *ppHead=pCur->pNext;
        if(NULL==*ppHead)
        {
            *ppTail=NULL;
        }
        printf("delete success\n");
    }
    else
    {
        while(pCur)
        {
            if(pCur->fd==deleteNum)
            {
                pPre->pNext=pCur->pNext;
                printf("delete success\n");
                break;
            }
            pPre=pCur;
            pCur=pCur->pNext;
        }
        if(NULL==pCur)
        {
            printf("Don't find deletenum\n");
            return;
        }
        if(pCur==*ppTail)
        {
            *ppTail=pPre;
        }
    }
    free(pCur);
    pCur=NULL;
}
