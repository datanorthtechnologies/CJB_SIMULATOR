#include <QDebug>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bufferMgmtStruct.h"

void buffMgmtAppend(globalBuffStruct* globalBuffStructptr, unsigned char* buff, int noOfBytesToAppend)
{
#if 1
    int noOfBytesToShift = 0;

    if(noOfBytesToAppend < sizeof(globalBuffStructptr->globalBuff))
    {
        if(sizeof(globalBuffStructptr->globalBuff) > (globalBuffStructptr->numBytesInGlobalBuff + noOfBytesToAppend))
        {
            memcpy(globalBuffStructptr->globalBuff + globalBuffStructptr->numBytesInGlobalBuff, buff, noOfBytesToAppend);
            globalBuffStructptr->numBytesInGlobalBuff = globalBuffStructptr->numBytesInGlobalBuff + noOfBytesToAppend;
        }
        else
        {
            noOfBytesToShift = (globalBuffStructptr->numBytesInGlobalBuff + noOfBytesToAppend) - sizeof(globalBuffStructptr->globalBuff);
            //qDebug() << "noOfBytesToShift: " << noOfBytesToShift << " numBytesInGlobalBuff: " << globalBuffStructptr->numBytesInGlobalBuff;
            memcpy(globalBuffStructptr->globalBuff, globalBuffStructptr->globalBuff + noOfBytesToShift, globalBuffStructptr->numBytesInGlobalBuff);
            globalBuffStructptr->numBytesInGlobalBuff = globalBuffStructptr->numBytesInGlobalBuff - noOfBytesToShift;

            memcpy(globalBuffStructptr->globalBuff + globalBuffStructptr->numBytesInGlobalBuff, buff, noOfBytesToAppend);
            globalBuffStructptr->numBytesInGlobalBuff = globalBuffStructptr->numBytesInGlobalBuff + noOfBytesToAppend;
        }
    }
    else
    {
        memcpy(globalBuffStructptr->globalBuff, buff, sizeof(globalBuffStructptr->globalBuff));
        globalBuffStructptr->numBytesInGlobalBuff = sizeof(globalBuffStructptr->globalBuff);
    }
#endif


}

void buffMgmtRemove(globalBuffStruct* globalBuffStructptr, int tillIndexInclusive)
{
    int noOfBytesToShift = globalBuffStructptr->numBytesInGlobalBuff-(tillIndexInclusive+1);
	
    if(tillIndexInclusive < globalBuffStructptr->numBytesInGlobalBuff)
    {
        memcpy(globalBuffStructptr->globalBuff, globalBuffStructptr->globalBuff+tillIndexInclusive+1, noOfBytesToShift);
        globalBuffStructptr->numBytesInGlobalBuff = noOfBytesToShift;
    }
    else
    {
		memset(globalBuffStructptr->globalBuff, 0, MGD_BUFF_SIZE);
		globalBuffStructptr->numBytesInGlobalBuff = 0;
    }
}

int buffMgmtSearch(globalBuffStruct* globalBuffStructptr, unsigned char charToSearch, int startOfSearch)
{
    for(int i = startOfSearch; i < globalBuffStructptr->numBytesInGlobalBuff; i++)
    {
        if(globalBuffStructptr->globalBuff[i] == charToSearch)
        {
            return i;
        }
    }
    return -1;
}

unsigned char buffMgmtGetValue(globalBuffStruct* globalBuffStructptr, int index)
{
    if(globalBuffStructptr->numBytesInGlobalBuff > index)
    {
        return globalBuffStructptr->globalBuff[index];
    }
    else
    {
        return 0;
    }
}


