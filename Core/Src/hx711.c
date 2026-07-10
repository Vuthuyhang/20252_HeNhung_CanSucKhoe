#include "hx711.h"

extern TIM_HandleTypeDef htim2;

static int32_t tare = 0; //gtri offset
static float scale = 1.0f; //he so chuyen doi

//static const float knownOriginal = 148000.0f; //mg
//static const float knownHX711 = 165012.0f; //ADC

//delay micro giây
void microDelay(uint16_t delay){
	_HAL_TIM_SET_COUNTER(&htim2, 0);
	while(_HAL_TIM_SET_COUNTER(&htim2) < delay);
}

//khoi tao hx711
void HX711_Init(void)
{
	HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
	HX711_Tare();
}

//đọc giữ liệu thô
int32_t HX711_ReadRaw(void)
{
	uint32_t data = 0;
	uint32_t startTime = HAL_GetTick();

	/* chờ HX711 sẵn sàng */
	while(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET){
		if(HAL_GetTick() - startTime > 200){
			return 0;
		}
	}

	//doc 24 bit du lieu

    return 0;
}

void HX711_Tare(void)
{

}

float HX711_GetWeight(void)
{
    return 0;
}

