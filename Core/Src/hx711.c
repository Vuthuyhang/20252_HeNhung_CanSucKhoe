// #include "hx711.h"

// extern TIM_HandleTypeDef htim2;

// static int32_t tare = 0; //gtri offset
// static float scale = 1.0f; //he so chuyen doi

// //static const float knownOriginal = 148000.0f; //mg
// //static const float knownHX711 = 165012.0f; //ADC

// //delay micro giây
// void microDelay(uint16_t delay){
// 	__HAL_TIM_SET_COUNTER(&htim2, 0);
// 	while (__HAL_TIM_GET_COUNTER(&htim2) < delay);
// }

// //khoi tao hx711
// void HX711_Init(void)
// {
// 	HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
// 	HX711_Tare();
// }

// //đọc giữ liệu thô
// int32_t HX711_ReadRaw(void)
// {
// 	uint32_t data = 0;
// 	uint32_t startTime = HAL_GetTick();

// 	/* chờ HX711 sẵn sàng */
// 	while(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET){
// 		if(HAL_GetTick() - startTime > 200){
// 			return 0;
// 		}
// 	}

// 	//doc 24 bit du lieu
// 	for(uint8_t i = 0; i<24; i++){
// 		HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
// 		microDelay(1);
// 		data <<= 1;
// 		HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
// 		microDelay(1);
// 		if(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET){
// 			data++;
// 		}
// 	}

// 	//chuyen doi so am
// 	data ^= 0x800000;

// 	//clock thu 25

// 	HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
// 	microDelay(1);
// 	HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
// 	microDelay(1);

//     return (int32_t)data;
// }
// //dat gia tri tare
// void HX711_Tare(void)
// {
// 	int64_t sum = 0;
// 	for(uint8_t i = 0; i < 20; i++){
// 		sum += HX711_ReadRaw();
// 	}

// 	tare = (int32_t)(sum/20);
// }

// float HX711_GetWeight(void)
// {
// 	int64_t sum = 0;
// 	for(uint8_t i = 0; i < 20; i++){
// 		sum += HX711_ReadRaw();
// 	}
// 	int32_t average = (int32_t)(sum/20);

//     return (average - tare) * scale;
// }


#include "hx711.h"

extern TIM_HandleTypeDef htim2;

/* Giá trị ADC khi không tải */
static int32_t tare = 0;

/*
 * Hệ số chuyển đổi từ ADC sang kg.
 *
 * Công thức:
 * scale = knownWeightKg / (loadedADC - tare)
 */
static float scale = 1.0f;

/* Hàm nội bộ, không cần khai báo trong hx711.h */
void microDelay(uint16_t delay)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);

    while (__HAL_TIM_GET_COUNTER(&htim2) < delay)
    {
    }
}

/* Đọc trung bình nhiều mẫu */
static int32_t HX711_ReadAverage(uint8_t samples)
{
    int64_t sum = 0;

    for (uint8_t i = 0; i < samples; i++)
    {
        sum += HX711_ReadRaw();
    }

    return (int32_t)(sum / samples);
}

/* Khởi tạo HX711 */
void HX711_Init(void)
{
    /*
     * SCK phải ở mức thấp để HX711 hoạt động bình thường.
     */
    HAL_GPIO_WritePin(
        SCK_PORT,
        SCK_PIN,
        GPIO_PIN_RESET
    );

    /*
     * Chờ HX711 và loadcell ổn định sau khi cấp nguồn.
     */
    HAL_Delay(500);

    /*
     * Đưa cân về 0.
     * Khi gọi hàm này không được đặt vật lên cân.
     */
    HX711_Tare();
}

/* Đọc dữ liệu ADC thô 24 bit */
int32_t HX711_ReadRaw(void)
{
    uint32_t data = 0;
    uint32_t startTime = HAL_GetTick();

    /*
     * Khi DT xuống mức thấp nghĩa là HX711 đã có dữ liệu mới.
     */
    while (HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
    {
        if ((HAL_GetTick() - startTime) > 200U)
        {
            /*
             * Timeout: HX711 không phản hồi.
             */
            return 0;
        }
    }

    /* Đọc 24 bit dữ liệu */
    for (uint8_t i = 0; i < 24; i++)
    {
        HAL_GPIO_WritePin(
            SCK_PORT,
            SCK_PIN,
            GPIO_PIN_SET
        );

        microDelay(1);

        /*
         * Dịch dữ liệu cũ sang trái trước khi đọc bit mới.
         */
        data <<= 1;

        /*
         * Phải đọc DT khi SCK đang ở mức cao.
         */
        if (HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
        {
            data |= 1U;
        }

        HAL_GPIO_WritePin(
            SCK_PORT,
            SCK_PIN,
            GPIO_PIN_RESET
        );

        microDelay(1);
    }

    /*
     * Xung thứ 25:
     * chọn Channel A, Gain 128 cho lần đọc sau.
     */
    HAL_GPIO_WritePin(
        SCK_PORT,
        SCK_PIN,
        GPIO_PIN_SET
    );

    microDelay(1);

    HAL_GPIO_WritePin(
        SCK_PORT,
        SCK_PIN,
        GPIO_PIN_RESET
    );

    microDelay(1);

    /*
     * HX711 trả về số có dấu 24 bit.
     * Nếu bit 23 bằng 1 thì mở rộng dấu lên 32 bit.
     */
    if ((data & 0x800000U) != 0U)
    {
        data |= 0xFF000000U;
    }

    return (int32_t)data;
}

/* Đưa cân về 0 */
void HX711_Tare(void)
{
    tare = HX711_ReadAverage(20);
}

/* Đặt hệ số hiệu chuẩn */
void HX711_SetScale(float newScale)
{
    if (newScale != 0.0f)
    {
        scale = newScale;
    }
}

/* Đọc khối lượng theo kg */
float HX711_GetWeight(void)
{
    int32_t average = HX711_ReadAverage(10);

    /*
     * scale có đơn vị kg/count.
     */
    float weightKg =
        ((float)average - (float)tare) * scale;

    /*
     * Loại bỏ dao động nhỏ quanh 0.
     */
    if (weightKg > -0.03f && weightKg < 0.03f)
    {
        weightKg = 0.0f;
    }

    if (weightKg < 0.0f)
    {
        weightKg = 0.0f;
    }

    return weightKg;
}
