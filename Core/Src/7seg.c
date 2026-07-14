
 // Điều khiển LED 7 đoạn 2 chữ số


#include "7seg.h"
#include "main.h"

static int DisplayValue = 0;
static int pos = 0;

/*
 * bit0 -> A
 * bit1 -> B
 * bit2 -> C
 * bit3 -> D
 * bit4 -> E
 * bit5 -> F
 * bit6 -> G
 * bit7 -> DP
 */
static const uint8_t Mask[10] =
{
    0x3F,   //0
    0x06,   //1
    0x5B,   //2
    0x4F,   //3
    0x66,   //4
    0x6D,   //5
    0x7D,   //6
    0x07,   //7
    0x7F,   //8
    0x6F    //9
};

void Set7SegDisplayValue(int value)
{
    if(value < 0)
        value = 0;

    if(value > 99)
        value = 99;

    DisplayValue = value;
}


void Run7SegDisplay(void)
{
    uint8_t val;

    pos++;

    /* Tắt cả hai LED */
    HAL_GPIO_WritePin(DIGIT1_GPIO_Port, DIGIT1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DIGIT2_GPIO_Port, DIGIT2_Pin, GPIO_PIN_RESET);

    /* Chọn số cần hiển thị */
    if(pos & 1)
        val = Mask[DisplayValue % 10];
    else
        val = Mask[(DisplayValue / 10) % 10];

    /* Xuất dữ liệu ra các đoạn */

    HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin,
            (val & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin,
            (val & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin,
            (val & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin,
            (val & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin,
            (val & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin,
            (val & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin,
            (val & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(SEG_DP_GPIO_Port, SEG_DP_Pin,
            (val & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* Bật LED tương ứng */

    if(pos & 1)
    {
        HAL_GPIO_WritePin(DIGIT1_GPIO_Port,
                          DIGIT1_Pin,
                          GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(DIGIT2_GPIO_Port,
                          DIGIT2_Pin,
                          GPIO_PIN_SET);
    }
}
