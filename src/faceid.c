/*
 * faceid.c: a simple xlock with face recognition
 */

#define _GNU_SOURCE
#include <stdio.h>

#include "x.h"
#include "python.h"
#include "kinect.h"

enum {
  state_running = -1,
  state_invalid = 0,
  state_valid = 1
};

int state = state_running;

int already_running(){
  int out = 1;

  FILE *fp = fopen("/tmp/faceid_lock", "r");
  if(fp == NULL){
    fp = fopen("/tmp/faceid_lock", "w");
    fclose(fp);
    out = 0;
  }

  return out;
}

void lock_cleanup(){
  remove("/tmp/faceid_lock");
}

int main(int argc, char **argv){
  if(argc == 2){
    printf("Running in fix_kinect mode.\n");
    kinect_init();
    printf("Done.\n");
    return 0;
  }

  if(already_running()){
    printf("Error: faceid_lock is already running.\n");
    return 1;
  }

  if(x_init() != 0){
    printf("Failed to open display.\n");
    goto error;
  }

  x_draw_text("Setting up...", 14);

  if(kinect_init() != 0){
    printf("Failed to initialize Kinect.\n");
    goto error;
  }

  if(python_init() != 0){
    printf("Failed to initialize Python subprocess.\n");
    goto error;
  }

  x_draw_text("Click to login.", 16);

  while(state != state_valid){
    if(x_check_event()){
      x_draw_text("Checking...", 12);
      state = kinect_check_face();
      if(state == state_invalid){
        x_draw_text("Face not recognized.", 20);
      } else {
        x_draw_text("Success.", 9);
      }
    } 
  }

error:;
  x_shutdown();
  kinect_shutdown();
  python_shutdown();
  lock_cleanup();
 
  return 0;
}
