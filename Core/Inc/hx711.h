#ifndef HX711_H
#define HX711_H

#include "common.h"


#define DT_PORT     HX711_DT_GPIO_Port
#define DT_PIN      HX711_DT_Pin

#define SCK_PORT    HX711_SCK_GPIO_Port
#define SCK_PIN     HX711_SCK_Pin


void HX711_Init(void);

void microDelay(uint16_t delay);

int32_t HX711_ReadRaw(void);

void HX711_Tare(void);

float HX711_GetWeight(void);

#endif
