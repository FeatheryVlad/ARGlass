#include "camera.h"

static const char *TAG = "_ Camera Module";
//#define TAG "UART TEST"

static camera_config_t camera_config = {
  .pin_pwdn = PWDN_GPIO_PIN,
  .pin_reset = RESET_GPIO_PIN,
  .pin_xclk = XCLK_GPIO_PIN,
  .pin_sccb_sda = SIOD_GPIO_PIN,
  .pin_sccb_scl = SIOC_GPIO_PIN,

  .pin_d7 = Y7_GPIO_PIN,
  .pin_d6 = Y6_GPIO_PIN,
  .pin_d5 = Y5_GPIO_PIN,
  .pin_d4 = Y4_GPIO_PIN,
  .pin_d3 = Y3_GPIO_PIN,
  .pin_d2 = Y2_GPIO_PIN,
  .pin_d1 = Y1_GPIO_PIN,
  .pin_d0 = Y0_GPIO_PIN,
  .pin_vsync = VSYNC_GPIO_PIN,
  .pin_href = HREF_GPIO_PIN,
  .pin_pclk = PCLK_GPIO_PIN,

  .xclk_freq_hz = 20000000, // EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode
  .ledc_timer = LEDC_TIMER_0,
  .ledc_channel = LEDC_CHANNEL_0,

  .pixel_format = PIXFORMAT_JPEG, // YUV422,GRAYSCALE,RGB565,JPEG
  .frame_size = FRAME_SIZE,   // QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

  .jpeg_quality = 4,                  // 0-63, for OV series camera sensors, lower number means higher quality
  .fb_count = 1,                      // When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
  .grab_mode = CAMERA_GRAB_WHEN_EMPTY // CAMERA_GRAB_LATEST. Sets when buffers should be filled
};

esp_err_t camera_init() {
  esp_log_level_set(TAG, ESP_LOG_INFO);
  // power up the camera if PWDN pin is defined
  //if (PWDN_GPIO_PIN != -1) {
  //  ESP_ERROR_CHECK(gpio_reset_pin(PWDN_GPIO_PIN));
  //  ESP_ERROR_CHECK(gpio_set_direction(PWDN_GPIO_PIN, GPIO_MODE_OUTPUT));
  //  ESP_ERROR_CHECK(gpio_set_level(PWDN_GPIO_PIN, 1));
  //}

  // initialize the camera
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK) {
    ESP_LOGI(TAG, "Init Failed");
    return err;
  }
  ESP_LOGI(TAG, "Inited!");

  return ESP_OK;
}

esp_err_t camera_capture() {
  // acquire a frame
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGI(TAG, "Capture Failed");
    //esp_camera_fb_return(fb);
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "Captured!");
  // replace this with your own function
  // process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);

  esp_camera_fb_return(fb);
  // return the frame buffer back to the driver for reuse
  return ESP_OK;
}