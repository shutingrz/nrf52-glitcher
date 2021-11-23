/*
   Copyright (c) 2021 Aaron Christophel ATCnetz.de
   SPDX-License-Identifier: GPL-3.0-or-later
*/
#include <Arduino.h>
#include "defines.h"
#include "swd.h"
#include "nrf_swd.h"
#include "glitcher.h"
#define GPIO_OUT_W1TS_REG_REF *(volatile uint32_t *)GPIO_OUT_W1TS_REG
#define GPIO_OUT_W1TC_REG_REF *(volatile uint32_t *)GPIO_OUT_W1TC_REG
#define delay_inc_step 1
#define width_inc_step 1

uint32_t delay_max = 2500;
uint32_t width_max = 10;

uint32_t delay_start = 2300;
uint32_t width_start = 0;

uint32_t _delay_us = 0;
uint32_t _delay_us_end = 0;
uint32_t _width = 0;

uint32_t _power_off_delay = 20;
uint32_t _swd_wait_delay = 40;

uint32_t paranoia_mode = 0;
bool glitcher_enabled = false;

void glitcher_prepare()
{
  pinMode(LED, OUTPUT);
  pinMode(GLITCHER, OUTPUT);
  pinMode(NRF_POWER, OUTPUT);
  digitalWrite(GLITCHER, LOW);
}

void glitcher_begin()
{
  _delay_us = delay_start;
  _delay_us_end = delay_max;
  _width = width_start;
  swd_begin();
}
void glitcher_stop()
{
  swd_stop();
  digitalWrite(GLITCHER, LOW);
}

inline void fast_glitch(int width) // width min 35ns, count+1 = +25ns
{
  GPIO_OUT_W1TS_REG_REF = BIT(GLITCHER);
  for(int i=0; i<width;i++){
    asm volatile("");
  }
  GPIO_OUT_W1TC_REG_REF = BIT(GLITCHER);
}

inline void slow_glitch(int width)
{
  GPIO_OUT_W1TS_REG_REF = BIT(GLITCHER);
  delayMicroseconds(width);
  GPIO_OUT_W1TC_REG_REF = BIT(GLITCHER);
}

inline void delay_25ns(int count)
{
  for(int i=0; i<count;i++){
    asm volatile("");
  }
}

void set_glitcher(bool new_state)
{
  glitcher_enabled = new_state;
}

bool get_glitcher()
{
  return glitcher_enabled;
}

void set_power(bool state)
{
  digitalWrite(LED, state);
  digitalWrite(NRF_POWER, state);
}

void do_glitcher()
{
  //Serial.println("Delay: " + String(get_delay()) + " Width: " + String(get_width()));
  Serial.print(".");
  digitalWrite(swd_clock_pin, LOW);
  set_power(LOW);
  delay(_power_off_delay);
  int width = get_width();
  int delay_count = get_delay();
  if(paranoia_mode){
    set_power(HIGH);
    delay_25ns(delay_count);
    fast_glitch(width);
  }else{
    set_power(HIGH);
    delayMicroseconds(delay_count);
    slow_glitch(width);
  }

  if (inc_width())
  {
    inc_delay();
    Serial.print(get_delay());
  }

  delay(_swd_wait_delay);
  //Serial.printf("SWD Id: 0x%08x\r\n", nrf_begin(true));
  nrf_begin(true);
  uint32_t variant_read = read_register(0x10000100, 1);
  if (variant_read == 0x00052832 || variant_read == 0x00052840 || nrf_read_lock_state() == 1)
  {
    Serial.println(F("glitch success!"));
    glitcher_enabled = false;
    Serial.printf("SWD Id: 0x%08x\r\n", nrf_begin());
    Serial.printf("UCIR_LOCK: 0x%08x\n", nrf_ufcr.ucir_lock);
    Serial.printf("Flash size: %i\r\n", nrf_ufcr.flash_size);
    Serial.println(F(""));
    Serial.println(F("Connect your debugger to the SWD port and attach it!"));
    glitcher_stop();
  }
}

void set_delay(uint32_t delay_us, uint32_t delay_us_end, uint32_t power_off_delay, uint32_t swd_wait_delay)
{
  _delay_us = delay_us;
  delay_start = _delay_us;

  _delay_us_end = delay_us_end;
  delay_max = _delay_us_end;

  _power_off_delay = power_off_delay;
  _swd_wait_delay = swd_wait_delay;
}

uint32_t get_delay()
{
  return _delay_us;
}

bool inc_delay()
{
  _delay_us += delay_inc_step;
  if (_delay_us > _delay_us_end)
  {
    _delay_us = delay_start;
    return true;
  }
  return false;
}

void set_width(uint32_t width)
{
  _width = width;
}

uint32_t get_width()
{
  return _width;
}

bool inc_width()
{
  _width += width_inc_step;
  if (_width > width_max)
  {
    _width = width_start;
    return true;
  }
  return false;
}
