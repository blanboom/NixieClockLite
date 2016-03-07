#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>      // http://platformio.org/#!/lib/show/83/RTC
#include <OneWire.h>     // http://platformio.org/#!/lib/show/1/OneWire
#include <NixieTube.h>
#include "nixieclock_config.h"


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

int i, j, k;

void setup() {
  /* 启动 RTC */
  rtc.begin();

  /* 启动串口 */
  Serial.begin(115200);

  /* 初始化显示内容 */
  displayBuffer[0] = 0;
  displayBuffer[1] = 0;
  displayBuffer[2] = 0;
  displayBuffer[3] = 0;
  displayBuffer[4] = (char)Blue;
  displayBuffer[5] = (char)Blue;
  displayBuffer[6] = (char)Blue;
  displayBuffer[7] = (char)Blue;
  displayBuffer[8] = 255;
  displayBuffer[9] = (char)None;
  displayBuffer[10] = (char)None;
  displayBuffer[11] = (char)None;
  displayBuffer[12] = (char)None;
}

void loop() {
  /* 获取现在时间 */
  now = rtc.now();

  /* 将时间放入显示缓冲区 */
  displayBuffer[0] = now.hour() / 10;
  displayBuffer[1] = now.hour() % 10;
  displayBuffer[2] = now.minute() / 10;
  displayBuffer[3] = now.minute() % 10;
  if(now.second() % 2 == 0) {
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
  if((now.minute() % 10 == 0) && (now.second() <= 35)) {
    i = 15;
    while(i > 0) {
      i--;
      nixieDis.setBackgroundColor(0, (Color)(i % 7));
      nixieDis.setBackgroundColor(1, (Color)((i + 1) % 7));
      nixieDis.setBackgroundColor(2, (Color)((i + 2) % 7));
      nixieDis.setBackgroundColor(2, (Color)((i + 3) % 7));
      for(j = 0; j < 10; j++){
        nixieDis.printf("%d:%d:%d:%d", j, (j + 1) % 10, (j + 2) % 10, (j + 3) % 10);
        nixieDis.display();
        delay(10);
      }
    }
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

  delay(100);
}
