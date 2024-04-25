#ifndef _NAVIGATION_MENU_H
#define _NAVIGATION_MENU_H

#include "button.h"
#include "fontx.h"
#include "st7735s.h"

typedef struct {
  int id;
  int prev_id;
  int stack_items;
  uint8_t selector;
  uint8_t prev_selector;
  int max_select;
  int item_count;
  bool is_inited;
  bool is_new;
  bool btn0_is_up;
  bool btn0_is_down;
  bool btn0_is_held;
  bool btn1_is_up;
  bool btn1_is_down;
  bool btn1_is_held;
  uint16_t items[];
} page_t;

typedef struct {
  int start_page;
  int current_page;
  ST7735_t* dev;
  FontxFile* font;
  uint8_t fontWidth;
  uint8_t fontHeight;
  page_t page_array[];
} nav_menu_t;

void init_menu(int* start_page, ST7735_t* dev, FontxFile* fx16);
void create_page_header(page_t * page, uint8_t * ascii, uint16_t stack_id);
void create_page_button(page_t * page, uint8_t * ascii, uint16_t stack_id);
void update_page_button(page_t * page, uint8_t * ascii, uint16_t stack_id, uint16_t back_color, uint16_t front_color);
void create_back_button(page_t * page, uint8_t * ascii);
void update_back_button(page_t * page, uint8_t * ascii, uint16_t back_color, uint16_t front_color);
//void create_page(page_t * page);
//void create_page_content(int page_id);

#endif /* _NAVIGATION_MENU_H */