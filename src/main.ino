#include <Arduino.h>
#include <Wire.h>
#include <RTClib.h>      //http://platformio.org/#!/lib/show/83/RTC
#include <OneWire.h>     //http://platformio.org/#!/lib/show/1/OneWire
#include <NixieTube.h>
#include "nixieclock_config.h"


NixieTube nixieDis(PIN_DIN, PIN_ST, PIN_SH, PIN_OE, NIXIE_NUM);
OneWire ds18b20(DS_PIN);
RTC_DS1307 rtc;

DateTime now;

void setup() {
  /* 启动 RTC */
  rtc.begin();

  /* 启动串口 */
  Serial.begin(115200);

  /* 初始化辉光管 */
  nixieDis.setBrightness(255);
  nixieDis.setBackgroundColor(Red);
  nixieDis.printf("1234");
  nixieDis.display();
}

void loop() {
  /* 获取现在时间 */
  now = rtc.now();

  /* 显示现在时间 */
  nixieDis.printf("%2d%2d", now.hour(), now.minute());
  nixieDis.display();

  delay(100);
}
