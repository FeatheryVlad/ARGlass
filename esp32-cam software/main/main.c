/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

  idf.py --no-ccache build

*/
#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_system.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "nvs_flash.h"
#include "http_client.h"
#include "button.h"
#include <math.h>

#include "proc_defs.h"
#include "nav_menu.h"
#include "st7735s.h"
#include "camera.h"
#include "fontx.h"
#include "wifi.h"


static const char *TAG = "_ ARG OS";
static const char *test_tr = "task";
ST7735_t dev;
FontxFile fx16[2];
uint8_t fontBuffer[FontxGlyphBufSize];
uint8_t fontWidth;
uint8_t fontHeight;
uint8_t ascii_text[128];
uint16_t progress_bar[4] = {LCD_WIDTH/4, LCD_HEIGHT/5, LCD_WIDTH/4+5, LCD_HEIGHT-LCD_HEIGHT/5};
button_event_t bEvents;
QueueHandle_t button_qHandler;
//nav_menu_t menu;
//int progress_bar_tasks = 4;
//uint16_t progress_bar_task = 1;

#define IRED rgb565(255, 15, 67)
#define IGREEN rgb565(153, 230, 95)
#define IGREEN2 rgb565(90, 197, 79)
#define CYELLOW rgb565(220, 241, 33)
#define DARKBLUE rgb565(20, 52, 100)

void init_text(char* text) {
  lcdDrawFillRect(&dev, 0, 0, fontHeight, LCD_HEIGHT, BLACK);
  strcpy((char *)ascii_text, text);
  lcdDrawString(&dev, fx16, -4, 1, ascii_text, IRED);
}

//void add_progress_bar(uint16_t task){
//  lcdDrawFillRect(&dev, progress_bar[0]+1, progress_bar[1]+1, progress_bar[2]-1, task-1, IGREEN2);
//  //progress_bar_task += 1;
//}

//#define UART_NUM_0 UART_NUM_0/progress_bar[5]*progress_bar[4]/progress_bar_tasks*progress_bar_task
//#define EX_TASK_STACK_SIZE 2048
//#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define BUF_SIZE (1024)
//#define RD_BUF_SIZE (BUF_SIZE)
static QueueHandle_t uart0_queue;

static void echo_task(void *arg) {
  ESP_LOGI(TAG, "UART echo init!");
  uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

  while (1) {
    // Read data from the UART
    int len = uart_read_bytes(UART_NUM_0, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
    // Write data back to the UART
    uart_write_bytes(UART_NUM_0, (const char *)data, len);
    //ESP_LOGI(TAG, "Recv str: init1");
    test_tr = "task1";
    if (len)
    {
      camera_capture();
      http_rest_with_url();
      data[len] = '\0';
      ESP_LOGI(TAG, "Recv str: %s", (char *)data);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

esp_err_t uart_init(void) {
  uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };

  esp_err_t ret = uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2, 20, &uart0_queue, 0);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed uart_driver_install");
    return ESP_FAIL;
  }
  ret = uart_param_config(UART_NUM_0, &uart_config);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed uart_param_config");
    return ESP_FAIL;
  }
  ret = uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed uart_set_pin");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "UART inited!");
  return ESP_OK;
}

esp_err_t nvs_init(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  if(ret == ESP_OK){
    return ESP_OK;
  }else return ESP_FAIL;
}

esp_err_t spiffs_init(void) {
  
  ESP_LOGI(TAG, "Initializing SPIFFS");

  esp_vfs_spiffs_conf_t conf = {
    .base_path = "/spiffs",
    .partition_label = NULL,
    .max_files = 10,
    .format_if_mount_failed =true
  };

  // Use settings defined above toinitialize and mount SPIFFS filesystem.
  // Note: esp_vfs_spiffs_register is anall-in-one convenience function.
  esp_err_t ret =esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK) {
    if (ret ==ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount or format filesystem");
    } else if (ret== ESP_ERR_NOT_FOUND) {
      ESP_LOGE(TAG, "Failed to find SPIFFS partition");
    } else {
      ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)",esp_err_to_name(ret));
    }
    return ESP_FAIL;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total,&used);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG,"Failed to get SPIFFS partition information (%s)",esp_err_to_name(ret));
    return ESP_FAIL;
  } else {
    ESP_LOGI(TAG,"Partition size: total: %d, used: %d", total, used);
  }
  return ESP_OK;
}

//void configure_menu(){
//  page_t page = {
//    .id = 0,
//    .stack_items = 0,
//    .item_count = 3
//  };
//  strcpy((char *)ascii_text, "ARG OS Menu");
//  create_page_header(&page, ascii_text);
//  strcpy((char *)ascii_text, "Wifi settings ->");
//  create_page_button(&page, ascii_text, 1, 1);
//  strcpy((char *)ascii_text, "Server settings ->");
//  create_page_button(&page, ascii_text, 2, 2);
//  create_page(&page);
//  //page_t page = {
//  //  .id = 0
//  //};
//}

static void main_handler_task(void *arg) {
  xTaskCreate(echo_task, "uart_echo_task", 4096*2, NULL, 10, NULL);
  lcdFillScreen(&dev, BLACK);
  //int event_id = 0;
  //int back_page_id = 0;
  button_qHandler = gpio_buttons_init(PIN_BIT(B0_GPIO_PIN) | PIN_BIT(B1_GPIO_PIN), GPIO_PULLUP_ONLY);
  init_menu(0, &dev, fx16);
  //ESP_ERROR_CHECK(gpio_reset_pin(B1_GPIO_PIN));
  //ESP_ERROR_CHECK(gpio_set_direction(B1_GPIO_PIN, GPIO_MODE_OUTPUT));
  //ESP_ERROR_CHECK(gpio_set_level(B1_GPIO_PIN, 0));
  //gpio_config_t io_conf;
  //io_conf.intr_type = GPIO_INTR_POSEDGE;
  //io_conf.mode = GPIO_MODE_INPUT;
  //io_conf.pull_up_en = (pull_mode == GPIO_PULLUP_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN);
  //io_conf.pull_down_en = (pull_mode == GPIO_PULLDOWN_ONLY || pull_mode == GPIO_PULLUP_PULLDOWN);;
  //io_conf.pin_bit_mask = pin_select;
  //ESP_ERROR_CHECK(gpio_reset_pin(B0_GPIO_PIN));
  //ESP_ERROR_CHECK(gpio_set_direction(B0_GPIO_PIN, GPIO_MODE_INPUT));
  //ESP_ERROR_CHECK(gpio_reset_pin(B1_GPIO_PIN));
  //ESP_ERROR_CHECK(gpio_set_direction(B1_GPIO_PIN, GPIO_MODE_INPUT));
  //ESP_ERROR_CHECK(gpio_set_pull_mode(B0_GPIO_PIN, GPIO_FLOATING));
  //ESP_ERROR_CHECK(gpio_set_pull_mode(B1_GPIO_PIN, GPIO_FLOATING));
  //gpio_config(&io_conf);note: 'page' was declared hereerror: 'page' is used uninitialized
  page_t page;
  page.id = 0;
  page.prev_id = 0;
  page.btn0_is_up = false;
  page.btn0_is_down = false;
  page.btn0_is_held = false;
  page.btn1_is_up = false;
  page.btn1_is_down = false;
  page.btn1_is_held = false;
  page.is_inited = false;
  page.is_new = true;
  page.selector = 0;
  page.prev_selector = -1;
  page.max_select = 0;
  //page.stack_items = 0;
  while(1){
    //event handler
    if (xQueueReceive(button_qHandler, &bEvents, 1000/portTICK_PERIOD_MS)) {
      if ((bEvents.pin == B0_GPIO_PIN) && (bEvents.event == BUTTON_UP)) {
        page.btn0_is_up = true;
        if(page.selector-1 >= 0) page.selector -= 1;
        else page.selector = page.max_select-1;
        ESP_LOGI(TAG, "button 0 up");
      }else{ page.btn0_is_up = false; }
      if ((bEvents.pin == B1_GPIO_PIN) && (bEvents.event == BUTTON_UP)) {
        page.btn1_is_up = true;
        if(page.selector+1 < page.max_select) page.selector += 1;
        else page.selector = 0;
        ESP_LOGI(TAG, "button 1 up");
      }else{ page.btn1_is_up = false; }
      if ((bEvents.pin == B0_GPIO_PIN) && (bEvents.event == BUTTON_DOWN)) {
        page.btn0_is_down = true;
        ESP_LOGI(TAG, "button 0 down");
      }else{ page.btn0_is_down = false; }
      if ((bEvents.pin == B1_GPIO_PIN) && (bEvents.event == BUTTON_DOWN)) {
        page.btn1_is_down = true;
        ESP_LOGI(TAG, "button 1 down");
      }else{ page.btn1_is_down = false; }
      if ((bEvents.pin == B0_GPIO_PIN) && (bEvents.event == BUTTON_HELD)) {
        page.btn0_is_held = true;
        //if(page.selector-1 >= 0) page.selector -= 1;
        //else page.selector = page.max_select;
        ESP_LOGI(TAG, "button 0 holded");
      }else{ page.btn0_is_held = false; }
      if ((bEvents.pin == B1_GPIO_PIN) && (bEvents.event == BUTTON_HELD)) {
        page.btn1_is_held = true;
        //if(page.selector+1 <= page.max_select) page.selector += 1;
        ESP_LOGI(TAG, "button 0 holded");
      }else{ page.btn1_is_held = false; }
    }
    //pages
    if(page.id == 0){
      if(!page.is_inited){
        lcdFillScreen(&dev, BLACK);
        strcpy((char *)ascii_text, "ARG OS Menu");
        create_page_header(&page, ascii_text, 0);
        strcpy((char *)ascii_text, "Applications");
        create_page_button(&page, ascii_text, 1);
        strcpy((char *)ascii_text, "Wifi settings");
        create_page_button(&page, ascii_text, 2);
        strcpy((char *)ascii_text, "Server settings");
        create_page_button(&page, ascii_text, 3);
        page.prev_selector = -1;
        page.is_inited = true;
        page.max_select = 3;
        page.is_new = true;
        page.selector = 0;
      }
      if(!page.is_new || !page.btn0_is_held){
        page.is_new = false;
        if(page.prev_selector != page.selector) {
          if(page.selector == 0){
            strcpy((char *)ascii_text, "Applications");
            update_page_button(&page, ascii_text, 1, WHITE, BLACK);
            if(page.btn0_is_held){
              page.prev_id = page.id;
              page.is_inited = false;
              page.id = 1;
            }
          }else{
            strcpy((char *)ascii_text, "Applications >");
            update_page_button(&page, ascii_text, 1, BLACK, WHITE);
          }
          if(page.selector == 1){
            strcpy((char *)ascii_text, "Wifi settings");
            update_page_button(&page, ascii_text, 2, WHITE, BLACK);
            if(page.btn0_is_held){
              page.prev_id = page.id;
              page.is_inited = false;
              page.id = 2;
            }
          }else{
            strcpy((char *)ascii_text, "Wifi settings >");
            update_page_button(&page, ascii_text, 2, BLACK, WHITE);
          }
          if(page.selector == 2){
            strcpy((char *)ascii_text, "Server settings");
            update_page_button(&page, ascii_text, 3, WHITE, BLACK);
            if(page.btn0_is_held){
              page.prev_id = page.id;
              page.is_inited = false;
              page.id = 3;
            }
          }else{
            strcpy((char *)ascii_text, "Server settings >");
            update_page_button(&page, ascii_text, 3, BLACK, WHITE);
          }
        }
      }
    }else if(page.id == 1){
      if(!page.is_inited){
        lcdFillScreen(&dev, BLACK);
        strcpy((char *)ascii_text, "Applications");
        create_page_header(&page, ascii_text, 0);
        strcpy((char *)ascii_text, "Object detecting");
        create_page_button(&page, ascii_text, 1);
        strcpy((char *)ascii_text, "Back");
        create_back_button(&page, ascii_text);
        //strcpy((char *)ascii_text, "Server settings");
        //create_page_button(&page, ascii_text, 3);
        //page.prev_id = page.id;
        page.prev_selector = -1;
        page.is_inited = true;
        page.max_select = 2;
        page.is_new = true;
        page.selector = 0;
      }
      if(!page.is_new || !page.btn0_is_held){
        page.is_new = false;
        if(page.prev_selector != page.selector) {
          if(page.selector == 0){
            strcpy((char *)ascii_text, "Object detecting");
            update_page_button(&page, ascii_text, 1, WHITE, BLACK);
            if(page.btn0_is_held){
              page.prev_id = page.id;
              page.is_inited = false;
              page.id = 4;
            }
          }else{
            strcpy((char *)ascii_text, "Object detecting >");
            update_page_button(&page, ascii_text, 1, BLACK, WHITE);
          }
          if(page.selector == 1){
            strcpy((char *)ascii_text, "Back");
            update_page_button(&page, ascii_text, 1, WHITE, BLACK);
            if(page.btn0_is_held){
              page.is_inited = false;
              page.id = page.prev_id;
              //page.prev_id = page.id;
            }
          }else{
            strcpy((char *)ascii_text, "< Back");
            update_page_button(&page, ascii_text, 1, BLACK, WHITE);
          }
          //strcpy((char *)ascii_text, "Wifi settings");
          //if(page.selector == 1){
          //  update_page_button(&page, ascii_text, 2, WHITE, BLACK);
          //  if(page.btn0_is_held){
          //    page.is_inited = false;
          //    page.is_new = true;
          //    page.id = 2;
          //  }
          //}else{
          //  strcpy((char *)ascii_text, "Wifi settings >");
          //  update_page_button(&page, ascii_text, 2, BLACK, WHITE);
          //}
          //strcpy((char *)ascii_text, "Server settings");
          //if(page.selector == 2){
          //  update_page_button(&page, ascii_text, 3, WHITE, BLACK);
          //  if(page.btn0_is_held){
          //    page.is_inited = false;
          //    page.is_new = true;
          //    page.id = 3;
          //  }
          //}else{
          //  strcpy((char *)ascii_text, "Server settings >");
          //  update_page_button(&page, ascii_text, 3, BLACK, WHITE);
          //}
        }
      }
    }else if(page.id == 2){

    }else if(page.id == 3){

    }else if(page.id == 4){

    }
    page.prev_selector = page.selector;
      ////lcdSetFontFill(&dev, CYAN);
      //lcdDrawFillRect(&dev, LCD_WIDTH-11, 0, LCD_WIDTH, LCD_HEIGHT, DARKBLUE);
      //lcdDrawString(&dev, fx16, LCD_WIDTH-fontHeight-3, LCD_HEIGHT/2-(fontWidth*strlen((char *)ascii_text)/2), ascii_text, IGREEN);
    //ESP_LOGI(TAG, "btns : b1-4-%d, b0-33-%d", gpio_get_level(B1_GPIO_PIN), gpio_get_level(B0_GPIO_PIN));
    //gpio_get_level(B0_GPIO_PIN);
    //vTaskDelay(1000);
    //ESP_ERROR_CHECK(gpio_set_level(B1_GPIO_PIN, 0));
    //vTaskDelay(1000);
    //ESP_ERROR_CHECK(gpio_set_level(B1_GPIO_PIN, 1));
  }
  vTaskDelete(NULL);
}

static void initialization_task(void *arg) {
  int init_status = 0;
  esp_err_t iret = 0;
  lcdFillScreen(&dev, BLACK);
  while (1) {
    strcpy((char *)ascii_text, "ARG OS");
    lcdDrawString(&dev, fx16, (LCD_WIDTH-(fontHeight*1))/2-2, LCD_HEIGHT/2-(fontWidth*strlen((char *)ascii_text)/2), ascii_text, IGREEN);
    strcpy((char *)ascii_text, "initialization...");
    lcdDrawString(&dev, fx16, -2, 1, ascii_text, RED);
    lcdDrawRect(&dev, progress_bar[0], progress_bar[1], progress_bar[2], progress_bar[3], GRAY);
    if(init_status < 1){
      init_text("init uart...");
      iret = uart_init();
      if (iret == ESP_OK) {
        init_text("uart: init ok");
        init_status = 1;
        lcdDrawFillRect(&dev, progress_bar[0]+1, progress_bar[1]+1, progress_bar[2]-1, 32, IGREEN2);
        continue;
      } else init_text("uart: init fail");
      vTaskDelay(250);
    }else init_text("uart: inited");
    if(init_status < 2){
      init_text("init nvs...");
      iret = nvs_init();
      if (iret == ESP_OK) {
        init_text("nvs: init ok");
        init_status = 2;
        lcdDrawFillRect(&dev, progress_bar[0]+1, progress_bar[1]+1, progress_bar[2]-1, 64, IGREEN2);
        continue;
      } else init_text("nvs: init fail");
      vTaskDelay(250);
    }else init_text("nvs: inited");
    if(init_status < 3){
      init_text("init cam module...");
      iret = camera_init();
      if (iret == ESP_OK) {
        init_text("cam module: init ok");
        init_status = 3;
        lcdDrawFillRect(&dev, progress_bar[0]+1, progress_bar[1]+1, progress_bar[2]-1, 96, IGREEN2);
        continue;
      } else init_text("cam module: init fail");
      vTaskDelay(250);
    }else init_text("cam module: inited");
    if(init_status < 4){
      init_text("init wifi...");//добавить инит текст для соединения с вайфаем
      iret = wifi_init_sta();
      if (iret == ESP_OK) {
        init_text("wifi: init ok");
        init_status = 4;
        lcdDrawFillRect(&dev, progress_bar[0]+1, progress_bar[1]+1, progress_bar[2]-1, 127, IGREEN2);
        continue;
      } else init_text("wifi: init fail");
      vTaskDelay(250);
    }else init_text("wifi: inited");
    //init_process = 0;
    xTaskCreate(main_handler_task, "main_handler_task", 8192, NULL, 10, NULL);
    break;
  }
  vTaskDelete(NULL);
}

static void SPIFFS_Directory(char *path) {
  DIR *dir = opendir(path);
  assert(dir != NULL);
  while (true) {
    struct dirent *pe = readdir(dir);
    if (!pe) break;
    ESP_LOGI(TAG, "d_name=%s d_ino=%d d_type=%x", pe->d_name, pe->d_ino, pe->d_type);
  }
  closedir(dir);
}

//static void task_info_logger(void *arg) {
//  ESP_LOGI(TAG, "Info Tasker init!");
//  while(1){
//    ESP_LOGI(TAG, "task id: %s", (char *)test_tr);
//    vTaskDelay(1000 / portTICK_PERIOD_MS);
//  }
//}

void init_main(void) {
  ESP_ERROR_CHECK(spiffs_init());
  InitFontx(fx16, "/spiffs/fonts/TAMSYN.FNT","");
  spi_master_init(&dev, LCD_MOSI, LCD_SCLK, LCD_CS, LCD_DC, LCD_RST);
  lcdInit(&dev, LCD_WIDTH, LCD_HEIGHT, LCD_OFFSET_X, LCD_OFFSET_Y);
  GetFontx(fx16, 0, fontBuffer, &fontWidth, &fontHeight);
  lcdFillScreen(&dev, BLACK);
  lcdSetFontDirection(&dev, 1);
  ESP_LOGI(TAG, "LCD init!");
}

void app_main(void) {
  esp_log_level_set(TAG, ESP_LOG_INFO);
  //test_tr = "task1";
  init_main();
  SPIFFS_Directory("/spiffs");
  xTaskCreate(initialization_task, "initialization_task", 3072, NULL, 1, NULL);
  ESP_LOGI(TAG, "testtttt");
  //xTaskCreate(task_info_logger, "task_info_logger", 1024, NULL, 2, NULL);
}