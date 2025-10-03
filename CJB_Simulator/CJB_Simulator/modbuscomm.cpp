#include "modbuscomm.h"
#include "bufferMgmtStruct.h"
#include "mainwindow.h"

extern MainWindow *mainAppWin;

modbusComm::modbusComm(QObject *parent) : QObject(parent)
{
    server = new QTcpServer();
    //  mavSock = new QUdpSocket();

    //if (server->listen(QHostAddress::Any, SERVER_PORT))
    if (server->listen(QHostAddress::LocalHost, SERVER_PORT))
    {
        qDebug() << "Server started. Waiting for connections...";
    }
    else
    {
        qDebug() << "Server could not start. Error:" << server->errorString();
    }
    // Connect the newConnection signal to a slot that will handle incoming client connections
    connect(server, &QTcpServer::newConnection, this, &modbusComm::onNewConnection);

    //    re = new QTimer(this);
    //    connect(sendTimer, SIGNAL(timeout()), this, SLOT(sendData()));
    status = 0;
    memset(&managedRxBuff, 0, sizeof(managedRxBuff));
    clientSocket = new QTcpSocket(this);
}

void modbusComm::setBaseAddrOfDO(unsigned short regAddr)
{
    baseAddrOfDO = regAddr;
}

void modbusComm::sendDiData(short *diVal)
{
    if (clientSocket == nullptr)
    {
        return;
    }

    if (clientSocket->state() != QAbstractSocket::ConnectedState)
    {
        //        qDebug() << "Socket is not connected!";
        return;
    }

    QByteArray request;

    QDataStream stream(&request, QIODevice::ReadWrite);
    //int dotransactionId = DI_TRANS_ID;
    stream << quint16(dotransactionId);
    stream << quint16(0);                   // Protocol identifier (0 for Modbus TCP/IP)
    //stream << quint16(19);
    stream << quint16(3 + (4*2));        /*(noOfReg * 2)*/

    stream << quint8(SLAVE_ADDRESS);
    stream << quint8(FUNCTION_CODE);
    stream << quint8(04*02);
    for(int i = 0 ; i < 4; i++)
    {
        stream << quint16(diVal[i]);
    }
    clientSocket->write(request);
    if (!clientSocket->waitForBytesWritten(3000))
    {
        qDebug() << "Failed to send Modbus request:" << clientSocket->errorString();
    }
    //qDebug()<<"data sending = "<<request.toHex();
}

void modbusComm::sendAoData(short *aoVal)
{
    //    if (clientSocket == nullptr) {
    //        return;
    //    }
    //    if (clientSocket->state() != QAbstractSocket::ConnectedState) {
    //        //        qDebug() << "Socket is not connected!";
    //        return;
    //    }
    int totalRegisters = 16;
    int aoTransactionId = AI_TRANS_ID;
    QByteArray request;

    QDataStream stream(&request, QIODevice::ReadWrite);

    stream << quint16(aoTransactionId);
    stream << quint16(0);                   // Protocol identifier (0 for Modbus TCP/IP)
    //stream << quint16(19);
    stream << quint16(3 + (totalRegisters * 2));        /*(noOfReg * 2)*/

    stream << quint8(SLAVE_ADDRESS);
    stream << quint8(FUNCTION_CODE);
    stream << quint8(totalRegisters * 02);
    for(int i = 0 ; i < totalRegisters; i++)
    {
        stream << quint16(aoVal[i]);
    }
    clientSocket->write(request);
    if (!clientSocket->waitForBytesWritten(3000))
    {
        qDebug() << "Failed to send Modbus request:" << clientSocket->errorString();
    }
}

void modbusComm::sendDoAoData(int transId, int regLength, short *inputArr)
{
    if (clientSocket == nullptr)
    {
        return;
    }
    if (clientSocket->state() != QAbstractSocket::ConnectedState)
    {
        //        qDebug() << "Socket is not connected!";
        return;
    }
    QByteArray request;

    QDataStream stream(&request, QIODevice::ReadWrite);
    //int dotransactionId = DI_TRANS_ID;
    stream << quint16(transId);
    stream << quint16(0);                   // Protocol identifier (0 for Modbus TCP/IP)
    //stream << quint16(19);
    stream << quint16(3 + (regLength *2));        /*(noOfReg * 2)*/

    stream << quint8(SLAVE_ADDRESS);
    stream << quint8(FUNCTION_CODE);
    stream << quint8(regLength * 02);
    for(int i = 0 ; i < regLength; i++)
    {
        stream << quint16(inputArr[i]);
    }
    clientSocket->write(request);
    if (!clientSocket->waitForBytesWritten(3000)) {
        qDebug() << "Failed to send Modbus request:" << clientSocket->errorString();
    }
    //qDebug()<<"regLength = "<<request.toHex();
    if(transId == DI_TRANS_ID)
    {
        //qDebug()<<"data sending = "<<request.toHex();
    }
}

void modbusComm::onNewConnection()
{
    /*QTcpSocket **/clientSocket = server->nextPendingConnection();

#if 0
    // Handle incoming data when data is received
    connect(clientSocket, &QTcpSocket::readyRead, this, &modbusComm::readData);

#endif



    connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    if(!clientSocket->isOpen())
    {
        sendTimer->stop();
    }

}

void modbusComm::readData()
{
    //sendTimer->start(50);
    //startSendData = 1;
    int numBytesRead = 0;
    // Declare a fixed-size buffer array (100 bytes)
    unsigned char bufferArray[1024]  = {0};


#if 0
    clientSocket = new QTcpSocket(this);
    clientSocket = qobject_cast<QTcpSocket *>(sender());
#endif
    if (clientSocket)
    {
        QByteArray data = clientSocket->readAll();

        //Check if the data is for DO print
        unsigned short transId = (data.at(0) << 8) | data.at(1);
        //qDebug() << "DO raw received data : " << data.toHex();
        if (transId == DO_TRANS_ID)
        {
            // Process DI data here
            //qDebug() << "DO raw received data : " << data.toHex();
        }
        //qDebug() << " raw received data : " << data.toHex();

#if 1
        numBytesRead = data.size();
        // Determine the size to copy (it should not exceed the buffer size)
        size_t sizeToCopy = std::min(static_cast<size_t>(data.size()), sizeof(bufferArray));
        // Copy the data from QByteArray to bufferArray
        memcpy(bufferArray, data.data(), sizeToCopy);

        if(sizeToCopy > 0)
        {
            //Check whether valid data is received
            buffMgmtAppend(&managedRxBuff, (unsigned char*)bufferArray, sizeToCopy);
        }
        //check valid packet
        if(checkCommPacket(&managedRxBuff) == 1)
        {
            //qDebug()<<"Received valid packet";
            //foundValidPacket = 1;
        }
        //mainAppWin->startSendData = 1;
#endif
#if 0
        decodeData(data);
#endif
    }
    else
    {
        statusData = 0;
        qDebug() << "Not Received" ;
    }

}


void modbusComm::sendData()
{
    //qDebug()
    // storeDoDataInRegArray(&dival[0], 4, &sendDOVals[0]);
    sendDiData(dival);
}

//to connect timer   03_03_2025 by muni
void modbusComm::checkReceivedData()
{
    //qDebug() << "hello not in if";
    if(clientSocket->bytesAvailable() > 0)
    {
        readData();
        qDebug() << "hello in if";
    }
    else
    {
        return ;
    }
}
#if 1
void modbusComm::decodeData(QByteArray responseData)
{
    //Check if correct modbus response is received
    if(responseData.length() < 5)
    {
        qDebug("Invalid packet");
        return;
    }
    unsigned char funcCode = responseData.at(7);
    switch(funcCode)
    {
    case WRITE_MULTIPLE_REGS:
    {
        //Extract data length
        unsigned char dataLength = responseData.at(11);
        //qDebug()<<"dataLength = "<<dataLength;
        for (int i = 0; i < (dataLength * 2); i += 2)
        {
            unsigned char lowByte = responseData.at(13+i);
            unsigned char highByte = responseData.at(14+i);
            //            qDebug("lowByte = %x ",lowByte);
            //            qDebug("highByte = %x ",highByte);
            inputReg[i/2] = ((lowByte << 8) | (highByte));
        }


        //Check if the data is for DI or AI based on transaction ID
        unsigned short transId = (responseData.at(0) << 8) | responseData.at(1);
        if (transId == DO_TRANS_ID)
        {
            // Process DI data here
            unsigned short noOfRegToRead = (((responseData.at(10) << 8) | responseData.at(11)));
            storeDoData(noOfRegToRead);
            //qDebug() << "DO decode received data : " << responseData.toHex();

        }
        if(transId == AO_TRANS_ID)
        {
            unsigned short noOfRegToRead = (((responseData.at(10) << 8) | responseData.at(11)));
            storeAiData(noOfRegToRead);
            //qDebug() << "AO decode received data : " << responseData.toHex();
        }
    }
    case REQ_MULTIPLE_REGS:
    {
        //Check if the data is for DI or AI based on transaction ID
        unsigned short transId = (responseData.at(0) << 8) | responseData.at(1);
        if (transId == 0x1f)
        {
            aiReqSts = true;
            //qDebug()<<"aiReqSts = "<<aiReqSts;

        }
        break;
    }
    }
}


#endif

void modbusComm::storeDoData(unsigned short noOfRegToRead)
{
    int index = 0, i = 0, bitPos = 0;
    //qDebug() << "NO_OF_DIs / MAX_BITS_IN_REG : " << NO_OF_DIs / MAX_BITS_IN_REG;
    //qDebug("No of regs are more than DI channels : %d ", noOfRegToRead);
    //checking if no of regs are less than or equal to no of DIs
    if(noOfRegToRead <= (NO_OF_DIs / MAX_BITS_IN_REG))
    {
        for(int j = 0; j < noOfRegToRead; j++)
        {
            for(i = index, bitPos = 0; bitPos < MAX_BITS_IN_REG, i < (index + MAX_BITS_IN_REG); bitPos++, i++)
            {
                receivDIs[i] = getBitValFrmReg(inputReg[j] , bitPos);
                //qDebug() << "receivDIs " << i << ": " << ((int)(receivDIs[i]));
            }
            index = i;
        }
    }
    else
    {
        qDebug() << "NO_OF_DIs / MAX_BITS_IN_REG : " << NO_OF_DIs / MAX_BITS_IN_REG;
        qDebug("No of regs are more than DI channels : %d ", noOfRegToRead);
    }
}

void modbusComm::storeAiData(unsigned short noOfRegToRead)
{
    if (noOfRegToRead <= (NO_OF_BYTES_AI / BYTES_IN_REG))
    {
        for (int i = 0; i < noOfRegToRead; i++)
        {
            plcAIs[i] = inputReg[i];
            //qDebug() << "plcAIs " << i << ": " << ((int)(plcAIs[i]));
        }
    }
    else
    {
        qDebug("Number of registers exceeds available AI channels.");
    }
}

bool modbusComm::getBitValFrmReg(unsigned short regValue, int bitPosition)
{
    int temp = ( (regValue & (1 << bitPosition)) != 0 );
    return(temp);
}

void modbusComm::receivBuffToInputReg(unsigned char *buff, int size)
{
    for (int i = 0; i < (size); i += 2)
    {
        unsigned char lowByte = buff[i];
        unsigned char highByte = buff[i + 1];
        //            qDebug("lowByte = %x ",lowByte);
        //            qDebug("highByte = %x ",highByte);
        inputReg[i/2] = ((lowByte << 8) | (highByte));

    }
#if 1
    qDebug() << "inputReg 0 :" <<   inputReg[0];
    qDebug() << "inputReg 1 : " <<  inputReg[1];
    qDebug() << "inputReg 2 : " <<  inputReg[2];
    qDebug() << "inputReg 3 :" <<   inputReg[3];
#endif
    storeDoData(size);
}

int modbusComm::checkCommPacket(globalBuffStruct *rxBuffStructPtr)
{
    //qDebug() << "checkCommPacket";
    int startFrameIndex = 0;
    int endframeIndex = 0;
    int foundDoHeader = 0;
    int foundAoHeader = 0;
    int foundDiHeader = 0;
    int foundAiHeader = 0;
    int rxNumBytesInFrame;
    int numBytesPkt = 0;
    int foundValidPacket = -1;
    int processingComplete = 0;
    int numAttempts = 0;
    int nextStartPos = 0;

    unsigned char calcChecksum;
    unsigned char commPacket[50];
    unsigned char debBuff[50];
    int numDebBytes;

    while(!processingComplete)
    {
        numAttempts++;
        //Precaution so that the execution does not get stuck in this while loop due to some unhandled issue
        if(numAttempts > 10)
        {
            processingComplete = 1;
        }

        foundDoHeader = 0;
        foundAoHeader = 0;
        foundDiHeader = 0;
        foundAiHeader = 0;
        //Check Header 1
        startFrameIndex = buffMgmtSearch(rxBuffStructPtr, MSG_HEAD1, nextStartPos);

        //Check Header 2
        if(startFrameIndex != -1)
        {
            nextStartPos = startFrameIndex + 1;
            //header for do received
            if(buffMgmtGetValue(rxBuffStructPtr, nextStartPos) == MSG_DO_HEAD2)
            {
                foundDoHeader = 1;
                //qDebug() << "Do header found";
            }
            else if(buffMgmtGetValue(rxBuffStructPtr, nextStartPos) == MSG_AO_HEAD2)
            {
                foundAoHeader = 1;
                //qDebug() << "Ao header found";
            }
            else if(buffMgmtGetValue(rxBuffStructPtr, nextStartPos) == MSG_AI_HEAD2)
            {
                foundAiHeader = 1;
                //qDebug() << "AI header found";
            }
            else if(buffMgmtGetValue(rxBuffStructPtr, nextStartPos) == MSG_DI_HEAD2)
            {
                 foundDiHeader  = 1;
                 //qDebug() << "DI header found";
            }
        }
        else
        {
            processingComplete = 1;
        }

        //DO receive
        if(foundDoHeader == 1)
        {
            char tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 4);
            char tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 5);

            rxNumBytesInFrame = (tempLen1 << 8) | tempLen2;    //length of the frame
            //qDebug() << "rxNumBytesInFrame = " << rxNumBytesInFrame;

            endframeIndex = startFrameIndex + 5 +  rxNumBytesInFrame;  //no need to use
            //qDebug() << "endframeIndex = " << endframeIndex;

            tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 10);
            tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 11);

            //qDebug() << "tempLen1 = " << (int)tempLen1;
            //qDebug() << "tempLen2 = " << (int)tempLen2;

            int rxNumReg = ((tempLen1 << 8) | tempLen2);
            //qDebug() << "rxNumReg = " << rxNumReg;

            int rxNumDataBytes = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 12);
            //qDebug() << "rxNumDataBytes = " << rxNumDataBytes;

            if((rxNumDataBytes == (2 * rxNumReg)) && ( (rxNumDataBytes + 7) == rxNumBytesInFrame))
            {
                memcpy(&commPacket[0], &rxBuffStructPtr->globalBuff[startFrameIndex], (rxNumBytesInFrame + 6));
                QByteArray commPacketByteArray((const char*)commPacket, (rxNumBytesInFrame + 6));
                decodeData(commPacketByteArray);
                //memset(&registerData, 0, sizeof(registerData));
                //                memcpy(&registerData, commPacket, numBytesPkt);
                //                receivBuffToInputReg(registerData, numBytesPkt);

                buffMgmtRemove(rxBuffStructPtr, endframeIndex);
                //qDebug() << "DO packet received";
                foundValidPacket = 1;
                //processingComplete = 1;   //commented
            }
        }

        //AO value get
        if(foundAoHeader == 1)
        {
            char tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 4);
            char tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 5);

            rxNumBytesInFrame = (tempLen1 << 8) | tempLen2;    //length of the frame
            //qDebug() << "rxNumBytesInFrame AO = " << rxNumBytesInFrame;

            endframeIndex = startFrameIndex + 5 +  rxNumBytesInFrame;  //no need to use
            //qDebug() << "endframeIndex AO = " << endframeIndex;

            tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 10);
            tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 11);

            //qDebug() << "tempLen1 = " << (int)tempLen1;
            //qDebug() << "tempLen2 = " << (int)tempLen2;

            int rxNumReg = ((tempLen1 << 8) | tempLen2);
            //qDebug() << "rxNumRegAO = " << rxNumReg;

            int rxNumDataBytes = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 12);
            //qDebug() << "rxNumDataBytes AO = " << rxNumDataBytes;


            if((rxNumDataBytes == (2 * rxNumReg)) && ( (rxNumDataBytes + 7) == rxNumBytesInFrame))
            {
                memcpy(&commPacket[0], &rxBuffStructPtr->globalBuff[startFrameIndex], (rxNumBytesInFrame + 6));
                QByteArray commPacketByteArray((const char*)commPacket, (rxNumBytesInFrame + 6));
                decodeData(commPacketByteArray);
                //memset(&registerData, 0, sizeof(registerData));
                //                memcpy(&registerData, commPacket, numBytesPkt);
                //                receivBuffToInputReg(registerData, numBytesPkt);

                buffMgmtRemove(rxBuffStructPtr, endframeIndex);
                //qDebug() << "AO packet received";
                foundValidPacket = 1;
                //processingComplete = 1;   //commented
            }
        }

        //Ai request get
        if(foundAiHeader == 1)
        {
            char tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 4);
            char tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 5);

            rxNumBytesInFrame = (tempLen1 << 8) | tempLen2;    //length of the frame
            //qDebug() << "tempLen2 AI =  " << ((int)tempLen2);
            //qDebug() << "tempLen1 AI =  " << ((int)tempLen2);

            endframeIndex = startFrameIndex + 4 +  rxNumBytesInFrame;  //no need to use

            tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 10);
            tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 11);


            int rxNumReg = ((tempLen1 << 8) | tempLen2);
            //qDebug() << "endframeIndex AI = " << endframeIndex;


            if(rxNumReg == 0x04 )
            {
                //buffMgmtRemove(rxBuffStructPtr, endframeIndex);
            }

        }

        //Di requist get
        if(foundDiHeader == 1)
        {
            char tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 4);
            char tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 5);

            rxNumBytesInFrame = (tempLen1 << 8) | tempLen2;    //length of the frame
            //qDebug() << "rxNumBytesInFrame DI = " << rxNumBytesInFrame;

            endframeIndex = startFrameIndex + 5 +  rxNumBytesInFrame;  //no need to use

            tempLen1 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 10);
            tempLen2 = buffMgmtGetValue(rxBuffStructPtr, startFrameIndex + 11);


            int rxNumReg = ((tempLen1 << 8) | tempLen2);
            //qDebug() << "endframeIndex DI = " << endframeIndex;


            if(rxNumReg == 0x10)
            {
                //buffMgmtRemove(rxBuffStructPtr, endframeIndex);
            }
        }


    }

    return(foundValidPacket);
}

int modbusComm::getDiValue(int diChannel)
{
    int diValue = 0;

    if(diChannel > 0)
    {
        diValue = receivDIs[diChannel - 1];
    }

    return (diValue);
}

int modbusComm::getAiValue(int aiChannel)
{
    short aiData = 0;
    unsigned char temp1;
    unsigned char temp2;

    if(aiChannel > 0)
    {
//        temp1 = plcAIs[aiChannel - 1] & 0xFF;
//        temp2 = (plcAIs[aiChannel - 1] >> 8) & 0xFF;
//        aiData = temp1 * 256 + temp2;
    }

   // return(aiData);
    return (plcAIs[aiChannel]);
}

void modbusComm::setDoValue(int doChannel, char data)
{
    //sendDOVals[doChannel] = data;
    //qDebug("sendDOVals[%d] = %d", doChannel,sendDOVals[doChannel]);
    if(doChannel > 0)
    {
        sendDOVals[doChannel - 1] = data;
    }
}



void modbusComm::storeDoDataInRegArray(short *doDataInRegArray, unsigned short noOfRegToWrite, char *doData)
{
    for(int i = 0; i < (noOfRegToWrite * 16); i++)
    {
        for(int i = 0; i < (noOfRegToWrite * 16); i++)
        {
            int regNo = i/16;
            short int dataMask = doData[i] << (i % 16);
            doDataInRegArray[regNo] = doDataInRegArray[regNo] | dataMask;
        }
    }
}

void modbusComm::setRegisterHIgh(int bitPosition, bool highLow, short arr[])
{
    if(bitPosition > 0 && bitPosition <= 16)
    {
        arr[0] = setBitHigh(arr[0],bitPosition, highLow);
    }
    else if(bitPosition  > 16 && bitPosition  <= 32)
    {
        arr[1] = setBitHigh(arr[1],(bitPosition - 16), highLow);
    }
    else if(bitPosition  > 32 && bitPosition  <= 48)
    {
        arr[2] = setBitHigh(arr[2],(bitPosition - 32), highLow);
    }
    else if(bitPosition  > 48 && bitPosition  <= 64)
    {
        arr[3] = setBitHigh(arr[3],(bitPosition - 48), highLow);
    }
    else if(bitPosition  > 64 && bitPosition  <= 80)
    {
        arr[4] = setBitHigh(arr[4],(bitPosition - 64), highLow);
    }
    else if(bitPosition  > 80 && bitPosition  <= 96)
    {
        arr[5] = setBitHigh(arr[5],(bitPosition - 80), highLow);
    }

    //return arr;
}

void modbusComm::initReadTimer()
{
#if 1
    receivReqTimer = new QTimer(this);
    connect(receivReqTimer, SIGNAL(timeout()), this, SLOT(checkReceivedData()));
    receivReqTimer->start(10);
#endif
}


int modbusComm::setBitHigh(int val, int bitPosition, bool highLow)
{
    if (highLow)
    {
        return val |= (1 << (bitPosition - 1));
    }
    else
    {
        return val &= ~(1 << (bitPosition - 1));
    }
}











