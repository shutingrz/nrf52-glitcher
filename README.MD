# Glitcher

Voltage Fault Injectionを行うESP32のプログラムです。

**※自分自身が所有する機器にのみ使用してください。**



# Usage

ソースコードをコンパイルし、ESP32に書き込みます。

paranoia_modeを利用すると、ディレイ間隔が1usからおよそ25nsに切り替わります。ビジーループで作り出しているため精度に揺らぎがあるかもしれません。解像度が40倍になるため、ディレイに関する各パラメータを調整してください。delay_startの値を40倍にすれば、paranoia_modeが0のときと同じディレイを発生させられるはずです。

# Example

```
  ____  _  _  _         _
 / ___|| |(_)| |_  ___ | |__    ___  _ __
| |  _ | || || __|/ __|| '_ \  / _ \| '__|
| |_| || || || |_| (__ | | | ||  __/| |
 \____||_||_| \__|\___||_| |_| \___||_|    by @shutingrz

Serial terminal usage:
  help               Print this usage
  nrf                Voltage glitch for nrf

> nrf
switch nrf mode.

Serial terminal usage:
  help               Print this usage
  set <item> <value> set param
  exit               exit nrf mode
  show               show glitch options
  power <on|off>     Power on or off nRF.
  swd                SWD IDCODE check

You can set glitch option value with "set" command.
 Params: delay_start, delay_end, width_start, width_end, paranoia_mode
and can get current value with "show" command.

nrf> swd
SWD IDCODE: 0x00000064
UICR device type(0x10000100): 0x00000000
APPROTECT enabled.
SWD port locked.
nrf>
(..snip..)
nrf> set delay_start 2360
set: delay_start=2360
nrf> run
----------------
Glitch options:
  delay_start   : 2360
  width_start   : 0
  delay_end     : 2500
  width_end     : 10
  paranoia_mode : 0
----------------
Start glitch! Press any key to stop.
...........2361...........2362...........2363...........2364...........2365...........2366...........2367...........2368...........2369...........2370...........2371...........2372...........2373...........2374...........2375...........2376...........2377...........2378...........2379...........2380.......glitch success!
SWD Id: 0x2ba01477
UICR_LOCK: 0xffffff00
Flash size: 1048576

Connect your debugger to the SWD port and attach it!
nrf> swd

SWD IDCODE: 0x2ba01477
UICR device type(0x10000100): 0x00052840
APPROTECT disabled.
SWD port unlocked!
nrf>
```

# Reference

- [nRF52 Debug Resurrection (APPROTECT Bypass) Part 1](https://limitedresults.com/2020/06/nrf52-debug-resurrection-approtect-bypass/)

This tool was inspired by [atc1441/ESP32_nRF52_SWD](https://github.com/atc1441/ESP32_nRF52_SWD)
