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

### 8. Kết nối RTC (DS1307 / DS3231) với STM32

Module Real-Time Clock (Tiny RTC) cung cấp thông tin thời gian thực (ngày, tháng, năm, giờ, phút, giây) cho hệ thống[cite: 2]. Module giao tiếp với STM32F429I-DISC1 thông qua chuẩn ngoại vi **I2C3** ở tốc độ 100 kHz[cite: 2].

| Tiny RTC | STM32F429I-DISC1 | Ghi chú |
|----------|------------------|---------|
| SCL | PA8 | I2C3_SCL |
| SDA | PC9 | I2C3_SDA |
| VCC | 3.3V | Nguồn cấp chính cho module |
| GND | GND | Chân mass chung |
| BAT | CR2032 (Nội bộ) | Pin dự phòng nuôi chip RTC khi mất điện |

Cấu hình I2C3:

- **Mode:** I2C Master
- **Clock Speed:** 100 kHz (Standard Mode)
- **Addressing Mode:** 7-bit
- **Địa chỉ I2C của DS1307/DS3231:** `0x68` (dạng 8-bit trong STM32 HAL: `0xD0`)

Trong quá trình hoạt động:

- **PA8 (SCL)** và **PC9 (SDA)** đảm nhận vai trò truyền nhận dữ liệu thời gian giữa vi điều khiển và module RTC qua chuỗi xung I2C.
- Dữ liệu thời gian đọc về ở dạng **BCD (Binary-Coded Decimal)** được vi điều khiển giải mã sang số nguyên Decimal và gắn vào bản ghi cân nặng của người dùng[cite: 2].
- Khi ngắt nguồn cấp chính (VCC), pin cúc áo **CR2032** kích hoạt để duy trì bộ đếm thời gian bên trong chip RTC[cite: 2].

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
1.4 convertWeight() 
```cpp
float convertWeight(float grams, WeightUnit unit)
{
    switch (unit)
    {
        case UNIT_KG:  return grams / 1000.0f;
        case UNIT_G:   return grams;
        case UNIT_LBS: return grams / 453.59237f;
        default:       return grams;
    }
}

/*
  HÀM convertWeight(float grams, WeightUnit unit) (main.c)

  . MỤC ĐÍCH:
      Quy đổi giá trị khối lượng từ đơn vị gốc (gram)
      sang đơn vị hiển thị do người dùng lựa chọn.

  . THAM SỐ:
      - Input:
          + float grams : Khối lượng theo đơn vị gram.
          + WeightUnit unit : Đơn vị cần chuyển đổi
            (UNIT_KG, UNIT_G, UNIT_LBS).

      - Output:
          + float : Giá trị khối lượng sau khi quy đổi.

  . HOẠT ĐỘNG/CHỨC NĂNG:
      - Nếu chọn UNIT_KG:
            grams / 1000
      - Nếu chọn UNIT_G:
            giữ nguyên giá trị.
      - Nếu chọn UNIT_LBS:
            grams / 453.59237
      - Nếu đơn vị không hợp lệ:
            trả về giá trị gram.

  . NGUYÊN LÝ HOẠT ĐỘNG:
      Hàm chỉ thực hiện quy đổi đơn vị hiển thị.
      Giá trị cân gốc luôn được lưu dưới đơn vị gram.
*/
```
1.5. Hàm weightToLed() 
```cpp
int weightToLed(float displayWeight)
{
    int ledValue = (int)(displayWeight + 0.5f);

    if (ledValue > LED_MAX_VALUE)
        ledValue = LED_MAX_VALUE;

    if (ledValue < 0)
        ledValue = 0;

    return ledValue;
}

/*
  HÀM weightToLed(float displayWeight) (main.c)

  . MỤC ĐÍCH:
      Chuyển giá trị khối lượng sang số nguyên
      để hiển thị trên LED 7 đoạn.

  . THAM SỐ:
      - Input:
          + float displayWeight:
            Giá trị khối lượng sau khi quy đổi.

      - Output:
          + int:
            Giá trị nguyên dùng để hiển thị LED.

  . HOẠT ĐỘNG/CHỨC NĂNG:
      - Làm tròn giá trị thực về số nguyên gần nhất.
      - Giới hạn giá trị hiển thị trong khoảng 0–99.
      - Trả về giá trị để hàm Set7SegDisplayValue()
        sử dụng.

  . NGUYÊN LÝ HOẠT ĐỘNG:
      LED 7 đoạn chỉ hiển thị được hai chữ số nên
      cần chuyển giá trị thực thành số nguyên và
      giới hạn trong phạm vi cho phép để tránh
      hiển thị sai hoặc tràn dữ liệu.
*/
```

**2. Các hàm điều khiển RTC (DS1307 / DS3231)**
2.1. Các hàm bổ trợ chuyển đổi BCD
```cpp
uint8_t BCD2DEC(uint8_t val) { return ((val / 16 * 10) + (val % 16)); } uint8_t DEC2BCD(uint8_t val) { return ((val / 10 * 16) + (val % 10)); } 
/*
  . MỤC ĐÍCH: Chuyển đổi qua lại giữa định dạng BCD (Binary-Coded Decimal) của thanh ghi RTC và số nguyên Decimal trong C.

  . THAM SỐ:
      - Input: uint8_t val (Giá trị số dạng BCD hoặc Decimal)
      - Output: uint8_t (Giá trị sau khi chuyển đổi)

  . HOẠT ĐỘNG/CHỨC NĂNG: Hàm BCD2DEC(val) (Chuyển từ BCD sang Decimal)
Hàm DEC2BCD(val) (Chuyển từ Decimal sang BCD).
 */
```
2.2. Hàm RTC_SetTime()
```cpp
void RTC_SetTime(uint8_t sec, uint8_t min, uint8_t hour, uint8_t date, uint8_t month, uint8_t year) { uint8_t set_time[7] = { DEC2BCD(sec), DEC2BCD(min), DEC2BCD(hour), DEC2BCD(1), // Mặc định thứ trong tuần = 1 DEC2BCD(date), DEC2BCD(month), DEC2BCD(year) }; HAL_I2C_Mem_Write(&hi2c3, RTC_DEV_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, set_time, 7, 100); } 
/*
  . MỤC ĐÍCH: Ghi cài đặt thời gian ban đầu vào các thanh ghi của module RTC qua bus I2C3.

  . THAM SỐ:
      - Input: sec, min, hour, date, month, year (Dạng số nguyên Decimal).
      - Output: Không có (void)

  . HOẠT ĐỘNG/CHỨC NĂNG: Chuyển đổi toàn bộ các tham số thời gian từ Decimal sang BCD bằng hàm DEC2BCD(). Lưu chuỗi 7 bytes dữ liệu vào mảng set_time. Gọi hàm HAL_I2C_Mem_Write() gửi mảng dữ liệu đến thanh ghi 0x00 của RTC qua bus hi2c3.
 */
```
2.3. Hàm RTC_SetTimeFromCompiler()
```cpp
void RTC_SetTimeFromCompiler(void) 
{
    char monthStr[4];
    int day, year, hour, min, sec;
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    // Parse ngày: "Mmm dd yyyy" (VD: "Jul 23 2026")
    sscanf(__DATE__, "%s %d %d", monthStr, &day, &year);
    // Parse giờ: "hh:mm:ss" (VD: "20:42:00")
    sscanf(__TIME__, "%d:%d:%d", &hour, &min, &sec);

    uint8_t month = 1;
    for (uint8_t i = 0; i < 12; i++) {
        if (strncmp(monthStr, months[i], 3) == 0) {
            month = i + 1;
            break;
        }
    }

    // Nạp thời gian máy tính vào RTC
    RTC_SetTime((uint8_t)sec, (uint8_t)min, (uint8_t)hour, (uint8_t)day, month, (uint8_t)(year % 100));
}
/*
  . MỤC ĐÍCH: Tự động trích xuất thời gian máy tính tại thời điểm biên dịch (Build code) và đồng bộ vào module RTC.

  . THAM SỐ:
      - Input: Không có.
      - Output: Không có (void).

  . HOẠT ĐỘNG/CHỨC NĂNG: Trích xuất chuỗi ngày/tháng/năm từ macro __DATE__ và giờ/phút/giây từ macro __TIME__ của GCC. Sử dụng sscanf() để phân tách chuỗi thành các giá trị số nguyên. Tra cứu chuỗi tên tháng tiếng Anh (Jan–Dec) để đổi sang số tháng tương ứng (1–12). Gọi RTC_SetTime() để nạp thời gian tự động vào RTC.
 */
```
2.4. Hàm RTC_GetTime()
```cpp
void RTC_GetTime(RTC_Time *time) 
{
    uint8_t get_time[7] = {0};

    // Đọc 7 thanh ghi từ địa chỉ 0x00 của RTC
    if (HAL_I2C_Mem_Read(&hi2c3, RTC_DEV_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, get_time, 7, 100) == HAL_OK) {
        time->sec   = BCD2DEC(get_time[0] & 0x7F); // Lọc bỏ bit Clock Halt
        time->min   = BCD2DEC(get_time[1] & 0x7F);
        time->hour  = BCD2DEC(get_time[2] & 0x3F); // Lọc bit 12/24h
        time->date  = BCD2DEC(get_time[4] & 0x3F);
        time->month = BCD2DEC(get_time[5] & 0x1F);
        time->year  = BCD2DEC(get_time[6]);
    } else {
        // Nếu I2C bị lỗi/mất kết nối với RTC thì gán về giá trị mặc định an toàn
        time->sec = 0; time->min = 0; time->hour = 0;
        time->date = 1; time->month = 1; time->year = 26;
    }
}
/*
  . MỤC ĐÍCH: Đọc dữ liệu thời gian thực hiện tại từ các thanh ghi của module RTC qua giao tiếp I2C3.

  . THAM SỐ:
      - Input: Con trỏ cấu trúc RTC_Time *time.
      - Output: Không có (void).

  . HOẠT ĐỘNG/CHỨC NĂNG: Sử dụng HAL_I2C_Mem_Read() để đọc 7 bytes dữ liệu từ địa chỉ thanh ghi 0x00 của RTC qua hi2c3. Nếu đọc thành công (HAL_OK): Áp dụng bộ lọc bitmask (0x7F, 0x3F, 0x1F) để loại bỏ bit điều khiển, dùng BCD2DEC() chuyển sang số Decimal và gán vào các trường tương ứng của time.Nếu đọc thất bại (Lỗi I2C/Tuột dây): Tự động gán giá trị mặc định an toàn để tránh in ra ký tự/số rác lên LCD và UART. 
 */
```



 **3. Hàm Timer và ngắt**

**3.1. Hàm `MX_TIM6_Init(void)`**

```cpp
/*
  HÀM MX_TIM6_Init(void) (main.c)

  . MỤC ĐÍCH:
      Khởi tạo Timer 6 để tạo ngắt định kỳ phục vụ việc quét LED 7 đoạn
      theo phương pháp Multiplexing.

  . THAM SỐ:
      - Input : Không có
      - Output: Không có (void)

  . HOẠT ĐỘNG/CHỨC NĂNG:
      - Chọn bộ định thời TIM6.
      - Thiết lập Prescaler để chia tần số xung Clock.
      - Thiết lập Period để xác định chu kỳ phát sinh ngắt.
      - Khởi tạo Timer bằng HAL_TIM_Base_Init().
      - Timer được khởi động bằng HAL_TIM_Base_Start_IT() để phát sinh
        ngắt định kỳ.

  . NGUYÊN LÝ HOẠT ĐỘNG:
      TIM6 hoạt động như bộ tạo thời gian (Time Base). Mỗi khi bộ đếm
      đạt giá trị Period, Timer tạo ngắt Update Event và gọi hàm
      HAL_TIM_PeriodElapsedCallback() để thực hiện quét LED 7 đoạn.
*/
```

---

**3.2. Hàm `HAL_TIM_PeriodElapsedCallback()`**

```cpp
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        Run7SegDisplay();
    }
}

/*
  HÀM HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) (main.c)

  . MỤC ĐÍCH:
      Xử lý ngắt Timer 6 và cập nhật hiển thị LED 7 đoạn.

  . THAM SỐ:
      - Input:
          + TIM_HandleTypeDef *htim:
            Con trỏ đến Timer phát sinh ngắt.
      - Output:
          Không có (void)

  . HOẠT ĐỘNG/CHỨC NĂNG:
      - Kiểm tra Timer phát sinh ngắt có phải TIM6 hay không.
      - Nếu đúng, gọi hàm Run7SegDisplay() để thực hiện quét LED
        theo phương pháp Multiplexing.
      - Việc quét được thực hiện liên tục với tần số cao giúp hai
        chữ số hiển thị ổn định, không bị nhấp nháy.

  . NGUYÊN LÝ HOẠT ĐỘNG:
      Đây là hàm callback của thư viện HAL, được gọi tự động khi
      TIM6 phát sinh ngắt Update Event. Chương trình không gọi trực
      tiếp hàm này trong vòng lặp chính.
*/
```
**4.Struct dữ liệu**\
4.1 Struct DuLieuCanNang
```cpp
typedef struct {
    int stt;              // Số thứ tự bản ghi
    uint8_t CardID[5];    // ID thẻ RFID (5 bytes)
    int canNang_g;          //Lưu trữ theo đơn vị gram
    RTC_Time timestamp;   // Thời gian đo từ RTC
} DuLieuCanNang;
/*
  . CHỨC NĂNG: Cấu trúc dữ liệu lưu trữ thông tin thẻ RFID và cân nặng
  . THÀNH PHẦN:
	stt: Định danh duy nhất cho mỗi bản ghi
	CardID: Mã định danh thẻ RFID
	canNang_g: Giá trị cân nặng được đo
            timestamp: Thời gian đo lấy từ RTC
*/
```
4.2. Struct RTC_Time
```cpp
typedef struct {
    uint8_t sec, min, hour, date, month, year;
} RTC_Time;
/*
  . CHỨC NĂNG: Cấu trúc dữ liệu lưu trữ thời gian
  . THÀNH PHẦN:
	sec: Giây
	min: Phút
	hour: Giờ
            day: Ngày
	month: Tháng
	year: Năm
*/
```


**5.Constants và biến toàn cục**\
5.1.Các hằng số hiệu chuẩn

```c
int32_t tare = 7723582;          // Giá trị cân không tải
float knownOriginal_g = 500.0f;     // Khối lượng chuẩn (mg)
float knownHX71_raw = -25760.0f;        // Giá trị ADC tương ứng
/*
  . CHỨC NĂNG: Các hằng số được sử dụng để hiệu chuẩn cân
  . Ý NGHĨA:
	tare: Giá trị ADC khi cân không có tải
	knownOriginal: Khối lượng thực của vật chuẩn
	knownHX711: Giá trị ADC đo được với vật chuẩn
*/
```
5.2.Định nghĩa pin HX711
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
**6.Khối khởi tạo màn hình LCD (SPI5 và ILI9341)**\


6.1. Các hàm điều khiển hiển thị LCD


```c
  CÁC HÀM ĐIỀU KHIỂN MÀN HÌNH LCD ILI9341 (Sử dụng từ file Driver)


  1. HÀM ILI9341_Init()
  . MỤC ĐÍCH: Khởi tạo phần cứng màn hình.
  . HOẠT ĐỘNG: Gửi chuỗi lệnh (Command) và dữ liệu (Data) qua SPI5 để cấu hình các thanh ghi của IC ILI9341 (như Power Control, Memory Access Control, Pixel Format), giúp màn hình thoát khỏi chế độ Sleep và sẵn sàng nhận dữ liệu ảnh.


  2. HÀM ILI9341_DrawText(const char* str, const uint8_t font[], uint16_t X, uint16_t Y, uint16_t color, uint16_t bgcolor)
  . MỤC ĐÍCH: In một chuỗi ký tự (String) lên màn hình.
  . HOẠT ĐỘNG: Tự động tính toán độ rộng của từng ký tự trong mảng dữ liệu font. Nó cộng dồn độ rộng thực tế của ký tự đang vẽ vào tọa độ X để xác định chính xác vị trí bắt đầu vẽ ký tự tiếp theo (Proportional spacing), giúp chuỗi hiển thị tự nhiên.
```



**.Logic chính trong hàm main**\

Sau khi khởi tạo toàn bộ các ngoại vi (GPIO, Timer, SPI, I2C, UART, LCD, HX711, RC522 và LED 7 đoạn), hệ thống đi vào vòng lặp vô hạn `while(1)` để thực hiện việc đo, hiển thị và quản lý dữ liệu theo thời gian thực.

### Các bước xử lý

#### 1. Đọc thời gian từ RTC

Hệ thống đọc thời gian hiện tại từ module DS1307 thông qua giao tiếp I2C. Thời gian được sử dụng để hiển thị trên UART và lưu cùng dữ liệu cân nặng của từng thẻ RFID.

#### 2. Kiểm tra nút B1

Nút B1 được sử dụng để chuyển đổi đơn vị hiển thị.

```
kg → g → lbs → kg
```

LCD sẽ cập nhật đơn vị mới, trong khi LED 7 đoạn luôn hiển thị giá trị theo đơn vị kilogram.

#### 3. Đọc và xử lý dữ liệu HX711

STM32 đọc 50 mẫu dữ liệu từ HX711 bằng phương pháp bit-banging, sau đó lấy giá trị trung bình để giảm nhiễu.

Khối lượng được tính theo công thức hiệu chuẩn:

```text
Weight = (Average - Tare) × CalibrationCoefficient
```

Giá trị sau đó được quy đổi sang đơn vị đang chọn (kg, g hoặc lbs).

#### 4. Cập nhật hiển thị

Sau khi tính toán, hệ thống đồng thời:

- Hiển thị khối lượng trên LCD.
- Hiển thị giá trị (kg) trên LED 7 đoạn.
- Gửi thời gian và khối lượng qua UART để theo dõi bằng Hercules.

#### 5. Quản lý dữ liệu RFID

Khi phát hiện thẻ RFID:

- Đọc UID của thẻ.
- Tìm UID trong danh sách đã lưu.

Nếu UID đã tồn tại:

- Có tải trọng → cập nhật cân nặng và thời gian đo.
- Không có tải trọng → hiển thị dữ liệu đã lưu trước đó.

Nếu UID chưa tồn tại:

- Có tải trọng → tạo bản ghi mới gồm UID, cân nặng và thời gian.
- Không có tải trọng → thông báo chưa có dữ liệu.

Nếu không phát hiện thẻ, hệ thống hiển thị trạng thái **"Đang chờ thẻ..."** và tiếp tục đo khối lượng theo thời gian thực.


**9.Kết Luận**\
Hệ thống cân thông minh RFID được thiết kế với các hàm chức năng rõ ràng:

	Đo lường: HX711 với độ chính xác cao
	Nhận dạng: RFID MFRC522
	Hiển thị: LED 7-segment với ngắt timer
	Giao tiếp: UART để debug và giám sát
	Lưu trữ: Cấu trúc dữ liệu trong RAM

### KẾT QUẢ

- Hiển thị cân nặng trên LED 7 thanh
<img width="3024" height="4032" alt="Image" src="https://github.com/user-attachments/assets/0b88e75f-bfb3-4985-a5ea-8cc5a6f73a2c" />

- Gửi dữ liệu cân nặng về máy tính, hiển thị trên Hercules
  <img width="772" height="682" alt="Screenshot 2026-07-24 003239" src="https://github.com/user-attachments/assets/bfd88232-d75b-450d-b877-36b376c95746" />
<img width="772" height="676" alt="Screenshot 2026-07-24 003153" src="https://github.com/user-attachments/assets/6c418fc2-c949-464e-bc56-e1cd929d9874" />
<img width="772" height="675" alt="Screenshot 2026-07-24 003138" src="https://github.com/user-attachments/assets/229d646f-a7e7-4fdd-a8de-c679f948e36f" />



