#include "nixieclock_config.h"
#include <Arduino.h>
#include <NixieTube.h>
#include <OneWire.h> // http://platformio.org/#!/lib/show/1/OneWire
#include <RTClib.h>  // http://platformio.org/#!/lib/show/83/RTC
#include <Wire.h>

NixieTube nixieDis(PIN_DIN, PIN_ST, PIN_SH, PIN_OE, NIXIE_NUM);
OneWire ds18b20(DS_PIN);
RTC_DS1307 rtc;

DateTime now;

/** 显示缓冲区
 ＊ 0～3: 辉光管显示的数字
 ＊ 4～7: 各个辉光管的颜色, 参考 NixieTube.h 中的 enum Color
 ＊ 8:    亮度
 ＊ 9~12: 冒号, 参考 NixieTube.h 中的 enum Colon
 */
char displayBuffer[13];

int i, j, k, tmp; // 用于 for 循环的技术
bool timeChanged; // 如果时间被重新设置，该值变为
                  // true，用于判断是否进入防阴极中毒模式
unsigned long currentSysTime = 0; // Arduino millis 函数得到的时间
unsigned long tmpSysTime = 0;

unsigned char default_color = Blue;
unsigned char alarm_sound = 0;

// int alarmHour = 0, alarmMinute = 0;

void setup() {
  // tone(PIN_BEEP, 1000);
  /* 启动 RTC */
  rtc.begin();

  /* 启动串口 */
  Serial.begin(115200);

  /* 初始化显示内容 */
  displayBuffer[0] = 0;
  displayBuffer[1] = 0;
  displayBuffer[2] = 0;
  displayBuffer[3] = 0;
  displayBuffer[4] = (char)default_color;
  displayBuffer[5] = (char)default_color;
  displayBuffer[6] = (char)default_color;
  displayBuffer[7] = (char)default_color;
  displayBuffer[8] = 255;
  displayBuffer[9] = (char)None;
  displayBuffer[10] = (char)None;
  displayBuffer[11] = (char)None;
  displayBuffer[12] = (char)None;

  currentSysTime = millis();
  tmpSysTime = millis();

  timeChanged = false;
}

void loop() {
  currentSysTime = millis();
  if (currentSysTime - tmpSysTime > 100) {
    tmpSysTime = millis();
    /* 获取现在时间 */
    now = rtc.now();

    if (Serial.available()) {
      switch (Serial.read()) {
      /* 设置时间 */
      case 'A':
        tmp = now.minute() + 1;
        if (tmp >= 60) {
          tmp = 0;
        }
        if (tmp < 0) {
          tmp = 59;
        }
        rtc.adjust(
            DateTime(now.year(), now.month(), now.day(), now.hour(), tmp, 0));
        now = rtc.now();
        timeChanged = true;
        break;
      case 'B':
        tmp = now.minute() - 1;
        if (tmp >= 60) {
          tmp = 0;
        }
        if (tmp < 0) {
          tmp = 59;
        }
        rtc.adjust(
            DateTime(now.year(), now.month(), now.day(), now.hour(), tmp, 0));
        now = rtc.now();
        timeChanged = true;
        break;
      case 'E':
        tmp = now.hour() + 1;
        if (tmp >= 24) {
          tmp = 0;
        }
        if (tmp < 0) {
          tmp = 59;
        }
        rtc.adjust(
            DateTime(now.year(), now.month(), now.day(), tmp, now.minute(), 0));
        now = rtc.now();
        timeChanged = true;
        break;
      case 'F':
        tmp = now.hour() - 1;
        if (tmp >= 24) {
          tmp = 0;
        }
        if (tmp < 0) {
          tmp = 23;
        }
        rtc.adjust(
            DateTime(now.year(), now.month(), now.day(), tmp, now.minute(), 0));
        now = rtc.now();
        timeChanged = true;
        break;
      //   /* 设置闹钟 */
      //   case 'U':
      //     alarmHour++;
      //     alarmHour = abs(alarmHour % 24);
      //     break;
      //   case 'L':
      //     alarmHour--;
      //     alarmHour = abs(alarmHour % 24);
      //     break;
      //   case 'R':
      //     alarmMinute++;
      //     alarmMinute = abs(alarmMinute % 60);
      //     break;
      case 'D':
        alarm_sound++;
        alarm_sound = alarm_sound % 13;
        alarm(PIN_BEEP, alarm_sound);
        break;
      case 'S': // 设置背景灯颜色
        default_color += 1;
        default_color = default_color % 8;
        displayBuffer[4] = (char)default_color;
        displayBuffer[5] = (char)default_color;
        displayBuffer[6] = (char)default_color;
        displayBuffer[7] = (char)default_color;
      default:
        break;
      }
    }

    /* 将时间放入显示缓冲区 */
    displayBuffer[0] = now.hour() / 10;
    displayBuffer[1] = now.hour() % 10;
    displayBuffer[2] = now.minute() / 10;
    displayBuffer[3] = now.minute() % 10;
    if (now.second() % 2 == 0) {
      displayBuffer[9] = (char)None;
      displayBuffer[10] = (char)None;
      displayBuffer[11] = (char)None;
      displayBuffer[12] = (char)None;
    } else {
      displayBuffer[9] = (char)None;
      displayBuffer[10] = (char)Both;
      displayBuffer[11] = (char)None;
      displayBuffer[12] = (char)None;
    }

    /* 防止阴极中毒 */
    if ((now.minute() % 10 == 0) && (now.second() <= 45)) {
      if (timeChanged == false) {
        i = 30;
        while (i > 0) {
          i--;
          nixieDis.setBackgroundColor(0, (Color)(i % 7));
          nixieDis.setBackgroundColor(1, (Color)((i + 1) % 7));
          nixieDis.setBackgroundColor(2, (Color)((i + 2) % 7));
          nixieDis.setBackgroundColor(3, (Color)((i + 3) % 7));
          for (j = 0; j < 10; j++) {
            nixieDis.printf("%d:%d:%d:%d", j, j, j, j);
            nixieDis.display();
            delay(60);
          }
        }
      }
    } else {
      timeChanged = false;
    }

    /* 显示缓冲区内容 */
    nixieDis.setNumber(0, displayBuffer[0]);
    nixieDis.setNumber(1, displayBuffer[1]);
    nixieDis.setNumber(2, displayBuffer[2]);
    nixieDis.setNumber(3, displayBuffer[3]);
    nixieDis.setBackgroundColor(0, (Color)displayBuffer[4]);
    nixieDis.setBackgroundColor(1, (Color)displayBuffer[5]);
    nixieDis.setBackgroundColor(2, (Color)displayBuffer[6]);
    nixieDis.setBackgroundColor(3, (Color)displayBuffer[7]);
    nixieDis.setBrightness(displayBuffer[8]);
    nixieDis.setColon(0, (Colon)displayBuffer[9]);
    nixieDis.setColon(1, (Colon)displayBuffer[10]);
    nixieDis.setColon(2, (Colon)displayBuffer[11]);
    nixieDis.setColon(3, (Colon)displayBuffer[12]);
    nixieDis.display();
    //
    // /* 通过蓝牙发送闹钟时间 */
    // Serial.print("[Alarm]");
    // Serial.print(alarmHour);
    // Serial.print(":");
    // Serial.print(alarmMinute);
  }
}

void alarm(uint8_t mPin, uint8_t var) {
  switch (var) {
  case 0:
    tone(mPin, 500, 300);
    delay(400);
    tone(mPin, 500, 300);
    delay(400);
  case 1: // sa jiao
    for (int i = 200; i < 990; i++) {
      tone(mPin, i, 10);
    }
    for (int i = 990; i > 200; i--) {
      tone(mPin, i, 10);
    }
    for (int i = 200; i < 350; i++) {
      tone(mPin, i, 10);
    }
    delay(500);
    break;
  case 2: // di di
    tone(mPin, 900, 200);
    delay(200);
    tone(mPin, 900, 200);
    delay(200);
    break;
  case 3: // de li
    tone(mPin, 300, 400);
    delay(50);
    tone(mPin, 400, 250);
    delay(50);
    break;
  case 4: // du da
    tone(mPin, 300, 500);
    delay(100);
    for (int i = 300; i < 500; i++) {
      tone(mPin, i, 5);
    }
    delay(50);
    break;
  case 5: // du~ do~~
    for (int i = 450; i < 650; i++) {
      tone(mPin, i);
      delay(1);
    }
    noTone(mPin);
    delay(500);
    //
    for (int i = 450; i < 750; i++) {
      tone(mPin, i);
      delay(1);
    }
    noTone(mPin);
    delay(500);
    break;
  case 6: // du_-`~
    for (int i = 1000; i < 1400; i++) {
      tone(mPin, i);
      delay(1);
    }
    noTone(mPin);
    delay(100);
    break;
  case 7: // di- dang-_
    tone(mPin, 1200);
    delay(100);
    noTone(mPin);
    delay(100);
    for (int i = 900; i > 300; i--) {
      tone(mPin, i);
      delay(1);
    }
    noTone(mPin);
    delay(100);
    break;
  case 8: // dang~ dang-
    tone(mPin, 1100);
    delay(120);
    noTone(mPin);
    delay(100);
    tone(mPin, 600);
    delay(350);
    noTone(mPin);
    delay(100);
    break;
  case 9: // u- u~
    tone(mPin, 150);
    delay(400);
    noTone(mPin);
    delay(250);
    tone(mPin, 200);
    delay(400);
    noTone(mPin);
    delay(100);
    break;
  case 12: // bee boo
    for (int i = 300; i < 400; i++) {
      tone(mPin, i);
      delay(1);
    }
    tone(mPin, 400);
    delay(200);
    for (int i = 400; i > 350; i--) {
      tone(mPin, i);
      delay(1);
    }
    tone(mPin, 350);
    delay(200);
    noTone(mPin);
    break;
  default: // bee bee
    tone(mPin, 500, 300);
    delay(400);
    tone(mPin, 500, 300);
    delay(400);
  }
}
