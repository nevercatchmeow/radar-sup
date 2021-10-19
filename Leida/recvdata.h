#ifndef RECVDATA_H
#define RECVDATA_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>

#include "common.h"

class RecvData : public QObject
{
    Q_OBJECT
public:
    QThread recvThread;

public:

    RecvData()
    {
        moveToThread(&recvThread);
        recvThread.start();
    }
    ~RecvData()
    {
        isRun = false;
        recvThread.quit();
        recvThread.wait();
    }

private:
    void procData(QByteArray inputData)
    {
        switch (checkData(inputData)) {
        case MSG_IS_WRONG:
            return;
        case MSG_TYPE_HB:
            return;
        case MSG_TYPE_APP:
            UNIT_ELE ele = {};
            int8_t ele_num = inputData[ELE_NUM_BYTE];

            vec = {};
            xVec = {};
            yVec = {};

            // 报文数据解析
            for (int i = 0; i < ele_num; i++)
            {
                ele = {};

                ele.id = static_cast<uint8_t>(inputData[ELE_ID_BYTE + APP_BODY_MIN_LENGTH * i]);
                // 文档中通信协议的X，Y坐标数据顺序与实际发送顺序相反。
                // X坐标数据
                ele.y_pos = static_cast<uint16_t>(inputData[ELE_XPOS1_BYTE + APP_BODY_MIN_LENGTH * i] << 8)
                                + static_cast<uint8_t>(inputData[ELE_XPOS2_BYTE + APP_BODY_MIN_LENGTH * i]);

                // Y坐标数据
                uint16_t x_pos_temp = static_cast<uint16_t>(inputData[ELE_YPOS1_BYTE + APP_BODY_MIN_LENGTH * i] << 8)
                            + static_cast<uint8_t>(inputData[ELE_YPOS2_BYTE + APP_BODY_MIN_LENGTH * i]);
                ele.x_pos = ushort2short(x_pos_temp);   // 将uint16_t转换为int16_t
                if(ele.x_pos < 0)                       // 如果是负数，则按位取反（协议中未说明，实验得出）
                {
                    ele.x_pos = negateByBit(ele.x_pos);
                }
                // X速度
                uint16_t x_speed_temp = static_cast<uint16_t>(inputData[ELE_XSPD1_BYTE + APP_BODY_MIN_LENGTH * i] << 8)
                            + static_cast<uint8_t>(inputData[ELE_XSPD2_BYTE + APP_BODY_MIN_LENGTH * i]);
                ele.x_speed = ushort2short(x_speed_temp);
                // Y速度
                uint16_t y_speed_temp = static_cast<uint16_t>(inputData[ELE_YSPD1_BYTE + APP_BODY_MIN_LENGTH * i] << 8)
                            + static_cast<uint8_t>(inputData[ELE_YSPD2_BYTE + APP_BODY_MIN_LENGTH * i]);
                ele.y_speed = ushort2short(y_speed_temp);
                // RCS
                uint16_t rcs_temp = static_cast<uint16_t>(inputData[ELE_RCS1_BYTE + APP_BODY_MIN_LENGTH * i] << 8)
                            + static_cast<uint8_t>(inputData[ELE_RCS2_BYTE + APP_BODY_MIN_LENGTH * i]);
                ele.rcs = ushort2short(rcs_temp);
                // 置信度
                ele.conf = static_cast<uint16_t>(inputData[ELE_CONF1_BYTE + APP_BODY_MIN_LENGTH * i] << 8)
                            + static_cast<uint8_t>(inputData[ELE_CONF2_BYTE + APP_BODY_MIN_LENGTH * i]);

                vec.push_back(ele);
            }
            return;
        }
    }

    int checkData(QByteArray data)
    {
        // 校验包体长度
        if((data.length() != HEAD_BEAT_MSG_LENGTH)
            && ((data.length() - MSG_HEAD_LENGTH - MSG_TYPE_LENGTH - MSG_TAIL_LENGTH - MSG_CRC_LENGTH - APP_ELEM_NUM_LENGTH)
                    % APP_BODY_MIN_LENGTH != 0))
        {
            wrongMsgCount++;
            return MSG_IS_WRONG;
        }

        // 校验包头
        uint16_t msg_head = static_cast<uint16_t>(data[0] << 8) + static_cast<uint8_t>(data[1]);
        if(msg_head != MSG_HEAD)
        {
            wrongMsgCount++;
            return MSG_IS_WRONG;
        }
        // 校验包尾
        uint16_t msg_tail = static_cast<uint16_t>(data[data.length() - 2] << 8)
                            + static_cast<uint8_t>(data[data.length() - 1]);
        if(msg_tail != MSG_TAIL)
        {
            wrongMsgCount++;
            return MSG_IS_WRONG;
        }
        // CRC校验：略
        // 解析报文类型
        if(static_cast<uint8_t>(data[MSG_HEAD_LENGTH + MSG_TYPE_LENGTH - 1]) == MSG_TYPE_APP)
        {
            return MSG_TYPE_APP;
        }
        else if(static_cast<uint8_t>(data[MSG_HEAD_LENGTH + MSG_TYPE_LENGTH - 1]) == MSG_TYPE_HB)
        {
            return MSG_TYPE_HB;
        }
        else
        {
            return MSG_IS_WRONG;
        }
    }

    // 按位取反
    int16_t negateByBit(int16_t val)
    {
        for(int i = 0; i < 15; i++)
        {
            val =  val^(1 << i);
        }
        return val;
    }

    // ushort转short
    int16_t ushort2short(uint16_t val)
    {
        int16_t rval = 0;
        int flag = (val >> 15); // 取标志位
        val = val & (0x7FFF);   // 获取除标志位之外的数据
        rval = static_cast<int16_t>(val);
        if(flag == 0x01)        // 是否为负数
        {
            rval *= -1;
        }
        return rval;
    }

public slots:
    // 子线程work槽函数：接收数据
    void startWork(QTcpSocket *socket) {
        this->socket = socket;
        isRun = true;
        // 等待TCP读信号
        connect(this->socket, &QTcpSocket::readyRead, this, [=](){
            if((isRun == true) && (recvThread.isRunning()))
            {
                QByteArray data = socket->readAll();
                if(!data.isEmpty())
                {
                    this->procData(data);
                    qInfo() << "[tcp] recv data: " << data.toHex();
                    emit resultReady(vec);
                }
            }
        }, Qt::QueuedConnection);
    }

    void stopWork()
    {
        this->isRun = false;
        this->socket = nullptr;
    }

signals:
    void resultReady(QVector<UNIT_ELE>);

private:
    QTcpSocket* socket = nullptr;
    bool isRun = true;
    uint64_t wrongMsgCount = 0;
    QVector<UNIT_ELE> vec = {};
    QVector<double> xVec = {};
    QVector<double> yVec = {};
};

class RecvDataCtl : public QObject
{
    Q_OBJECT
public:
    RecvDataCtl() : worker(new RecvData())
    {
        // 开始读数据
        connect(this, &RecvDataCtl::start, worker, &RecvData::startWork, Qt::QueuedConnection);
        // 停止读数据
        connect(this, &RecvDataCtl::stop, worker, &RecvData::stopWork, Qt::QueuedConnection);
        // 返回数据
        connect(worker, &RecvData::resultReady, this, &RecvDataCtl::resultReady, Qt::QueuedConnection);
    }
    ~RecvDataCtl()
    {
        delete worker;
    }
public slots:
    void resultReady(QVector<UNIT_ELE> vec)
    {
        emit returnReady(vec);
    }
signals:
    // 开始信号
    void start(QTcpSocket *socket);
    // 结束信号
    void stop();
    // 数据处理完成信号
    void returnReady(QVector<UNIT_ELE>);
private:
    RecvData *worker = nullptr;
};

#endif // RECVDATA_H
