#include "doorphone.h"

DoorPhoneObserver::DoorPhoneObserver()
{
}

DoorPhoneObserver::~DoorPhoneObserver()
{
}

void DoorPhoneObserver::init()
{
    clear();
}

void DoorPhoneObserver::inc(uint16_t pos, double maxData)
{
    if (pos <= 0 || pos > DATATYPE_COUNT - 1)
    {
        return;
    }

    data[pos]++;
    dataMax[pos] = maxData;
    count++;
}

void DoorPhoneObserver::clear()
{
    for (int i = 0; i < DATATYPE_COUNT; i++)
    {
        data[i] = 0;
    }
    count = 0;
}

bool DoorPhoneObserver::isNeedCheck()
{
    return (count >= COUNT_MAX);
}

bool DoorPhoneObserver::isNotice()
{
    int count = 0;
    for (int i = 0; i < DATATYPE_COUNT; i++)
    {
        if (data[i] > 1)
        {
            count++;
        }
    }

    return (count <= COUNT_NOTICE && data[1] < DATA_1_VALUE);
}

String DoorPhoneObserver::dump()
{
    String buf;
    int count = 0;
    for (int i = 0; i < DATATYPE_COUNT; i++)
    {
        if (data[i] > 1)
        {
            buf.concat(i);
            buf.concat(":");
            buf.concat(data[i]);
            buf.concat(",");
            count++;
        }
    }

    buf.concat(count);
    return buf;
}

uint16_t DoorPhoneObserver::numPos()
{
    return 0;
}
uint16_t DoorPhoneObserver::numCount()
{
    return 0;
}

DoorPhoneObserver DP;
