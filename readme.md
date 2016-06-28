# 四位辉光管时钟

[![Build Status](https://travis-ci.org/blanboom/NixieClockLite.svg?branch=master)](https://travis-ci.org/blanboom/NixieClockLite)

## 硬件

- [Q30-1 辉光管模块](https://item.taobao.com/item.htm?id=15397910473)
- [Bluno Beetle](http://www.dfrobot.com.cn/goods.php?id=1097)
- [DS1307 时钟模块（带有 DS18B20 温度传感器）](http://www.dfrobot.com.cn/goods.php?id=535)
- 蜂鸣器
- 滚珠开关

## 软件

- 开发平台： [PlatformIO](http://platformio.org)
- RTC 读取
- 时间显示
- 防止阴极中毒
- 闹钟
- 蓝牙控制

## 蓝牙通信方式

控制信息均通过单个大写字母的形式进行发送，每个字母代表一个功能。

- 设置时间
    - **A**: 分钟加一
    - **B**: 分钟减一
    - **E**: 小时加一
    - **F**: 小时减一
- 设置闹钟
    - **U**: 闹钟小时加一
    - **L**: 闹钟分钟加一（当闹钟时、分均为 0 时，闹钟关闭）
    - **D**: 更改闹钟声音
    - **R**: 停止播放闹钟声（也可通过摇晃时钟若干次来停止闹钟播放）
- **S**: 设置背景灯颜色

闹钟时间也可通过蓝牙的方式进行读取，通过读取 UUID 为 DFB1 的字符串即可得到现在的闹钟时间。具体方法请参考 Bluno 官方文档。

可使用 HackerRemote 这一 App 方便地对时钟进行设置，下载地址： [https://itunes.apple.com/cn/app/id1120243546](https://itunes.apple.com/cn/app/id1120243546)

## 详细信息

[http://blanboom.org/nixie-clock-lite.html](http://blanboom.org/nixie-clock-lite.html)

## 感谢

本项目使用 aguegu 的 [nixie-tube](https://github.com/aguegu/nixie-tube) 库进行辉光管的相关操作
