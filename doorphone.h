#ifndef _DOORPHONE_H_
#define _DOORPHONE_H_

#include <stdint.h>

class DoorPhoneObserver
{
private:
    const static uint32_t COUNT_MAX = 50;    // FFT結果の取りまとめ数
    const static uint32_t COUNT_NOTICE = 15; // 同じ音の数
    const static int DATATYPE_COUNT = 128;   // データの種類

    /* data */
    uint16_t count = 0;
    uint16_t data[DATATYPE_COUNT] = {0};

public:
    DoorPhoneObserver(/* args */);
    ~DoorPhoneObserver();

    void init();

    void inc(uint16_t pos);
    void clear();

    bool isNeedCheck();
    bool isNotice();

    const char *dump();
    uint16_t numPos();
    uint16_t numCount();
};

extern DoorPhoneObserver DP;

#endif // _DOORPHONE_H_