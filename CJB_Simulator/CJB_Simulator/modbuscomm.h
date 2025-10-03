#ifndef MODBUSCOMM_H
#define MODBUSCOMM_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QTimer>
#include <QDebug>
#include <QTcpServer>
#include <bufferMgmtStruct.h>

#define SERVER_PORT                 502

#define READ_HOLDING_REGS           3
#define WRITE_MULTIPLE_REGS         16

#define DI_TRANS_ID                 11
#define DO_TRANS_ID                 0x15
#define AI_TRANS_ID                 31
#define AO_TRANS_ID                 41

#define NO_OF_REGISTERS             125

//#define NO_OF_DI_REG                4
#define NO_OF_DI_REG                8
#define NO_OF_DO_REG                8
#define NO_OF_AI_REG                16
#define WRITE_MULTIPLE_REGS         16
#define REQ_MULTIPLE_REGS           3
#define NO_OF_DIs                   16*NO_OF_DI_REG
#define NO_OF_DOs                   16*NO_OF_DO_REG
#define NO_OF_BYTES_AI              2*NO_OF_AI_REG
#define BYTES_IN_REG                2
#define MAX_BITS_IN_REG             16

#define SLAVE_ADDRESS               0
#define FUNCTION_CODE               3


#define MSG_HEAD1	0x00
#define MSG_DO_HEAD2	0x15
#define MSG_AO_HEAD2	0x29
#define MSG_DI_HEAD2	0x0B
#define MSG_AI_HEAD2	0x1F

class modbusComm : public QObject
{
    Q_OBJECT
public:
    explicit modbusComm(QObject *parent = nullptr);\

    QTcpSocket *clientSocket;
    globalBuffStruct managedRxBuff;
    int status = 0;
    int dotransactionId = 0;
    unsigned short inputReg[NO_OF_REGISTERS] = {0};
    char sendDOVals[NO_OF_DOs] = {0};
    bool aiReqSts = false;
    unsigned short baseAddrOfDO;
    char receivDIs[NO_OF_DIs] = {0};
    char statusData = 0;
    short dival[4] = {0};
    short plcAIs[NO_OF_BYTES_AI] = {0};

    void connectModbusTcpSocket(const QString &host, quint16 port);
    int setBitHigh(int val, int bitPosition, bool highLow);
    void setBaseAddrOfDO(unsigned short regAddr);
    void sendDiData(short *diVal);
    void sendAoData(short *aoVal);
    void sendDoAoData(int transId, int regLength,  short *inputArr);
    void setBitInRegister(int bitPosition);
    int getDiValue(int diChannel);
    int getAiValue(int aiChannel);
    void setDoValue(int doChannel, char data);
    void storeDoDataInRegArray(short *doDataInRegArray, unsigned short noOfRegToWrite, char *doData);
    void setRegisterHIgh(int bitPosition, bool highLow, short arr[]);
    void initReadTimer();

public slots:
    void sendData();
    void checkReceivedData();
private slots:
    void onNewConnection();
    void readData();
private:
    void decodeData(QByteArray responseData);
    void storeDoData(unsigned short noOfRegToRead);
    void storeAiData(unsigned short noOfRegToRead);
    bool getBitValFrmReg(unsigned short regValue, int bitPosition);

    //receiveBuffer to input Registers
    void receivBuffToInputReg(unsigned char *buff, int size);

    //test for decode data
    int checkCommPacket(globalBuffStruct *rxBuffStructPtr);
    //    void setBitInRegister(int bitPosition);
    QTcpSocket *modbusTcpSocket;
    QTimer *sendReqTimer;
    QTcpServer *tcpServer;
    QTimer *receivReqTimer;
    QTcpServer *server;
    //qint16 dival[4] = {0};
    QTimer *recevTimer;
    QTimer *sendTimer;
    unsigned char registerData[8] = {0};
};

#endif // MODBUSCOMM_H
