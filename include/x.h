/*
 * x.h: Xlib frontend
 */

#ifndef __X_H
#define __X_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>

#ifdef DEBUG_BUILD
#  define MONITOR_WIDTH  100
#  define MONITOR_HEIGHT 100
#else
#  define MONITOR_WIDTH  1920
#  define MONITOR_HEIGHT 1080
#endif

#define CENTER(dim, pos) ((dim-pos)/2)

Display *dpy;
Window root,
       win;
Screen *scr;

XftFont *font;
XftDraw *draw;
XRenderColor xrcolor_white;
XRenderColor xrcolor_black;
XftColor xftcolor_white;
XftColor xftcolor_black;

XColor x_get_color(Display *dpy, char *color_name){
  Colormap cmap;
  XColor near_color, true_color;

  cmap = DefaultColormap(dpy, 0);
  XAllocNamedColor(dpy, cmap, color_name, &near_color, &true_color);
  return near_color;
}

int x_init(){
  XSetWindowAttributes attrs;

  dpy = XOpenDisplay(NULL);

  if(dpy == NULL){
    return 1;
  }

  root = DefaultRootWindow(dpy);
  scr = DefaultScreenOfDisplay(dpy);

  attrs.override_redirect = True;
  attrs.background_pixel = BlackPixel(dpy, DefaultScreen(dpy));
  attrs.event_mask = SubstructureNotifyMask|ButtonPressMask;

  win = XCreateWindow(
    dpy,
    root,
    0, 0,
#ifdef DEBUG_BUILD
    100, 100,
#else
    XWidthOfScreen(scr), XHeightOfScreen(scr),
#endif
    0,
    DefaultDepth(dpy, DefaultScreen(dpy)),
    InputOutput,
    DefaultVisual(dpy, DefaultScreen(dpy)),
    CWOverrideRedirect|CWBackPixel|CWEventMask,
    &attrs
  );

  XSelectInput(dpy, win, SubstructureNotifyMask|ButtonPressMask);

#ifndef DEBUG_BUILD
  XClientMessageEvent evt;
  evt.type = ClientMessage;
  evt.message_type = XInternAtom(dpy,"_NET_WM_STATE",0);
  evt.display = dpy;
  evt.window = win;
  evt.format = 32;
  evt.data.l[0] = 1;
  evt.data.l[1] = XInternAtom(dpy,"_NET_WM_STATE_ABOVE",0);
  XSendEvent(
    dpy,
    root,
    0,
    SubstructureRedirectMask,
    (XEvent*)&evt
  );
  
  XMapRaised(dpy, win);

  XGrabKeyboard(
    dpy,
    win,
    0,
    GrabModeAsync,
    GrabModeAsync,
    CurrentTime
  );

  XGrabPointer(
    dpy,
    win,
    1,
    ButtonPressMask,
    GrabModeAsync,
    GrabModeAsync,
    None,
    None,
    CurrentTime
  );
#else
  XMapWindow(dpy, win);
#endif

  /*
   * XFT
   */

  font = XftFontOpenName(dpy, 0, "scientifica-10");
  draw = XftDrawCreate(dpy, win, DefaultVisual(dpy, 0), DefaultColormap(dpy, 0));

  xrcolor_white.red   = 50535;
  xrcolor_white.green = 50535;
  xrcolor_white.blue  = 50535;
  xrcolor_white.alpha = 65535;
  XftColorAllocValue(dpy, DefaultVisual(dpy, 0), DefaultColormap(dpy, 0), &xrcolor_white, &xftcolor_white);

  xrcolor_black.red   = 13107;
  xrcolor_black.green = 13107;
  xrcolor_black.blue  = 13107;
  xrcolor_black.alpha = 65535;
  XftColorAllocValue(dpy, DefaultVisual(dpy, 0), DefaultColormap(dpy, 0), &xrcolor_black, &xftcolor_black);

  return 0;
}

void x_shutdown(){
  if(dpy != NULL){
    XftDrawDestroy(draw);
    XftColorFree(dpy, DefaultVisual(dpy, 0), DefaultColormap(dpy, 0), &xftcolor_white);
    XftColorFree(dpy, DefaultVisual(dpy, 0), DefaultColormap(dpy, 0), &xftcolor_black);

    XUngrabPointer(dpy, CurrentTime);
    XUngrabKeyboard(dpy, CurrentTime);

    XUnmapWindow(dpy, win);
    XCloseDisplay(dpy);
  }
}

int x_check_event(){
  XEvent evt;
  XNextEvent(dpy, &evt);
  return (evt.type == ButtonPress);
}

void x_draw_text(char *str, int len){
  XftDrawRect(
    draw,
    &xftcolor_white,
    CENTER(MONITOR_WIDTH, 150), CENTER(MONITOR_HEIGHT, 50),
    150, 50
  );
  XftDrawString8(draw, &xftcolor_black, font, CENTER(MONITOR_WIDTH, 5*len), CENTER(MONITOR_HEIGHT, -4), (unsigned char*)str, len);
  XFlush(dpy);
}

#endif
