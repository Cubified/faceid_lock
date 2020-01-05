/*
 * kinect.h: kinect webcam backend
 */

#ifndef __KINECT_H
#define __KINECT_H

#include <stdlib.h>
#include <string.h>

#include <libfreenect/libfreenect_sync.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define IMAGE_WIDTH  640
#define IMAGE_HEIGHT 480
#define IMAGE_DEPTH  3

/*
 * This is a lot dedicated to doing very little -
 *  disables logging and turns off the LED/IR
 */
int kinect_init(){
  freenect_context *ctx;
  freenect_device  *dev;

  if(freenect_init(&ctx, NULL) < 0){
    goto error_nodev;
  }

  if(freenect_open_device(ctx, &dev, 0) < 0){
    goto error_nodev;
  }

  if(freenect_set_led(dev, FREENECT_OFF) < 0){
    goto error_withdev;
  }

  freenect_set_log_level(ctx, FREENECT_LOG_FATAL);
  freenect_select_subdevices(ctx, FREENECT_DEVICE_CAMERA);

  goto noerr;

error_nodev:;
  freenect_shutdown(ctx);
  return 1;

error_withdev:;
  freenect_close_device(dev);
  freenect_shutdown(ctx);
  return 2;

noerr:;
  freenect_close_device(dev);
  freenect_shutdown(ctx);

  return 0;
}

void kinect_shutdown(){
  freenect_sync_stop();

  remove("/tmp/faceid_lock.png");
}

/*
 * TODO: Check depth
 */
int kinect_check_face(){
  unsigned char *data;
  uint32_t timestamp;
  int out;

  freenect_sync_get_video_with_res((void**)&data, &timestamp, 0, FREENECT_RESOLUTION_MEDIUM, FREENECT_VIDEO_RGB);

  stbi_write_png("/tmp/faceid_lock.png", IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_DEPTH, data, 0);

  out = python_scan();

  return out;
}

#endif
