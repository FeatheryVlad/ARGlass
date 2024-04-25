#ifndef _PROCESSOR_DEFS_H
#define _PROCESSOR_DEFS_H

#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

//Camera pins
#define PWDN_GPIO_PIN  32
#define RESET_GPIO_PIN -1
#define XCLK_GPIO_PIN  0
#define SIOD_GPIO_PIN  26
#define SIOC_GPIO_PIN  27
#define Y7_GPIO_PIN    35
#define Y6_GPIO_PIN    34
#define Y5_GPIO_PIN    39
#define Y4_GPIO_PIN    36
#define Y3_GPIO_PIN    21
#define Y2_GPIO_PIN    19
#define Y1_GPIO_PIN    18
#define Y0_GPIO_PIN    5
#define VSYNC_GPIO_PIN 25
#define HREF_GPIO_PIN  23
#define PCLK_GPIO_PIN  22

//LCD
#define LCD_WIDTH  80
#define LCD_HEIGHT 160
#define LCD_OFFSET_X 26
#define LCD_OFFSET_Y 1
//#define LCD_MISO 4  //not use
#define LCD_MOSI 13 //sda/sdin
#define LCD_SCLK 14 //sck
#define LCD_CS   15 //
#define LCD_DC   2  //ao/ds/rs
#define LCD_RST  12 //

#define B0_GPIO_PIN 33
#define B1_GPIO_PIN 4

#endif /* _PROCESSOR_DEFS_H */