#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nav_menu.h"
#include "proc_defs.h"

nav_menu_t local_menu;

void init_menu(int* start_page, ST7735_t* dev, FontxFile* font){
  local_menu.start_page = 0;
  local_menu.dev = dev;
  local_menu.font = font;
  local_menu.fontWidth = getFortWidth(font);
  local_menu.fontHeight = getFortHeight(font);
}

void create_page_header(page_t * page, uint8_t * ascii, uint16_t stack_id){
  lcdDrawFillRect(local_menu.dev, LCD_WIDTH-11*(stack_id+1), 0, LCD_WIDTH-11*(stack_id), LCD_HEIGHT, GRAY);
  lcdDrawString(local_menu.dev, local_menu.font, LCD_WIDTH-3-local_menu.fontHeight-11*(stack_id), LCD_HEIGHT/2-(local_menu.fontWidth*strlen((char *)ascii)/2), ascii, WHITE);
}

void create_page_button(page_t * page, uint8_t * ascii, uint16_t stack_id){
  lcdDrawFillRect(local_menu.dev, LCD_WIDTH-11*(stack_id+1)-stack_id, 0, LCD_WIDTH-11*(stack_id)-stack_id, LCD_HEIGHT, BLACK);
  lcdDrawString(local_menu.dev, local_menu.font, LCD_WIDTH-3-local_menu.fontHeight-11*(stack_id)-stack_id, 2, ascii, WHITE);
}

void update_page_button(page_t * page, uint8_t * ascii, uint16_t stack_id, uint16_t back_color, uint16_t front_color){
  lcdDrawFillRect(local_menu.dev, LCD_WIDTH-11*(stack_id+1)-stack_id, 0, LCD_WIDTH-11*(stack_id)-stack_id, LCD_HEIGHT, back_color);
  lcdDrawString(local_menu.dev, local_menu.font, LCD_WIDTH-3-local_menu.fontHeight-11*(stack_id)-stack_id, 2, ascii, front_color);
}

void create_back_button(page_t * page, uint8_t * ascii){
  lcdDrawFillRect(local_menu.dev, 0, 0, 11, LCD_HEIGHT, BLACK);
  lcdDrawString(local_menu.dev, local_menu.font, -3, 2, ascii, WHITE);
}

void update_back_button(page_t * page, uint8_t * ascii, uint16_t back_color, uint16_t front_color){
  lcdDrawFillRect(local_menu.dev, 0, 0, 11, LCD_HEIGHT, back_color);
  lcdDrawString(local_menu.dev, local_menu.font, -3, 2, ascii, front_color);
}

//void create_page(page_t * page){

//}