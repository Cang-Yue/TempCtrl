#include <Arduino.h>
#include <U8g2lib.h>
#include <max6675.h>

// I2C pins
#define I2C1_SDA PB7
#define I2C1_SCL PB6

// Output pins
#define LED0 PC13
#define EN PA4

// Input pins
#define MIN PA0
#define ADD PA1
#define MODE PA2
#define SCREEN PA3

// max6675 Pin
#define MAX6675_DO PA5
#define MAX6675_CS PA6
#define MAX6675_CLK PA7

// Delay
#define DEBOUNCE_DELAY 80
#define LONGPRESS_DELAY 1000

// Temp fix
#define TEMP_HIGH 0
#define TEMP_LOW 0

int mode = 0;
int range_low = 200;
int range_high = 400;
int time = 0;
int screen_sleep = 0;

unsigned long tempLastGetTime;
int tempLast;

unsigned long timeLastMinTime;

unsigned long keyLastPressTime;
unsigned long screenCloseTimeOut = 1000 * 30;

unsigned long modeLastPressTime;
int modeLastButtonState = HIGH;
unsigned long addShortPressTime;
unsigned long addLongPressTime;
int addLastButtonState = HIGH;
unsigned long minShortPressTime;
unsigned long minLongPressTime;
int minLastButtonState = HIGH;
unsigned long screenLastPressTime;
int screenLastButtonState = HIGH;

// OLED setup
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE, I2C1_SCL, I2C1_SDA);

// MAX6675 setup
MAX6675 thermocouple(MAX6675_CLK, MAX6675_CS, MAX6675_DO);

void refresh_screen (int range_low, int range_high, int time, int selected);
int getTemp ();
int secondToHour(int second);
int secondToHourSecond(int second);
int secondToMinute(int second);
int secondToMinuteSecond(int second);
void actionAdd();
void actionAddLongPress();
void actionMin();
void actionMinLongPress();
void actionMode();
void actionScreen();