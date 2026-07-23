# BÁO CÁO BÀI TẬP LỚN HỆ NHÚNG - CÂN SỨC KHOẺ

Dự án thiết kế một thiết bị cân kỹ thuật số sử dụng vi điều khiển STM32F429-DISC. Hệ thống đọc dữ liệu từ cảm biến lực (Loadcell) thông qua module chuyển đổi ADC **HX711**, xử lý tín hiệu để tính toán trọng lượng và hiển thị kết quả trên màn hình LED 7 thanh cũng như màn hình touch tích hợp trên kit. Người dùng có thể sử dụng nút nhấn B1 để chuyển đổi giữa các đơn vị đo. Dữ liệu trọng lượng được truyền về máy tính thông qua giao tiếp UART. Bên cạnh đó, hệ thống tích hợp module RFID RC522 để quét thẻ, định danh người sử dụng và kết hợp với đồng hồ thời gian thực (RTC) sử dụng chân VBAT để ghi nhận thời gian đo ngay cả khi mất nguồn. Nhờ đó, hệ thống có khả năng lưu trữ và theo dõi lịch sử cân nặng của từng người theo thời gian, phục vụ việc quản lý và đánh giá sự thay đổi cân nặng một cách thuận tiện.


## GIỚI THIỆU

__Đề bài__: Thiết kế chế tạo cân đo trọng lượng của người, hiển thị trên màn hình LED 7 thanh + gửi kết quả về PC qua UART.
Mở rộng: quét thẻ RFID để định danh người cân, lưu trữ và theo dõi cân nặng theo thời gian. Ghép nối RC522 để quét thẻ, và built-in Real-time clock (có pin vào chân VBAT) để giữ thời gian khi mạch mất điện.
Hiển thị khối lượng lên màn hình touch
Bấm nút B1 để chuyển đơn vị đo: kg, g, lbs

__Sản phẩm:__
1. Thiết kế chế tạo cân đo trọng lượng của người, hiển thị trên màn hình LED 7 thanh và màn hình touch
2. Gửi kết quả cân nặng về máy tính (PC) thông qua UART.
3. Quét thẻ RFID để định danh người cân, lưu trữ và theo dõi cân nặng theo thời gian

## TÁC GIẢ

- Tên nhóm: Thiếu 1 người
- Thành viên trong nhóm
  |STT|Họ tên|MSSV|Công việc|
  |--:|--|--|--|
  |1|Vũ Thúy Hằng|20225310|Phát triển module đo và xử lý trọng lượng: Thiết kế giao tiếp với Loadcell thông qua module HX711; xây dựng hàm đọc dữ liệu ADC 24-bit getHX711(); xây dựng hàm tạo độ trễ chính xác microDelay() bằng Timer; xử lý hiệu chuẩn (tare, knownHX711_raw, knownOriginal_g) và tính toán khối lượng thực tế qua hàm weigh_grams(). |
  |2|Ngô Minh Thu|20225411|Phát triển hệ thống hiển thị và điều khiển người dùng: Cấu hình Timer 6 (MX_TIM6_Init) và ngắt HAL_TIM_PeriodElapsedCallback() phục vụ quét LED 7 đoạn theo phương pháp multiplexing; điều khiển LED hiển thị khối lượng; tích hợp màn hình cảm ứng TFT ILI9341; in ra màn hình trạng thái, khối lượng, UID thẻ, đơn vị đo; xử lý nút B1 chuyển đổi đơn vị kg/g/lbs.|
  |3|Nguyễn Nhật Minh|20225210|Phát triển module nhận dạng và quản lý dữ liệu: Cấu hình giao tiếp SPI4 (MX_SPI4_Init) kết nối module RC522; tích hợp thư viện MFRC522; xây dựng chức năng đọc UID thẻ RFID thông qua hàm TM_MFRC522_Check(); xử lý quản lý dữ liệu người dùng (DuLieuCanNang), cập nhật cân nặng khi thẻ tồn tại và tạo bản ghi mới khi phát hiện thẻ mới; tích hợp module RTC DS1307 qua giao tiếp I2C để lấy thời gian đo|
  |4|Nguyễn Thị Thùy Linh |20225354| Phát triển giao tiếp và tích hợp hệ thống: Cấu hình UART1 (MX_USART1_UART_Init) truyền dữ liệu từ STM32 về máy tính; xây dựng định dạng dữ liệu gửi qua UART (khối lượng, UID, thời gian); kiểm tra giao tiếp bằng phần mềm Hercules.|


## MÔI TRƯỜNG HOẠT ĐỘNG

- **Module CPU/dev kit:** STM32F429-DISC
- **Kit, module phần cứng được sử dụng**
  + **STM32F429I Discovery kit**
  + **Module HX711:** bộ chuyển đổi tín hiệu ADC để đọc tín hiệu từ cảm biến lực, chuyển đổi lực tác động thành tín hiệu điện
  + **Module RC522:** sử dụng để đọc và ghi dữ liệu cho thẻ RFID / NFC
  + **Module Tiny RTC:** Cung cấp thông tin thời gian thực (ngày, giờ, phút, giây) và duy trì thời gian nhờ pin dự phòng khi hệ thống mất nguồn, phục vụ việc ghi nhận thời điểm đo cân nặng.
- **Phần mềm và công cụ phát triển**
  + **STM32CubeIDE:** môi trường phát triển tích hợp (IDE) dùng để lập trình và debug vi điều khiển STM32.
  + **Hercules:** giao tiếp, kiểm tra và mô phỏng các kết nối nối tiếp (UART)

## SƠ ĐỒ NGUYÊN LÝ
Hệ thống được xây dựng trên kit **STM32F429I-DISC1** sử dụng vi điều khiển **STM32F429ZIT6** làm bộ điều khiển trung tâm. STM32 có nhiệm vụ thu thập dữ liệu từ cảm biến tải trọng thông qua module HX711, nhận mã định danh từ đầu đọc RFID RC522, xử lý dữ liệu, hiển thị kết quả lên LED 7 đoạn và màn hình LCD TFT tích hợp sẵn trên kit, đồng thời truyền dữ liệu đến máy tính thông qua giao tiếp UART1.

Trong hệ thống, module **HX711** được kết nối với Loadcell để khuếch đại và chuyển đổi tín hiệu từ cảm biến tải trọng thành dữ liệu số. STM32 giao tiếp với HX711 bằng phương pháp **bit-banging** thông qua hai chân GPIO. Đầu đọc **RFID RC522** được kết nối với STM32 qua chuẩn giao tiếp **SPI4** để nhận UID của thẻ RFID. Kết quả đo được hiển thị trực tiếp trên **màn hình LCD ILI9341 tích hợp sẵn trên kit** và **LED 7 đoạn**. Đồng thời, dữ liệu được truyền đến máy tính qua **USART1** để phục vụ việc giám sát bằng phần mềm Hercules.

Ngoài ra, kit STM32F429I-DISC1 còn tích hợp sẵn **nút nhấn B1**, được sử dụng để chuyển đổi đơn vị hiển thị giữa **kg, g và lbs**. Trong khi đó, LED 7 đoạn luôn hiển thị giá trị theo đơn vị kilogram.

## ĐẤU NỐI PHẦN CỨNG

### 1. Kết nối Loadcell với HX711

Loadcell là cảm biến biến dạng (Strain Gauge), hoạt động theo nguyên lý cầu Wheatstone. Khi có tải trọng tác dụng, điện áp vi sai sinh ra có giá trị rất nhỏ (mức mV), không thể đưa trực tiếp vào vi điều khiển. Module HX711 đảm nhiệm việc khuếch đại tín hiệu và chuyển đổi sang dữ liệu số với độ phân giải 24 bit trước khi truyền đến STM32.

| Loadcell | HX711 |
|----------|-------|
| E+ | E+ |
| E− | E− |
| A+ | A+ |
| A− | A− |

Trong đó:

- **E+, E−:** cấp nguồn cho cầu Wheatstone bên trong Loadcell.
- **A+, A−:** truyền tín hiệu điện áp vi sai từ Loadcell đến HX711.
- HX711 thực hiện khuếch đại tín hiệu và chuyển đổi ADC 24 bit trước khi gửi dữ liệu về STM32.

---

### 2. Kết nối HX711 với STM32

HX711 giao tiếp với STM32 bằng giao thức nối tiếp hai dây tự cài đặt (bit-banging), không sử dụng ngoại vi SPI hay I2C phần cứng. Việc tạo xung clock và đọc dữ liệu được thực hiện hoàn toàn bằng GPIO trong chương trình.

| HX711 | STM32F429I-DISC1 | Định nghĩa trong chương trình |
|-------|------------------|-------------------------------|
| SCK | PE0 | `SCK_PORT`, `SCK_PIN` (GPIO Output) |
| DT | PE1 | `DT_PORT`, `DT_PIN` (GPIO Input) |
| VCC | 3.3V | |
| GND | GND | |

Trong quá trình hoạt động:

- STM32 tạo các xung clock trên chân **SCK** để dịch từng bit dữ liệu từ HX711.
- Sau mỗi xung clock, trạng thái chân **DT** được đọc để thu nhận dữ liệu 24 bit.
- Độ trễ giữa các xung clock được tạo bằng **TIM2** chạy ở chế độ free-running thông qua hàm `microDelay()`.

---

### 3. Kết nối RC522 với STM32

Đầu đọc RFID RC522 giao tiếp với STM32 thông qua **SPI4**, sử dụng thư viện `tm_stm32f4_mfrc522`. SPI4 được cấu hình với tốc độ khoảng **2.8125 Mbit/s** (Prescaler = 16).

| RC522 | STM32F429I-DISC1 | Ghi chú |
|-------|------------------|---------|
| SCK | PE2 | SPI4_SCK |
| MISO | PE5 | SPI4_MISO |
| MOSI | PE6 | SPI4_MOSI |
| SDA (CS) | GPIO | Điều khiển bằng GPIO (NSS Software) |
| RST | GPIO | Điều khiển bằng GPIO |
| VCC | 3.3V | |
| GND | GND | |


---

### 4. Kết nối màn hình LCD ILI9341

Màn hình TFT ILI9341 đã được tích hợp sẵn trên kit STM32F429I-DISC1.

Màn hình giao tiếp với STM32 thông qua **SPI5** ở chế độ **TX Only (Simplex Master)** vì chỉ ghi dữ liệu xuống LCD, không cần đọc ngược.

| Tín hiệu LCD | STM32F429I-DISC1 | Nhãn CubeMX |
|--------------|------------------|-------------|
| SCK | PF7 | SPI5_SCK |
| MOSI (SDI) | PF9 | SPI5_MOSI |
| CS | PC2 | LCD_CS |
| RST | PD12 | LCD_RST |
| DC | PD13 | LCD_DC |

SPI5 được cấu hình với tốc độ truyền khoảng **22.5 Mbit/s**, đáp ứng yêu cầu cập nhật giao diện nhanh trên màn hình TFT.

---

### 5. Kết nối LED 7 đoạn

Hệ thống sử dụng LED 7 đoạn hai chữ số để hiển thị nhanh giá trị khối lượng.

Việc điều khiển LED được thực hiện bằng phương pháp **quét Multiplex**, sử dụng các chân GPIO Output và ngắt định kỳ của **TIM6**. Mỗi lần TIM6 tràn, hàm `HAL_TIM_PeriodElapsedCallback()` sẽ gọi `Run7SegDisplay()` để chuyển đổi chữ số hiển thị.

| Chức năng | Số chân | GPIO |
|-----------|---------|------|
| Các đoạn A–G và DP | 8 | PE8–PE15, PG2, PG3 |
| Chọn Digit 1, Digit 2 | 2 | PE8–PE15, PG2, PG3 |

Việc ánh xạ chi tiết từng chân tới từng đoạn (A, B, C, D, E, F, G, DP) và các chân chọn Digit được khai báo trong file `7seg.h`.

---

### 6. Kết nối UART1

USART1 được sử dụng để truyền dữ liệu giữa STM32 và máy tính nhằm phục vụ việc giám sát và gỡ lỗi.

| STM32 (USART1) | Máy tính |
|---------------|----------|
| PA9 (TX) | RX |
| PA10 (RX) | TX |
| GND | GND |

Thông số cấu hình:

- Baudrate: **115200 bps**
- Data bits: **8 bit**
- Stop bits: **1 bit**
- Parity: **None**
- Flow Control: **None**

Dữ liệu truyền bao gồm:

- Giá trị khối lượng đo được.
- UID của thẻ RFID.
- Các thông báo trạng thái của hệ thống.

Thông tin được hiển thị trên phần mềm **Hercules**.

---

### 7. Nút bấm chuyển đơn vị

Kit STM32F429I-DISC1 sử dụng nút nhấn **B1** để thay đổi đơn vị hiển thị trên màn hình LCD.

| Tín hiệu | STM32 |
|----------|--------|
| B1 | PA0 (GPIO Input) |

Mỗi lần nhấn nút, đơn vị hiển thị sẽ chuyển theo chu kỳ:

```text
kg → g → lbs → kg → ...
```

Trong khi đó, **LED 7 đoạn luôn hiển thị giá trị theo đơn vị kilogram (kg)** và không thay đổi theo lựa chọn đơn vị trên LCD.

## TÍCH HỢP HỆ THỐNG

- Mô tả các thành phần phần cứng và vai trò của chúng: máy chủ, máy trạm, thiết bị IoT, MQTT Server, module cảm biến IoT...


| Thành phần              | Vai trò chính                                                                 |
|--------------------------|------------------------------------------------------------------------------|
| **Thiết bị IoT (STM32 + Load Cell + RC522 )** | Thiết bị nhúng trung tâm, có nhiệm vụ đọc giá trị cân nặng, nhận dạng người dùng qua RFID, hiển thị và gửi dữ liệu |
| **Load Cell + HX711**   | Cảm biến đo trọng lượng, chuyển tín hiệu analog sang số (digital) qua HX711 |
| **Module RFID RC522**   | Quét thẻ RFID, xác định danh tính người cân để lưu thông tin cá nhân|
| **RTC**   | Cung cấp thông tin thời gian thực (ngày, giờ, phút, giây) phục vụ ghi nhận thời điểm đo và theo dõi lịch sử cân nặng; vẫn duy trì thời gian khi mất nguồn nhờ pin dự phòng.|
| **LED 7 đoạn**          | Hiển thị trọng lượng đã đo được một cách trực quan|
| **Màn hình Touch LCD**       | Hiển thị chi tiết thông tin như khối lượng, đơn vị đo, UID người dùng; hỗ trợ giao diện trực quan.|
| **Nút B1**       | Cho phép người dùng chuyển đổi giữa các đơn vị đo kg, g và lbs.|
| **PC (Máy trạm)**        | Nhận dữ liệu từ STM32 qua UART, có thể hiển thị lên giao diện terminal (VD: Hercules)|

  
- Mô tả các thành phần phần mềm và vai trò của chúng, vị trí nằm trên phần cứng nào: Front-end, Back-end, Worker, Middleware...


| Thành phần              | Vai trò                                                                                 | Vị trí triển khai       |
|--------------------------|------------------------------------------------------------------------------------------|--------------------------|
| **Embedded Software (firmware)** | Điều khiển phần cứng STM32, đọc giá trị từ cảm biến, quét RFID, hiển thị và gửi dữ liệu | Trên **STM32**          |
| **UART Communication Handler** | Quản lý việc truyền dữ liệu từ STM32 sang máy tính                                 | Trên **STM32** & **PC** |
| **Front-end (PC Terminal hoặc giao diện GUI)** | Giao diện người dùng để xem dữ liệu cân nặng, mã RFID, thời gian (trên Hercules) | Trên **PC**             |
| **Data Logger / Storage**      | Lưu trữ dữ liệu theo thời gian để phục vụ theo dõi sức khỏe, thống kê               | Trên **PC** |


## ĐẶC TẢ HÀM



  
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
	  while (__HAL_TIM_GET_COUNTER(&htim2) < delay); //cpu kiểm tra giá trị bộ đếm Timer2 đến khi counter >= delay

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
1.3.Hàm weigh_grams()
```cpp
float weigh_grams(void)
{
  int32_t total = 0;
  const int32_t samples = 50;

  for (int32_t i = 0; i < samples; i++) {
    total += getHX711();
  }
  int32_t average = total / samples;

  float coefficient = knownOriginal_g / knownHX711_raw; // gram / (don vi raw)
  float grams = (float)(average - tare) * coefficient;

  if (grams < 0) grams = 0;
  return grams;
}
/*
  HÀM weigh() (main .c)

  . MỤC ĐÍCH: chuyển đổi giá trị RAW từ HX711 thành khối lượng thực tế theo đơn vị gram.

  . THAM SỐ:
      - Input: Không có
      - Output: float - khối lượng đo được bằng 

  . HOẠT ĐỘNG/CHỨC NĂNG:

	  int32_t total = 0;
	  const int32_t samples = 50;                   // Lấy 50 mẫu để tính trung bình
	  int milligram;
	  float coefficient;
	  
	  // Lấy 50 mẫu ADC để giảm nhiễu
	  for(int32_t i=0; i<samples; i++)
	  {
	    total += getHX711();                     // Cộng dồn các giá trị đọc được
	  }
	  
	  int32_t average = total / samples;      // Tính giá trị trung bình
	  float coefficient = knownOriginal_g / knownHX711_raw;          
	  float grams = (float)(average - tare) * coefficient;      // Công thức: (ADC - Tare) × Hệ số
	  
	  if (grams < 0) grams = 0;
      return grams;

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


