#ifndef _DOORPHONE_H_
#define _DOORPHONE_H_

#include "WString.h"

class DoorPhoneObserver
{
private:
    const static uint32_t COUNT_MAX = 300;    // FFT結果の取りまとめ数
    const static uint32_t COUNT_NOTICE = 15;  // 同じ音の数
    const static uint32_t DATA_1_VALUE = 150; // 1番目のデータの数
    const static uint32_t DELAY_MSEC = 10;    // FFT結果の確認頻度(msec)

    const static int DATATYPE_COUNT = 128; // データの種類

    /* data */
    uint16_t count = 0;
    uint16_t data[DATATYPE_COUNT] = {0};
    double dataMax[DATATYPE_COUNT] = {0};

public:
    DoorPhoneObserver(/* args */);
    ~DoorPhoneObserver();

    void init();

    void inc(uint16_t pos, double maxData);
    void clear();

    bool isNeedCheck();
    bool isNotice();

    String dump();
    uint16_t numPos();
    uint16_t numCount();

    uint32_t getDelayMsec() { return DELAY_MSEC; }
};

extern DoorPhoneObserver DP;

#endif // _DOORPHONE_H_
