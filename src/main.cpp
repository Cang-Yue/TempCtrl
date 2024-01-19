/*
 * @Author: Cang-Yue
 * @Date: 2023-12-24 12:29:23
 * @Last Modified by: Cang-Yue
 * @Last Modified time: 2023-12-31 20:18:33
 */

#include "main.h"

void setup() {
  u8g2.begin();
  pinMode(MIN, INPUT_PULLUP);
  pinMode(ADD, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);
  pinMode(SCREEN, INPUT_PULLUP);
  pinMode(EN, OUTPUT);
}

void loop() {
  // Key MODE
  if (digitalRead(MODE) != modeLastButtonState) {
    modeLastPressTime = millis();
  }
  if (digitalRead(MODE) == LOW && millis() - modeLastPressTime > DEBOUNCE_DELAY) {
    keyLastPressTime = millis();
    actionMode();
    modeLastPressTime = millis();
  }
  modeLastButtonState = digitalRead(MODE);

  // Key ADD
  if (digitalRead(ADD) != addLastButtonState) {
    addShortPressTime = millis();
    addLongPressTime = millis();
  }
  if (digitalRead(ADD) == LOW && millis() - addShortPressTime > DEBOUNCE_DELAY && millis() - addShortPressTime < LONGPRESS_DELAY) {
    keyLastPressTime = millis();
    actionAdd();
    addShortPressTime = millis();
  }
  if (digitalRead(ADD) == LOW && millis() - addLongPressTime > LONGPRESS_DELAY) {
    keyLastPressTime = millis();
    actionAddLongPress();
  }
  addLastButtonState = digitalRead(ADD);

  // Key MIN
  if (digitalRead(MIN) != minLastButtonState) {
    minShortPressTime = millis();
    minLongPressTime = millis();
  }
  if (digitalRead(MIN) == LOW && millis() - minShortPressTime > DEBOUNCE_DELAY) {
    keyLastPressTime = millis();
    actionMin();
    minShortPressTime = millis();
  }
  if (digitalRead(MIN) == LOW && millis() - minLongPressTime > LONGPRESS_DELAY) {
    keyLastPressTime = millis();
    actionMinLongPress();
  }
  minLastButtonState = digitalRead(MIN);
  
  // Key SCREEN
  if (digitalRead(SCREEN) != screenLastButtonState) {
    screenLastPressTime = millis();
  }
  if (digitalRead(SCREEN) == LOW && millis() - screenLastPressTime > DEBOUNCE_DELAY) {
    actionScreen();
    screenLastPressTime = millis();
  }
  screenLastButtonState = digitalRead(SCREEN);

  // Close Screen
  if (millis() - keyLastPressTime > screenCloseTimeOut) {
    u8g2.sleepOn();
  }

  // Timer 
  if (millis() - timeLastMinTime > 1000) {
    timeLastMinTime += 1000;
    if (time > 0) {
      time--;
    }
  }

  // Output EN
  if (getTemp() >= range_high) {
    digitalWrite(EN, LOW);
  }
  if (time == 0) {
    digitalWrite(EN, LOW);
  }
  if (getTemp() <= range_low && time > 0) {
    digitalWrite(EN, HIGH);
  }

  refresh_screen(range_low, range_high, time, mode);
}

/**
 * Refreshes the screen with the given range, time, and selected options.
 *
 * @param range_low the lower bound of the range
 * @param range_high the upper bound of the range
 * @param time the time in seconds
 * @param selected the selected option
 *
 * @throws ErrorType description of error
 */
void refresh_screen(int range_low, int range_high, int time, int selected) {
  // clear buffer
  u8g2.clearBuffer();
  // set draw color 2
  u8g2.setDrawColor(2);
  // set temp font
  u8g2.setFont(u8g2_font_freedoomr25_tn);
  // draw temp
  u8g2.setCursor(0, 30);
  u8g2.printf("%03d", getTemp());
  // set range and time font
  u8g2.setFont(u8g2_font_spleen8x16_mn);
  // draw range low
  u8g2.setCursor(58, 14);
  u8g2.printf("%03d-%03d", range_low, range_high);
  // draw time
  int hour = time / 3600;
  int minute = (time - hour * 3600) / 60;
  int second = time - hour * 3600 - minute * 60;
  u8g2.setCursor(58, 28);
  u8g2.printf("%02d:%02d:%02d", hour, minute, second);
  // outline
  // u8g2.drawLine(0, 0, 127, 0);
  // u8g2.drawLine(0, 0, 0, 31);
  // u8g2.drawLine(0, 31, 127, 31);
  // u8g2.drawLine(127, 0, 127, 31);

  switch (selected) {
  case 1:
    // select range low
    u8g2.drawBox(57, 2, 25, 14);
    break;
  case 2:
    // select range high
    u8g2.drawBox(89, 2, 25, 14);
    break;
  case 3:
    // select time hour
    u8g2.drawBox(57, 16, 17, 14);
    break;
  case 4:
    // select time minute
    u8g2.drawBox(81, 16, 17, 14);
    break;
  case 5:
    // select time second
    u8g2.drawBox(105, 16, 17, 14);
    break;
  default:
    break;
  }

  // send buffer
  u8g2.sendBuffer();
}

/**
 * Retrieves the temperature.
 *
 * @return the temperature value.
 */
int getTemp() {
  if (millis() - tempLastGetTime > 250) {
    tempLastGetTime = millis();
    tempLast = thermocouple.readCelsius();
  }
  return tempLast;
}

int secondToHour(int second) {
  return second / 3600;
}

int secondToHourSecond(int second) {
  return secondToHour(second) * 3600;
}

int secondToMinute(int second) {
  return (second - secondToHourSecond(second)) / 60;
}

int secondToMinuteSecond(int second) {
  return secondToMinute(second) * 60;
}

void actionMode() {
  screen_sleep = 0;
  u8g2.sleepOff();
  mode++;
  if (mode == 6) {
    mode = 0;
  }
}

void actionMin() {
  screen_sleep = 0;
  u8g2.sleepOff();
  switch (mode) {
  case 1:
    range_low > 0 ? range_low-- : range_low = 0;
    break;
  case 2:
    range_high > 0 ? range_high-- : range_high = 0;
    break;
  case 3:
    secondToHour(time) > 0 ? time -= 3600 : time += 3600 * 99;
    break;
  case 4:
    secondToMinute(time) > 0 ? time -= 60 : time += 60 * 59;
    break;
  case 5:
    time - secondToHourSecond(time) - secondToMinuteSecond(time) > 0 ? time-- : time = secondToHourSecond(time) + secondToMinuteSecond(time);
    break;
  default:
    break;
  }
}

void actionMinLongPress() {
  screen_sleep = 0;
  u8g2.sleepOff();
  switch (mode) {
  case 1:
    range_low > 0 ? range_low -= 5 : range_low = 0;
    break;
  case 2:
    range_high > 0 ? range_high -= 5 : range_high = 0;
    break;
  case 3:
    secondToHour(time) > 0 ? time -= 3600 * 2 : time += 3600 * 99 * 2;
    break;
  case 4:
    secondToMinute(time) > 0 ? time -= 60 * 2 : time += 60 * 59 * 2;
    break;
  case 5:
    time - secondToHourSecond(time) - secondToMinuteSecond(time) > 0 ? time -= 2 : time = secondToHourSecond(time) + secondToMinuteSecond(time);
    break;
  default:
    break;
  }
}

void actionAdd() {
  screen_sleep = 0;
  u8g2.sleepOff();
  switch (mode) {
  case 1:
    range_low < 999 ? range_low++ : range_low = 999;
    break;
  case 2:
    range_high < 999 ? range_high++ : range_high = 999;
    break;
  case 3:
    secondToHour(time) < 99 ? time += 3600 : time -= secondToHourSecond(time);
    break;
  case 4:
    secondToMinute(time) < 60 ? time += 60 : time -= secondToMinuteSecond(time);
    break;
  case 5:
    time - secondToHourSecond(time) - secondToMinuteSecond(time) < 60 ? time++ : time = 0;
    break;
  default:
    break;
  }
}


void actionAddLongPress() {
  screen_sleep = 0;
  u8g2.sleepOff();
  switch (mode) {
  case 1:
    range_low < 999 ? range_low += 5 : range_low = 999;
    break;
  case 2:
    range_high < 999 ? range_high += 5 : range_high = 999;
    break;
  case 3:
    secondToHour(time) < 99 ? time += 3600 * 2 : time -= secondToHourSecond(time);
    break;
  case 4:
    secondToMinute(time) < 60 ? time += 60 * 2 : time -= secondToMinuteSecond(time);
    break;
  case 5:
    time - secondToHourSecond(time) - secondToMinuteSecond(time) < 60 ? time += 2 : time = 0;
    break;
  default:
    break;
  }
}

void actionScreen() {
  if (screen_sleep == 0) {
    screen_sleep = 1;
    u8g2.sleepOn();
  } else {
    screen_sleep = 0;
    u8g2.sleepOff();
  }
}