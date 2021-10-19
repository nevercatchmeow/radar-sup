#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <QString>

//测试数据：FA EB 02 02 01 17 7C 8B B8 00 64 80 64 00 50 21 66 02 0C E4 08 34 00 64 80 64 00 50 21 66 00 00 EC FD

#define MSG_HEAD 0xFAEB         // 公共包头
#define MSG_TAIL 0xECFD         // 公共包尾
#define MSG_HEAD_LENGTH 0x02    // 包头长度
#define MSG_TAIL_LENGTH 0x02    // 包尾长度
#define MSG_CRC_LENGTH 0x02     // CRC校验位长度
#define MSG_TYPE_LENGTH 0x01    // 报文类型位长度

#define MSG_IS_WRONG 0x00       // 错误报文
#define MSG_TYPE_HB 0x01        // 应用报文
#define MSG_TYPE_APP 0x02       // 心跳报文

#define ELE_NUM_BYTE 0x03       // 元素数量字节位
#define ELE_ID_BYTE 0x04
#define ELE_XPOS1_BYTE 0x05
#define ELE_XPOS2_BYTE 0x06
#define ELE_YPOS1_BYTE 0x07
#define ELE_YPOS2_BYTE 0x08
#define ELE_XSPD1_BYTE 0x09
#define ELE_XSPD2_BYTE 0x0A
#define ELE_YSPD1_BYTE 0x0B
#define ELE_YSPD2_BYTE 0x0C
#define ELE_RCS1_BYTE 0x0D
#define ELE_RCS2_BYTE 0x0E
#define ELE_CONF1_BYTE 0x0F
#define ELE_CONF2_BYTE 0x10

#define APP_ELEM_NUM_LENGTH 0x01    // 应用报文中元素数量位长度（单独处理）

#define APP_
#define APP_ELE_ID_LENGTH 0x01      // 元素ID长度
#define X_POS_LENGTH 0x02
#define Y_POS_LENGTH 0x02

#define APP_BODY_MIN_LENGTH 0x0D    // 应用报文单个元素数据长度
#define HEAD_BEAT_MSG_LENGTH 0x0B   // 心跳包长度

#define X_POS_STEP MSG_ELE_ID_LENGTH + 1
#define Y_POS_STEP X_POS_STEP + Y_POS_LENGTH

#define LEN_BEFORE_BODY MSG_HEAD_LENGTH + MSG_TYPE_LENGTH + APP_ELEM_NUM_LENGTH // 0x04

#define MAX_WAIT_TIME 2

typedef struct _UNIT_ELE
{
    uint8_t id;
    int16_t x_pos;
    uint16_t y_pos;
    int16_t x_speed;
    int16_t y_speed;
    int16_t rcs;
    uint16_t conf;
}UNIT_ELE;

typedef struct _UNIT_ELE_VIEW
{
    QString id;
    QString x_pos;
    QString y_pos;
    QString x_speed;
    QString y_speed;
    QString rcs;
    QString conf;
}UNIT_ELE_VIEW;


#endif // COMMON_H
