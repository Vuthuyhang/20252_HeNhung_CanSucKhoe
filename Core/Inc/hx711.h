#ifndef HX711_H
#define HX711_H

#include "common.h"

#define DT_PORT     HX711_DT_GPIO_Port
#define DT_PIN      HX711_DT_Pin

#define SCK_PORT    HX711_SCK_GPIO_Port
#define SCK_PIN     HX711_SCK_Pin

/* Khởi tạo HX711 */
void HX711_Init(void);

/* Đọc giá trị ADC thô */
int32_t HX711_ReadRaw(void);

/* Đưa cân về 0 */
void HX711_Tare(void);

/* Đặt hệ số hiệu chuẩn */
void HX711_SetScale(float newScale);

/* Lấy khối lượng theo kg */
float HX711_GetWeight(void);

#endif