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

void DoorPhoneObserver::inc(uint16_t pos)
{
    if (pos <= 0 || pos > DATATYPE_COUNT - 1)
    {
        return;
    }

    data[pos]++;
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
    return false;
}

String DoorPhoneObserver::dump()
{
    String buf;
    for (int i = 0; i < DATATYPE_COUNT; i++)
    {
        if (data[i] > 1 )
        {
            buf += String(i) + ":" + String(data[i]) + ",";
        }
    }

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
