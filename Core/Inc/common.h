#ifndef COMMON_H
#define COMMON_H

#include "main.h"

typedef enum
{
    UNIT_KG,
    UNIT_G,
    UNIT_LBS
} WeightUnit;

typedef struct
{
    char uid[20];
    float weight;
    RTC_DateTypeDef date;
    RTC_TimeTypeDef time;
} WeightRecord;

#endif

//thống nhất kiểu dữ liệu
