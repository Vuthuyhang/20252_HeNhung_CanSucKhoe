#ifndef COMMON_H
#define COMMON_H

#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum
{
    UNIT_KG = 0,
    UNIT_G,
    UNIT_LBS
} WeightUnit;

typedef struct
{
    char uid[20];              // UID của thẻ RFID
    float weight;              // Khối lượng
    WeightUnit unit;           // Đơn vị hiện tại

    RTC_DateTypeDef date;      // Ngày
    RTC_TimeTypeDef time;      // Giờ

} WeightRecord;

#endif
