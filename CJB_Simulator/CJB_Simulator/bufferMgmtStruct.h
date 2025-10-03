#ifndef BUFFER_MGMT_STRUCTS_H
#define BUFFER_MGMT_STRUCTS_H

#define MGD_BUFF_SIZE	400

typedef struct globalBuffStruct
{
	unsigned char globalBuff[MGD_BUFF_SIZE];
	int numBytesInGlobalBuff;
}globalBuffStruct;

extern void buffMgmtAppend(globalBuffStruct* globalBuffStructptr, unsigned char* buff, int noOfBytesToAppend);
extern void buffMgmtRemove(globalBuffStruct* globalBuffStructptr, int tillIndexInclusive);
extern int buffMgmtSearch(globalBuffStruct* globalBuffStructptr, unsigned char charToSearch, int startOfSearch);
extern unsigned char buffMgmtGetValue(globalBuffStruct* globalBuffStructptr, int index);

#endif
