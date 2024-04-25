#ifndef _CAMERA_DEFS_H
#define _CAMERA_DEFS_H

#include "proc_defs.h"
#include "esp_camera.h"

esp_err_t camera_init();
esp_err_t camera_capture();

#define FRAME_SIZE FRAMESIZE_VGA


#endif /* _CAMERA_DEFS_H */