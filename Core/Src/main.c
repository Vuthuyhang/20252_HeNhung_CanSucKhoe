/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "tm_stm32f4_mfrc522.h"
#include "7seg.h"
#include <stdbool.h>
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
#include "fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define HX711_SCK_GPIO_Port GPIOB
#define HX711_SCK_Pin GPIO_PIN_6
#define HX711_DT_GPIO_Port GPIOB
#define HX711_DT_Pin GPIO_PIN_7

/* Bat CALIBRATION_MODE = 1 de CHI doc va in gia tri RAW cua HX711 qua UART
 * (dung luc chua dat gi / dat qua can chuan, de xac dinh tare va knownHX711_raw).
 * Sau khi hieu chuan xong, dat lai = 0 de chay che do can + LED + RFID binh thuong. */
#define CALIBRATION_MODE 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;

SPI_HandleTypeDef hspi4;
SPI_HandleTypeDef hspi5;
DMA_HandleTypeDef hdma_spi5_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
int TIM6_Count;
unsigned char LED_Value;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C3_Init(void);
static void MX_SPI4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM2_Init(void);
static void MX_SPI5_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* ---------- Du lieu luu tru theo the RFID ---------- */
typedef struct {
	int stt;
	uint8_t CardID[5];
	float canNang_g;     // Luon luu theo don vi goc: GRAM (khong phu thuoc don vi dang hien thi)
} DuLieuCanNang;

/* ---------- Chan HX711 ---------- */
#define SCK_PIN   GPIO_PIN_0
#define SCK_PORT  GPIOE
#define DT_PIN    GPIO_PIN_1
#define DT_PORT   GPIOE

/* ---------- Hieu chuan HX711 ---------- */
// tare: gia tri raw trung binh khi KHONG co tai (do bang cach in ra UART luc ban dau, khong dat gi len ban can)
int32_t tare = 7723582;

// Hieu chuan 2 diem: dat mot vat co khoi luong CHUAN (vd qua can 5kg) len ban can,
// ghi lai gia tri raw (average) tra ve, roi tinh:
//   knownHX711_raw = average_khi_co_vat - tare
//   knownOriginal_g = khoi luong that cua vat chuan (don vi: GRAM)
float knownOriginal_g = 500.0f;
float knownHX711_raw  = -25760.0f;

/* ---------- Don vi hien thi (chuyen bang nut B1) ---------- */
typedef enum {
	UNIT_KG = 0,
	UNIT_G,
	UNIT_LBS,
	UNIT_COUNT
} WeightUnit;

volatile WeightUnit currentUnit = UNIT_KG;
const char *unitName[UNIT_COUNT] = { "kg", "g", "lbs" };

/* Gioi han hien thi LED 7 doan: chi dung 2 LED 7 thanh -> hien thi duoc 0-99. */
#define LED_MAX_VALUE 99

/* ---------- Bo cuc man hinh (LCD 320 x 240, SCREEN_HORIZONTAL_1) ----------
 * Duoc dan trai ra toan bo chieu cao/chieu rong man hinh (thay vi chi
 * chiem 1 goc nho nhu truoc) va co khoang cach du de khong bi chong lan. */
#define LABEL_X        10     // Cot nhan (ten truong du lieu)
#define VALUE_X        150    // Cot gia tri (du lieu thay doi)

#define STATUS_Y       45     // Dong "Trang thai"
#define WEIGHT_Y       80     // Dong "Can nang"
#define CARDID_Y       125    // Dong "Ma the"
#define UNIT_Y         160    // Dong "Don vi"
#define COUNT_Y        190    // Dong "So the da luu"
#define FOOTER_LINE_Y  215    // Duong ke chan man hinh

/* Do rong CO DINH (so ky tu) cho tung truong gia tri. Moi lan ve lai, ca vung
 * nay se duoc ve chen khoang trang cho du day chieu rong -> du lieu cu, du
 * ngan hay dai hon du lieu moi, se luon bi xoa sach hoan toan (khong con bi
 * "dinh" lai tren man hinh gay hien tuong chu no de len chu kia). */
#define STATUS_FIELD_WIDTH   20
#define WEIGHT_FIELD_WIDTH   16
#define CARDID_FIELD_WIDTH   16
#define UNIT_FIELD_WIDTH       6
#define COUNT_FIELD_WIDTH      6

/* Thoi gian toi thieu (ms) 1 thong bao trang thai duoc GIU tren man hinh
 * truoc khi cho phep trang thai "Dang cho the..." (idle) ghi de len. Nho vay
 * khi vua quet the / vua doi don vi, nguoi dung van con kip doc duoc. */
#define STATUS_HOLD_MS      1500U

void microDelay(uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim2, 0);
  while (__HAL_TIM_GET_COUNTER(&htim2) < delay);
}

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

/**
 * @brief  Can va tra ve khoi luong o don vi GOC: GRAM (float).
 *         Day la ham "nguon su that" duy nhat cho khoi luong; moi noi khac
 *         (LED, UART, luu tru RFID) deu quy doi tu gia tri gram nay.
 */
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

/**
 * @brief Quy doi khoi luong tu gram sang don vi dang chon (kg/g/lbs) de hien thi.
 */
float convertWeight(float grams, WeightUnit unit)
{
  switch (unit) {
    case UNIT_KG:  return grams / 1000.0f;
    case UNIT_G:   return grams;
    case UNIT_LBS: return grams / 453.59237f;
    default:       return grams;
  }
}

/**
 * @brief Chuyen gia tri hien thi (da quy doi don vi) thanh so nguyen de day
 *        ra LED 7 doan, co gioi han (clamp) tranh tran so.
 */
int weightToLed(float displayWeight)
{
  int ledValue = (int)(displayWeight + 0.5f);
  if (ledValue > LED_MAX_VALUE) ledValue = LED_MAX_VALUE;
  if (ledValue < 0) ledValue = 0;
  return ledValue;
}

/**
 * @brief Ve 1 truong du lieu tai (x,y) voi do rong KY TU CO DINH (fieldWidth).
 *        Chuoi hien thi luon duoc dem them khoang trang cho du day do rong nay,
 *        nen du lieu cu (du dai hay ngan hon du lieu moi) se luon duoc xoa het,
 *        khac phuc loi "du lieu sau bi de/chong len du lieu truoc".
 */
static void LCD_DrawField(uint16_t x,
                          uint16_t y,
                          uint8_t fieldWidth,
                          const uint8_t font[],
                          const char *text,
                          uint16_t color)
{
    uint16_t charWidth;
    uint16_t charHeight;

    if(font == FONT4)
    {
        charWidth  = 16;
        charHeight = 26;
    }
    else if(font == FONT3)
    {
        charWidth  = 11;
        charHeight = 18;
    }
    else
    {
        charWidth  = 8;
        charHeight = 16;
    }

    // Xóa toàn bộ vùng trước
    ILI9341_DrawRectangle(
        x,
        y,
        fieldWidth * charWidth,
        charHeight,
        WHITE);

    // Vẽ lại nội dung mới
    ILI9341_DrawText(text, font, x, y, color, WHITE);
}

/**
 * @brief Hien thi dong "Trang thai" tren LCD, nhung GIU nguyen thong bao hien
 *        tai toi thieu STATUS_HOLD_MS truoc khi cho phep trang thai "idle"
 *        (Dang cho the...) ghi de len. Nho vay thong bao (vd "Da update data!",
 *        "Tao the moi!"...) khong bi xoa qua nhanh, du nguoi dung co doi don vi
 *        hay nhac the ra ngay sau do.
 */
static void ShowStatus(const char *text, uint16_t color)
{
  static char lastText[STATUS_FIELD_WIDTH + 1] = "";
  static uint32_t holdUntil = 0;
  uint32_t now = HAL_GetTick();
  uint8_t isIdle = (strcmp(text, "Dang cho the...") == 0);

  if (isIdle && now < holdUntil)
  {
    return; // Con dang trong thoi gian giu thong bao truoc do
  }

  if (strcmp(text, lastText) != 0)
  {
    LCD_DrawField(VALUE_X, STATUS_Y, STATUS_FIELD_WIDTH, FONT3, text, color);
    strncpy(lastText, text, sizeof(lastText) - 1);
    lastText[sizeof(lastText) - 1] = '\0';
  }

  if (!isIdle)
  {
    holdUntil = now + STATUS_HOLD_MS;
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C3_Init();
  MX_SPI4_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_SPI5_Init();
  /* USER CODE BEGIN 2 */
   HAL_TIM_Base_Start(&htim2); // Cho microDelay
   // Cac dong HX711 init
   HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET); // HX711 SCK init
   HAL_Delay(10);
   HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
   HAL_Delay(10);

   HAL_TIM_Base_Start_IT(&htim6); // Quan trong: Khoi dong TIM6 o che do ngat
   Set7SegDisplayValue(0); // Gia tri ban dau

   // static: nam trong vung .bss, KHONG chiem stack (tranh stack overflow voi mang 1000 phan tu)
   static DuLieuCanNang duLieu[1000];
   int stt = 0;          // So luong ban ghi da luu = index se ghi tiep theo
   char buf[100];
   char lcdBuf[40];      // Buffer dung rieng cho hien thi LCD

   TM_MFRC522_Init();

   uint8_t oldButton = GPIO_PIN_RESET;
   uint32_t lastButtonTick = 0;

   // ----------------------------------------------------
   // KHOI TAO LCD VA VE GIAO DIEN TINH (DAN TRAI TOAN BO MAN HINH 320x240)
   // ----------------------------------------------------
   ILI9341_Init();
   ILI9341_SetRotation(SCREEN_HORIZONTAL_1);
   ILI9341_FillScreen(WHITE);

   // ----- Tieu de + duong ke ngan cach -----
   ILI9341_DrawText("HE NHUNG 2025.2", FONT3, 10, 8, BLUE, WHITE);
   ILI9341_DrawHLine(0, 30, ILI9341_SCREEN_WIDTH, DARKGREY);

   // ----- Cac nhan (label) co dinh, khong doi trong suot qua trinh chay -----
   ILI9341_DrawText("Trang thai:",    FONT3, LABEL_X, STATUS_Y, BLACK, WHITE);
   ILI9341_DrawText("Can nang:",      FONT3, LABEL_X, WEIGHT_Y, BLACK, WHITE);
   ILI9341_DrawText("Ma the:",        FONT3, LABEL_X, CARDID_Y, BLACK, WHITE);
   ILI9341_DrawText("Don vi:",        FONT3, LABEL_X, UNIT_Y, BLACK, WHITE);
   ILI9341_DrawText("So the da luu:", FONT3, LABEL_X, COUNT_Y, BLACK, WHITE);

   // ----- Chan man hinh -----
   ILI9341_DrawHLine(0, FOOTER_LINE_Y, ILI9341_SCREEN_WIDTH, DARKGREY);
   ILI9341_DrawText("Nhan nut B1 de doi don vi kg / g / lbs", FONT3, 10, FOOTER_LINE_Y + 8, DARKGREY, WHITE);

   // ----- Gia tri khoi tao cho tung truong -----
   ShowStatus("Dang cho the...", DARKGREY);
   LCD_DrawField(VALUE_X, WEIGHT_Y, WEIGHT_FIELD_WIDTH, FONT4, "0.00 kg", RED);
   LCD_DrawField(VALUE_X, CARDID_Y, CARDID_FIELD_WIDTH, FONT3, "---", BLACK);
   LCD_DrawField(VALUE_X, UNIT_Y, UNIT_FIELD_WIDTH, FONT3, unitName[currentUnit], PURPLE);
   LCD_DrawField(VALUE_X, COUNT_Y, COUNT_FIELD_WIDTH, FONT3, "0", DARKGREEN);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  uint8_t CardID[5];

#if CALIBRATION_MODE
	  /* ============================================================
	   * CHE DO HIEU CHUAN: chi doc va in RAW cua HX711 qua UART.
	   * - Khong dat gi len ban can  -> ghi lai RAW nay lam gia tri "tare".
	   * - Dat qua can chuan (vd 5kg) len ban can -> ghi lai RAW nay, roi tinh:
	   *       knownHX711_raw = RAW_luc_co_qua_can - tare
	   * Sau khi co du 2 so tren, cap nhat vao "tare" va "knownHX711_raw" o dau
	   * file, roi dat CALIBRATION_MODE = 0 de chay che do can + LED + RFID.
	   * ============================================================ */
	  int32_t raw = getHX711();

	  sprintf(buf, "RAW = %ld", raw);
	  HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);
	  HAL_UART_Transmit(&huart1, (uint8_t *)"\r\n", 2, 100);

	  HAL_Delay(500);

#else
	  /* ---- 1. Doc nut B1 (PA0) de doi don vi, khong dung HAL_Delay chan chuong trinh ---- */
	  uint8_t newButton = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
	  if (newButton == GPIO_PIN_SET && oldButton == GPIO_PIN_RESET &&
	      (HAL_GetTick() - lastButtonTick) > 200)
	  {
	      currentUnit = (WeightUnit)((currentUnit + 1) % UNIT_COUNT);
	      lastButtonTick = HAL_GetTick();
	      // Cap nhat ngay dong "Don vi:" - dung field co dinh nen khong can xoa trang thu cong
	      LCD_DrawField(VALUE_X, UNIT_Y, UNIT_FIELD_WIDTH, FONT3, unitName[currentUnit], PURPLE);
	  }
	  oldButton = newButton;

	  /* ---- 2. HX711 luon duoc quet & cap nhat MOI VONG LAP (khong phu thuoc RFID/nut bam) ---- */
	  float weightGrams = weigh_grams();               // luon la gram, khong am
	  float displayWeight = convertWeight(weightGrams, currentUnit); // theo don vi dang chon hien tai

	  // LED 7 doan LUON hien thi theo KG, khong phu thuoc don vi dang chon boi nut B1
	  int ledValue = weightToLed(convertWeight(weightGrams, UNIT_KG));
	  Set7SegDisplayValue(ledValue);

	  // LCD hien thi can nang hien tai (thanh ghi real-time), do rong co dinh
	  // nen doi don vi (chuoi ngan/dai khac nhau) khong con de lai ky tu cu
	  sprintf(lcdBuf, "%.2f %s", displayWeight, unitName[currentUnit]);
	  LCD_DrawField(VALUE_X, WEIGHT_Y, WEIGHT_FIELD_WIDTH, FONT4, lcdBuf, RED);

	  sprintf(buf, "Can Nang: %.2f %s", displayWeight, unitName[currentUnit]);
	  HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 100);
	  HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, 100);
	  HAL_Delay(50);

	  /* ---- 3. Quet the RFID ---- */
	  if(TM_MFRC522_Check(CardID) == MI_OK){
	      sprintf(buf, "ID: %02X%02X%02X%02X%02X", CardID[0], CardID[1], CardID[2], CardID[3],CardID[4]);
	      HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 1000);
	      HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, 1000);

	      // Hien thi ma the vua quet duoc len dong "Ma the:"
	      sprintf(lcdBuf, "%02X%02X%02X%02X%02X", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
	      LCD_DrawField(VALUE_X, CARDID_Y, CARDID_FIELD_WIDTH, FONT3, lcdBuf, BLACK);

	      bool tonTai = false;

	      for(int i = 0; i < stt; i++){
	          if(memcmp(duLieu[i].CardID, CardID, sizeof(CardID)) == 0){
	              tonTai = true;

	              if(weightGrams > 0){
	                  // Cap nhat du lieu cu bang can nang moi vua do
	                  duLieu[i].canNang_g = weightGrams;

	                  // Hien thi LCD trang thai (duoc giu toi thieu STATUS_HOLD_MS)
	                  ShowStatus("Da update data!", DARKGREEN);

	                  float shown = convertWeight(duLieu[i].canNang_g, currentUnit);
	                  sprintf(buf, "Cap nhat du lieu: STT: %d , ID: %02X%02X%02X%02X%02X , Can Nang Moi: %.2f %s",
	                          duLieu[i].stt, duLieu[i].CardID[0], duLieu[i].CardID[1], duLieu[i].CardID[2],
	                          duLieu[i].CardID[3], duLieu[i].CardID[4], shown, unitName[currentUnit]);
	              } else {
	                  // Khong co vat tren ban can -> hien thi lai du lieu cu da luu cho the nay
	                  ShowStatus("Dang doc the cu", BLUE);

	                  float shown = convertWeight(duLieu[i].canNang_g, currentUnit);

	                  // De gia tri cu len man hinh thay cho can nang thuc te (luc nay = 0)
	                  sprintf(lcdBuf, "%.2f %s (Cu)", shown, unitName[currentUnit]);
	                  LCD_DrawField(VALUE_X, WEIGHT_Y, WEIGHT_FIELD_WIDTH, FONT4, lcdBuf, BLUE);

	                  sprintf(buf, "Du lieu cu: STT: %d , ID: %02X%02X%02X%02X%02X , Can Nang: %.2f %s",
	                          duLieu[i].stt, duLieu[i].CardID[0], duLieu[i].CardID[1], duLieu[i].CardID[2],
	                          duLieu[i].CardID[3], duLieu[i].CardID[4], shown, unitName[currentUnit]);

	                  // LED van luon hien thi theo kg
	                  Set7SegDisplayValue(weightToLed(convertWeight(duLieu[i].canNang_g, UNIT_KG)));
	              }

	              HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 1000);
	              HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, 1000);
	              break;
	          }
	      }

	      if(!tonTai && weightGrams > 0){
	          if (stt < 1000) {
	              duLieu[stt].stt = stt;
	              memcpy(duLieu[stt].CardID, CardID, sizeof(CardID));
	              duLieu[stt].canNang_g = weightGrams;

	              // Hien thi LCD trang thai tao the moi + cap nhat so the da luu
	              ShowStatus("Tao the moi!", MAGENTA);
	              sprintf(lcdBuf, "%d", stt + 1);
	              LCD_DrawField(VALUE_X, COUNT_Y, COUNT_FIELD_WIDTH, FONT3, lcdBuf, DARKGREEN);

	              float shown = convertWeight(duLieu[stt].canNang_g, currentUnit);
	              sprintf(buf, "Ghi du lieu moi: STT: %d , ID: %02X%02X%02X%02X%02X , Can Nang: %.2f %s",
	                      duLieu[stt].stt, duLieu[stt].CardID[0], duLieu[stt].CardID[1], duLieu[stt].CardID[2],
	                      duLieu[stt].CardID[3], duLieu[stt].CardID[4], shown, unitName[currentUnit]);

	              HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 1000);
	              HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, 1000);

	              stt++;
	          } else {
	              ShowStatus("Bo nho da day!", RED);
	              sprintf(buf, "Bo nho day, khong the luu the moi");
	              HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 1000);
	              HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, 1000);
	          }
	      } else if(!tonTai && weightGrams <= 0){
	          ShowStatus("Chua co du lieu", ORANGE);
	          sprintf(buf, "Khong ton tai du lieu cu");
	          HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 1000);
	          HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, 1000);
	      }

	      HAL_Delay(50);
	  }else{
		  // Khong co the: chi chuyen ve "Dang cho the..." SAU KHI het thoi gian giu
		  // thong bao truoc do (xu ly ben trong ShowStatus) -> khong bi mat du lieu
		  // vua hien thi qua nhanh khi rut the ra.
		  ShowStatus("Dang cho the...", DARKGREY);

		  sprintf(buf, "Khong co the !");
		  HAL_UART_Transmit(&huart1, (uint8_t *)buf, strlen(buf), 1000);
		  HAL_UART_Transmit(&huart1, (uint8_t *)"\n", 1, 1000);
		  HAL_Delay(50);
	  }
#endif /* CALIBRATION_MODE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief SPI4 Initialization Function
  * @param None
  * @retval None
  */
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

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
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

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
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

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream4_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pin : LCD_CS_Pin */
  GPIO_InitStruct.Pin = LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RST_Pin LCD_DC_Pin */
  GPIO_InitStruct.Pin = LCD_RST_Pin|LCD_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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

/* USER CODE BEGIN 4 */
//Implement ham Callback cua Timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    Run7SegDisplay(); // Goi ham quet LED trong ngat timer
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
