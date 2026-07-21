# BÁO CÁO BÀI TẬP LỚN HỆ NHÚNG - CÂN SỨC KHOẺ

Dự án thiết kế một thiết bị cân kỹ thuật số sử dụng vi điều khiển STM32F429-DISC. Nó đọc dữ liệu từ một cảm biến lực thông qua ADC, xử lý các phép đo và hiển thị trọng lượng đã tính toán trên màn hình LED 7 thanh. Trọng lượng cũng được truyền qua UART về máy tính kết hợp với tính năng quét thẻ RFID để định danh người, phục vụ lưu trữ và theo dõi cân nặng theo thời gian.

## GIỚI THIỆU

__Đề bài__: Thiết kế cân đo trọng lượng người, hiển thị trên màn hình LED 7 thanh và gửi kết quả về PC qua UART. Tính năng mở rộng: ghét nổi RC522 để quét thẻ RFID, định danh người cân, lưu trữ và theo dõi cân nặng theo thời gian

__Sản phẩm:__
1. Thiết kế chế tạo cân đo trọng lượng của người, hiển thị trên màn hình LED 7 thanh
2. Gửi kết quả cân nặng về máy tính (PC) thông qua UART.
3. Quét thẻ RFID để định danh người cân, lưu trữ và theo dõi cân nặng theo thời gian

## TÁC GIẢ

- Tên nhóm: 10h37
- Thành viên trong nhóm
  |STT|Họ tên|MSSV|Công việc|
  |--:|--|--|--|
  |1|Lê Văn Được|20225296|Phát triển cảm biến và thuật toán cân: SPI đọc dữ liệu từ HX711, viết hàm getHX711(), microDelay() và weigh(); Tối ưu và kiểm thử phần đọc/hiển thị dữ liệu từ load cell, Unit-test thuật toán đo để xác nhận độ ổn định, không lỗi tràn; Vẽ sơ đồ schematic|
  |2|Nguyễn Hoàng Giang|20225125|Hiển thị LED 7‑segment; Cấu hình Timer 6 (MX_TIM6_Init) và callback HAL_TIM_PeriodElapsedCallback cho multiplexing LED; Kiểm tra hiển thị đúng số, không bị flicker, ánh sáng đều; Viết báo cáo|
  |3|Hoàng Trung Hải|20225307|Cấu hình MX_SPI4_Init để kết nối với RC522 qua SPI; Viết hàm đọc card (TM_MFRC522_Check) và xử lý logic: Cập nhật DuLieuCanNang nếu tồn tại và thêm mới nếu là thẻ mới; Cấu hình UART1 (MX_USART1_UART_Init) để truyền kết quả cân qua máy tính sử dụng Hercules|

## MÔI TRƯỜNG HOẠT ĐỘNG

- **Module CPU/dev kit:** STM32F429-DISC
- **Kit, module phần cứng được sử dụng**
  + **STM32F429I Discovery kit**
  + **Module HX711:** bộ chuyển đổi tín hiệu ADC để đọc tín hiệu từ cảm biến lực, chuyển đổi lực tác động thành tín hiệu điện
  + **Module RC522:** sử dụng để đọc và ghi dữ liệu cho thẻ RFID / NFC
- **Phần mềm và công cụ phát triển**
  + **STM32CubeIDE:** môi trường phát triển tích hợp (IDE) dùng để lập trình và debug vi điều khiển STM32.
  + **Hercules:** giao tiếp, kiểm tra và mô phỏng các kết nối nối tiếp (UART)

## SƠ ĐỒ SCHEMATIC

![Untitled Sketch 2_bb](https://github.com/user-attachments/assets/2107ced8-3b4a-4645-872c-b71cd2a6ddce)



### TÍCH HỢP HỆ THỐNG

- Mô tả các thành phần phần cứng và vai trò của chúng: máy chủ, máy trạm, thiết bị IoT, MQTT Server, module cảm biến IoT...


| Thành phần              | Vai trò chính                                                                 |
|--------------------------|------------------------------------------------------------------------------|
| **Thiết bị IoT (STM32 + Load Cell + RC522)** | Thiết bị nhúng trung tâm, có nhiệm vụ đọc giá trị cân nặng, nhận dạng người dùng qua RFID, hiển thị và gửi dữ liệu |
| **Load Cell + HX711**   | Cảm biến đo trọng lượng, chuyển tín hiệu analog sang số (digital) qua HX711 |
| **Module RFID RC522**   | Quét thẻ RFID, xác định danh tính người cân để lưu thông tin cá nhân|
| **LED 7 đoạn**          | Hiển thị trọng lượng đã đo được một cách trực quan|
| **PC (Máy trạm)**        | Nhận dữ liệu từ STM32 qua UART, có thể hiển thị lên giao diện terminal (VD: Hercules)|

  
- Mô tả các thành phần phần mềm và vai trò của chúng, vị trí nằm trên phần cứng nào: Front-end, Back-end, Worker, Middleware...


| Thành phần              | Vai trò                                                                                 | Vị trí triển khai       |
|--------------------------|------------------------------------------------------------------------------------------|--------------------------|
| **Embedded Software (firmware)** | Điều khiển phần cứng STM32, đọc giá trị từ cảm biến, quét RFID, hiển thị và gửi dữ liệu | Trên **STM32**          |
| **UART Communication Handler** | Quản lý việc truyền dữ liệu từ STM32 sang máy tính                                 | Trên **STM32** & **PC** |
| **Front-end (PC Terminal hoặc giao diện GUI)** | Giao diện người dùng để xem dữ liệu cân nặng, mã RFID, thời gian (trên Hercules) | Trên **PC**             |
| **Data Logger / Storage**      | Lưu trữ dữ liệu theo thời gian để phục vụ theo dõi sức khỏe, thống kê               | Trên **PC** |


### ĐẶC TẢ HÀM



  
**I.File 7seg.c**\
\
1.Hàm Set7SegDisplayValue(int val)
  
```cpp
void Set7SegDisplayValue(int val)
{
    DisplayValue = val;
    pos = 0;
}
/*
  HÀM Set7SegDisplayValue(int val) (7seg.c)

  . MỤC ĐÍCH: Thiết lập giá trị cần hiển thị trên LED 7 đoạn

  . THAM SỐ:
      - Input: int val - Giá trị số cần hiển thị (0-99)
      - Output: không có (void)

  . HOẠT ĐỘNG/CHỨC NĂNG:
     - DisplayValue = val;  // Lưu giá trị cần hiển thị
     - pos = 0;               // Reset vị trí multiplexing
 */
```
2.Hàm Run7SegDisplay()
```cpp
void Run7SegDisplay()
{
	unsigned char val;
	pos++;
	HAL_GPIO_WritePin(PORT_7SEG_CONTROL0, PIN_7SEG_CONTROL0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(PORT_7SEG_CONTROL1, PIN_7SEG_CONTROL1, GPIO_PIN_RESET);

	if (pos & 0x1)
		val = Mask[DisplayValue % 10];
	else
		val = Mask[(DisplayValue / 10) % 10];

	if (val & 0x80)
		HAL_GPIO_WritePin(PORT_7SEG_P, PIN_7SEG_P, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_P, PIN_7SEG_P, GPIO_PIN_RESET);

	if (val & 0x40)
		HAL_GPIO_WritePin(PORT_7SEG_G, PIN_7SEG_G, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_G, PIN_7SEG_G, GPIO_PIN_RESET);

	if (val & 0x20)
		HAL_GPIO_WritePin(PORT_7SEG_F, PIN_7SEG_F, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_F, PIN_7SEG_F, GPIO_PIN_RESET);

	if (val & 0x10)
		HAL_GPIO_WritePin(PORT_7SEG_E, PIN_7SEG_E, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_E, PIN_7SEG_E, GPIO_PIN_RESET);

	if (val & 0x8)
		HAL_GPIO_WritePin(PORT_7SEG_D, PIN_7SEG_D, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_D, PIN_7SEG_D, GPIO_PIN_RESET);

	if (val & 0x4)
		HAL_GPIO_WritePin(PORT_7SEG_C, PIN_7SEG_C, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_C, PIN_7SEG_C, GPIO_PIN_RESET);

	if (val & 0x2)
		HAL_GPIO_WritePin(PORT_7SEG_B, PIN_7SEG_B, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_B, PIN_7SEG_B, GPIO_PIN_RESET);

	if (val & 0x1)
		HAL_GPIO_WritePin(PORT_7SEG_A, PIN_7SEG_A, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_A, PIN_7SEG_A, GPIO_PIN_RESET);


	if (pos & 0x1)
		HAL_GPIO_WritePin(PORT_7SEG_CONTROL0, PIN_7SEG_CONTROL0, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(PORT_7SEG_CONTROL1, PIN_7SEG_CONTROL1, GPIO_PIN_SET);
}

/*
  HÀM Run7SegDisplay() (7seg.c)

  . MỤC ĐÍCH: Thực hiện multiplexing để hiển thị 2 digit trên LED 7 đoạn

  . THAM SỐ:
      - Input: Không có
      - Output: không có (void)

  . HOẠT ĐỘNG/CHỨC NĂNG:
      pos++;  // Tăng bộ đếm vị trí để chuyển đổi giữa các chữ số

      // Tắt tất cả LED 7 đoạn
      HAL_GPIO_WritePin(PORT_7SEG_CONTROL0, PIN_7SEG_CONTROL0, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(PORT_7SEG_CONTROL1, PIN_7SEG_CONTROL1, GPIO_PIN_RESET);
      
      // Chọn chữ số hiển thị dựa trên vị trí quét
      if (pos & 0x1)
          val = Mask[DisplayValue % 10];      // Vị trí lẻ: hàng đơn vị
      else
          val = Mask[(DisplayValue / 10) % 10]; // Vị trí chẵn: hàng chục
      
      // Xuất mã LED cho từng đoạn (kiểm tra từng bit và điều khiển GPIO tương ứng)
      if (val & 0x80) HAL_GPIO_WritePin(PORT_7SEG_P, PIN_7SEG_P, GPIO_PIN_SET);
      if (val & 0x40) HAL_GPIO_WritePin(PORT_7SEG_G, PIN_7SEG_G, GPIO_PIN_SET);
      // ... tương tự cho các đoạn F, E, D, C, B, A
      
      // Bật LED tương ứng với vị trí đang quét
      if (pos & 0x1)
          HAL_GPIO_WritePin(PORT_7SEG_CONTROL0, PIN_7SEG_CONTROL0, GPIO_PIN_SET);
      else
          HAL_GPIO_WritePin(PORT_7SEG_CONTROL1, PIN_7SEG_CONTROL1, GPIO_PIN_SET);

   . NGUYÊN LÝ HOẠT ĐỘNG: Sử dụng kỹ thuật multiplexing - chỉ hiển thị 1 chữ số tại 1 thời điểm nhưng chuyển đổi nhanh để mắt thấy cả 2 chữ số sáng đồng thời.
 */

```

**II.File main.c**\
\
**1.Hàm điều khiển HX711**\
1.1.Hàm microDelay(uint16_t delay)
```cpp
void microDelay(uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  while (__HAL_TIM_GET_COUNTER(&htim2) < delay);
}
/*
  HÀM microDelay(uint16_t delay) (main .c)

  . MỤC ĐÍCH: Tạo độ trễ chính xác ở mức microsecond cho giao tiếp HX711

  . THAM SỐ:
      - Input: uint16_t delay - Thời gian delay (0-65535 μs)
      - Output: không có (void)

  . HOẠT ĐỘNG/CHỨC NĂNG:
     
	  __HAL_TIM_SET_COUNTER(&htim2, 0);          // Reset bộ đếm Timer 2 về 0
	  while (__HAL_TIM_GET_COUNTER(&htim2) < delay); // Chờ đợi cho đến khi bộ đếm đạt giá trị delay

	- Sử dụng Timer 2 làm timebase cho độ trễ chính xác
	- Độ trễ được tính bằng microsecond dựa trên tần số timer
 */
```
1.2.Hàm getHX711(void)
```cpp
int32_t getHX711(void)
{
  uint32_t data = 0;
  uint32_t startTime = HAL_GetTick();
  while(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
  {
    if(HAL_GetTick() - startTime > 200)
      return 0;
  }
  for(int8_t len=0; len<24 ; len++)
  {
    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
    microDelay(1);
    data = data << 1;
    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
    microDelay(1);
    if(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
      data ++;
  }
  data = data ^ 0x800000;
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
  microDelay(1);
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
  microDelay(1);
  return data;
}
/*
  HÀM  getHX711(void) (main .c)

  . MỤC ĐÍCH:  Đọc dữ liệu thô từ chip HX711 (ADC 24-bit cho cảm biến cân)

  . THAM SỐ:
      - Input: Không có
      - Output: int32_t - Giá trị ADC 24-bit (0 nếu timeout)

  . HOẠT ĐỘNG/CHỨC NĂNG:
     
	  uint32_t data = 0;
	  uint32_t startTime = HAL_GetTick();
	  
	  // Chờ HX711 sẵn sàng (DT pin LOW)
	  while(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
	  {
	    if(HAL_GetTick() - startTime > 200)      // Timeout sau 200ms
	      return 0;                              // Trả về 0 nếu không có phản hồi
	  }
	  
	  // Đọc 24 bit dữ liệu theo giao thức serial
	  for(int8_t len=0; len<24 ; len++)
	  {
	    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);  // Clock HIGH
	    microDelay(1);                                       // Độ trễ 1μs
	    data = data << 1;                                    // Dịch bit sang trái
	    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET); // Clock LOW
	    microDelay(1);                                       // Độ trễ 1μs
	    if(HAL_GPIO_ReadPin(DT_PORT, DT_PIN) == GPIO_PIN_SET)
	      data++;                                            // Đọc bit dữ liệu
	  }
	  
	  data = data ^ 0x800000;                    // Chuyển đổi two's complement
	  
	  // Tạo xung clock thứ 25 để chọn gain = 128 cho channel A
	  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
	  microDelay(1);
	  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
	  microDelay(1);
	  
	  return data;                               // Trả về giá trị ADC 24-bit
 */
```
1.3.Hàm weigh()
```cpp
int weigh()
{
  int32_t  total = 0;
  int32_t  samples = 50;
  int milligram;
  float coefficient;
  for(uint16_t i=0 ; i<samples ; i++)
  {
      total += getHX711();
  }
  int32_t average = (int32_t)(total / samples);
  coefficient = knownOriginal / knownHX711;
  milligram = (int)(average-tare)*coefficient;
  return milligram;
}
/*
  HÀM weigh() (main .c)

  . MỤC ĐÍCH: Tính toán trọng lượng thực tế từ giá trị ADC

  . THAM SỐ:
      - Input: Không có
      - Output: int - Trọng lượng tính bằng milligram

  . HOẠT ĐỘNG/CHỨC NĂNG:

	  int32_t total = 0;
	  int32_t samples = 50;                      // Lấy 50 mẫu để tính trung bình
	  int milligram;
	  float coefficient;
	  
	  // Lấy 50 mẫu ADC để giảm nhiễu
	  for(uint16_t i=0; i<samples; i++)
	  {
	    total += getHX711();                     // Cộng dồn các giá trị đọc được
	  }
	  
	  int32_t average = (int32_t)(total / samples);      // Tính giá trị trung bình
	  coefficient = knownOriginal / knownHX711;          // Hệ số hiệu chuẩn = 148000/165012
	  milligram = (int)(average-tare)*coefficient;       // Công thức: (ADC - Tare) × Hệ số
	  
	  return milligram;                                  // Trả về trọng lượng (mg)

 */
```
**2.Hàm khởi tạo hệ thống**\
2.1.Hàm SystemClock_Config()
```cpp
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}
/*
  HÀM SystemClock_Config(void) (main .c)

  . MỤC ĐÍCH: Cấu hình clock hệ thống STM32F4 hoạt động tối ưu

  . THAM SỐ:
      - Input: Không có
      - Output: Không có

  . HOẠT ĐỘNG/CHỨC NĂNG:

	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
	
	__HAL_RCC_PWR_CLK_ENABLE();                           // Kích hoạt clock PWR
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);  // Thiết lập voltage scaling
	
	// Cấu hình HSE và PLL
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;    // Sử dụng HSE
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;                     // Bật HSE
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                 // Bật PLL
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;         // PLL từ HSE
	RCC_OscInitStruct.PLL.PLLM = 4;                              // PLLM = 4
	RCC_OscInitStruct.PLL.PLLN = 180;                            // PLLN = 180
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                 // PLLP = 2
	
	// Cấu hình bus clock
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;    // SYSCLK từ PLL
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;          // AHB = 180MHz
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;           // APB1 = 45MHz
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;           // APB2 = 45MHz

 */
```
2.2.Hàm MX_GPIO_Init() 
```cpp
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE4 PE8 PE9 PE10
                           PE11 PE12 PE13 PE14
                           PE15 PE0 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PG2 PG3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/*
  HÀM MX_GPIO_Init(void) (main .c)

  . MỤC ĐÍCH: Khởi tạo các pin GPIO cho LED 7-segment, HX711, RFID

  . THAM SỐ:
      - Input: Không có
      - Output: Không có

  . HOẠT ĐỘNG/CHỨC NĂNG:
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	// Kích hoạt clock cho các port GPIO
	__HAL_RCC_GPIOE_CLK_ENABLE();                         // Kích hoạt GPIOE
	__HAL_RCC_GPIOH_CLK_ENABLE();                         // Kích hoạt GPIOH
	__HAL_RCC_GPIOA_CLK_ENABLE();                         // Kích hoạt GPIOA
	__HAL_RCC_GPIOG_CLK_ENABLE();                         // Kích hoạt GPIOG
	__HAL_RCC_GPIOC_CLK_ENABLE();                         // Kích hoạt GPIOC
	
	// Cấu hình các pin output cho LED 7-segment và HX711
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
	                     |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
	                     |GPIO_PIN_15|GPIO_PIN_0;           // Chọn các pin
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;            // Push-pull output
	GPIO_InitStruct.Pull = GPIO_NOPULL;                    // Không pull-up/down
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;           // Tốc độ thấp
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);               // Khởi tạo GPIOE
	
	// Cấu hình pin input cho HX711 DT
	GPIO_InitStruct.Pin = GPIO_PIN_1;                      // Pin PE1
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;                // Chế độ input
	GPIO_InitStruct.Pull = GPIO_NOPULL;                    // Không pull
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);               // Khởi tạo pin input

 */
```
**3.Hàm giao tiếp ngoại vi**\
3.1.Hàm MX_USART1_UART_Init()
```cpp
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/*
  HÀM MX_USART1_UART_Init(void) (main .c)

  . MỤC ĐÍCH: Khởi tạo UART1 cho giao tiếp với PC/thiết bị ngoài

  . THAM SỐ:
      - Input: Không có
      - Output: Không có

  . HOẠT ĐỘNG/CHỨC NĂNG:
	huart1.Instance = USART1;                             // Chọn UART1
	huart1.Init.BaudRate = 115200;                        // Tốc độ baud 115200
	huart1.Init.WordLength = UART_WORDLENGTH_8B;          // 8 bit dữ liệu
	huart1.Init.StopBits = UART_STOPBITS_1;               // 1 stop bit
	huart1.Init.Parity = UART_PARITY_NONE;                // Không parity
	huart1.Init.Mode = UART_MODE_TX_RX;                   // Cả TX và RX
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;          // Không flow control
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;      // Oversampling x16
	HAL_UART_Init(&huart1);                               // Khởi tạo UART

 */
```
3.2.Hàm MX_SPI4_Init()
```cpp
static void MX_SPI4_Init(void)
{

  /* USER CODE BEGIN SPI4_Init 0 */

  /* USER CODE END SPI4_Init 0 */

  /* USER CODE BEGIN SPI4_Init 1 */

  /* USER CODE END SPI4_Init 1 */
  /* SPI4 parameter configuration*/
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI4_Init 2 */

  /* USER CODE END SPI4_Init 2 */

}
/*
  HÀM MX_SPI4_Init(void) (main .c)

  . MỤC ĐÍCH: Khởi tạo SPI4 cho giao tiếp với module RFID MFRC522

  . THAM SỐ:
      - Input: Không có
      - Output: Không có

  . HOẠT ĐỘNG/CHỨC NĂNG:
	hspi4.Instance = SPI4;                                // Chọn SPI4
	hspi4.Init.Mode = SPI_MODE_MASTER;                    // STM32 là Master
	hspi4.Init.Direction = SPI_DIRECTION_2LINES;          // Full duplex
	hspi4.Init.DataSize = SPI_DATASIZE_8BIT;              // 8 bit data
	hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;            // Clock idle LOW
	hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;                // Data trên cạnh đầu
	hspi4.Init.NSS = SPI_NSS_SOFT;                        // Software NSS
	hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;  // Chia tần số /16
	hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;               // MSB trước
	HAL_SPI_Init(&hspi4);                                 // Khởi tạo SPI
 */
```
**4.Hàm timer và ngắt**\
4.1.Hàm MX_TIM6_Init()
```cpp
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 89;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 99;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}
/*
  HÀM MX_TIM6_Init(void) (main .c)

  . MỤC ĐÍCH: Khởi tạo Timer 6 cho việc quét LED 7-segment

  . THAM SỐ:
      - Input: Không có
      - Output: Không có

  . HOẠT ĐỘNG/CHỨC NĂNG:
	htim6.Instance = TIM6;                                  // Chọn Timer 6
	htim6.Init.Prescaler = 89;                             // Chia tần số: 90MHz/90 = 1MHz
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;           // Chế độ đếm lên
	htim6.Init.Period = 99;                                // Chu kỳ: 100us (1MHz/100)
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;  // Tắt preload
	HAL_TIM_Base_Init(&htim6);                             // Khởi tạo timer
 */
```
4.2.Hàm HAL_TIM_PeriodElapsedCallback()
```cpp
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    Run7SegDisplay(); // Gọi hàm quét LED trong ngắt timer
  }
}
/*
  HÀM HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) (main .c)

  . MỤC ĐÍCH: Hàm callback được gọi khi timer overflow

  . THAM SỐ:
      - Input: TIM_HandleTypeDef *htim - Con trỏ đến handle timer
      - Output: Không có

  . HOẠT ĐỘNG/CHỨC NĂNG:
	if (htim->Instance == TIM6)                    // Kiểm tra Timer 6
	{
	  Run7SegDisplay();                            // Gọi hàm quét LED trong ngắt timer
	}
 */
```
**5.Struct dữ liệu**\
5.1 Struct DuLieuCanNang
```cpp
typedef struct {
    int stt;              // Số thứ tự bản ghi
    uint8_t CardID[5];    // ID thẻ RFID (5 bytes)
    int canNang;          // Cân nặng tương ứng (mg)
} DuLieuCanNang;
/*
  . CHỨC NĂNG: Cấu trúc dữ liệu lưu trữ thông tin thẻ RFID và cân nặng
  . THÀNH PHẦN:
	stt: Định danh duy nhất cho mỗi bản ghi
	CardID: Mã định danh thẻ RFID
	canNang: Giá trị cân nặng được đo
*/
```
**6.Constants và biến toàn cục**\
6.1.Các hằng số hiệu chuẩn

```c
uint32_t tare = 8412745;          // Giá trị cân không tải
float knownOriginal = 148000;     // Khối lượng chuẩn (mg)
float knownHX711 = 165012;        // Giá trị ADC tương ứng
/*
  . CHỨC NĂNG: Các hằng số được sử dụng để hiệu chuẩn cân
  . Ý NGHĨA:
	tare: Giá trị ADC khi cân không có tải
	knownOriginal: Khối lượng thực của vật chuẩn
	knownHX711: Giá trị ADC đo được với vật chuẩn
*/
```
6.2.Định nghĩa pin HX711
```c
#define SCK_PIN   GPIO_PIN_0      // Pin clock
#define SCK_PORT  GPIOE
#define DT_PIN    GPIO_PIN_1      // Pin dữ liệu
#define DT_PORT   GPIOE
/*
  . CHỨC NĂNG: Định nghĩa các pin kết nối với HX711
  . Ý NGHĨA: Mapping phần cứng cho việc giao tiếp với load cell amplifier
*/
```
**7.Logic chính trong hàm main**\
7.1 Xử lý dữ liệu RFID và cân nặng
```c
// Đọc thẻ RFID
if(TM_MFRC522_Check(CardID) == MI_OK) {
    bool tonTai = false;
    
    // Kiểm tra thẻ đã tồn tại
    for(int i = 0; i <= stt; i++) {
        if(memcmp(duLieu[i].CardID, CardID, sizeof(CardID)) == 0) {
            tonTai = true;
            
            if(weight > 0) {
                // Cập nhật cân nặng mới
                duLieu[i].canNang = weight;
            } else {
                // Hiển thị dữ liệu cũ
                weight = duLieu[i].canNang;
                Set7SegDisplayValue(weight / 10);
            }
            break;
        }
    }
    
    // Thêm bản ghi mới
    if(!tonTai && weight > 0) {
        stt++;
        duLieu[stt].stt = stt;
        memcpy(duLieu[stt].CardID, CardID, sizeof(CardID));
        duLieu[stt].canNang = weight;
    }
}
/*
  . CHỨC NĂNG: Xử lý logic chính của hệ thống
  . Ý NGHĨA: Quản lý dữ liệu thẻ RFID và cân nặng, cập nhật hoặc thêm mới bản ghi
*/
```
**8.Kết Luận**\
Hệ thống cân thông minh RFID được thiết kế với các hàm chức năng rõ ràng:

	Đo lường: HX711 với độ chính xác cao
	Nhận dạng: RFID MFRC522
	Hiển thị: LED 7-segment với ngắt timer
	Giao tiếp: UART để debug và giám sát
	Lưu trữ: Cấu trúc dữ liệu trong RAM

### KẾT QUẢ

- Hiển thị cân nặng trên LED 7 thanh
![image](https://github.com/user-attachments/assets/120a02e9-63fb-435e-bae3-535e0e544a04)
- Gửi dữ liệu cân nặng về máy tính, hiển thị trên Hercules
  	+ Nếu không có thẻ
  	  
  	  ![image](https://github.com/user-attachments/assets/a8028080-c84a-4451-b938-65e9767cb10e)
  	+ Nếu có thẻ

  	  ![image](https://github.com/user-attachments/assets/9f214305-590d-4028-bd15-fde427d21c0b)
	+ Nếu cân nặng không thay đổi thì hiển thị dữ liệu cũ

	![image](https://github.com/user-attachments/assets/00909acb-81a4-4006-ba3d-814800e94cf1)


