/* dockapp.h - support functions for a WindowMaker docked application
 *
 * Copyright (C) 2000-2002 Casey Harkins (charkins@pobox.com)
 *
 * DESCRIPTION:
 *   This small library of functions should simplify the creation of
 *   WindowMaker docked applications. First the user must make a call to
 *   create_dockapp. Then calls to set_paint_func and set_mouse_func to
 *   set the appropriate callback functions. Finally calling run_dockapp
 *   enters the event loop which calls the appropriate callbacks. The
 *   callbacks receive parameters which will be useful in handling those
 *   events. If more information is needed, the get_XXXXXX calls provide
 *   some additional information. The clear_window and request_paint
 *   functions may also be called from the callbacks to clear the draw
 *   window or request that the paint callback be called.
 *
 *   For information or for updates to these libraries check:
 *       http://www.pobox.com/~charkins/dockapp.html
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef __DOCKAPP_H__
#define __DOCKAPP_H__

#include "../config.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/xpm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


/* time to wait till the next XPending */
#define _DELAY_TIME	100000

/* MOUSE MACROS */
#define MOUSE_PRESSED  1
#define MOUSE_RELEASED 0

/* tooltip structure */
typedef struct _Tooltip_ {
  Window win;
  GC gc;
  char **text;
  int lines;
  XFontStruct *font;
} Tooltip;


/* dockapp_create
 *	PARAMETERS:
 *		char * appname    : name of application to be registered with X
 *		char * geometry   : X geometry string to use when creating the window
 *		char **interface  : the XPM to use for the background and shape mask
 *		int withdrawn     : if withdrawn is > 0 then the app is drawn in
 *                        withdrawn state (WindowMaker DockApps)
 *                        if withdrawn is zero then it is drawn as a 
 *                        normal window
 *    int argc          : number of command line parameters
 *    char **argv       : array of command line parameters
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_create(char *appname, char *geometry, char **interface,
                   int withdrawn, int argc, char **argv);

/* dockapp_run
 *	PARAMETERS:
 *		none
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_run();

/* dockapp_redraw - requests that the paint function be called
 *  PARAMETERS:
 *      none
 *  RETURN:
 *      none
 */
void dockapp_redraw();

/* dockapp_clear - clears the draw window
 *  PARAMETERS:
 *      none
 *  RETURN:
 *      none
 */
void dockapp_clear();

/* dockapp_set_paint
 *	PARAMETERS:
 *		void (*func)(Display *dsp, Drawable drw, GC g) : pointer to paint 
 *                                                      function
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_set_paint(	void (*func)(Display *dsp, Drawable drw, GC g));

/* dockapp_set_mouse
 *	PARAMETERS:
 *		void (*func)(int x, int y, int b, int s) : pointer to mouse function 
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_set_mouse(	void (*func)(int x, int y, int b, int s));

/* dockapp_init_display
 *
 * RETURN: status. <0 on error, 0 when OK
 */
int  dockapp_init_display(void);

/* dockapp_get_display
 *	RETURN:
 *		Display * : returns display
 */
Display* dockapp_get_display();

/* dockapp_get_screen
 *	RETURN:
 *		int : returns screen
 */
int dockapp_get_screen();

/* dockapp_black_pixel
 *	RETURN:
 *		unsigned long : black pixel value
 */
unsigned long dockapp_black_pixel();

/* dockapp_white_pixel
 *	RETURN:
 *		unsigned long : white pixel value
 */
unsigned long dockapp_white_pixel();

/* dockapp_to_screen_coords
 *   PARAMETERS:
 *     int x             : x coordinate relative to origin of draw_window
 *     int y             : y coordinate relative to origin of draw_window
 *     int *x_return     : returned x coordinate relative to root window
 *     int *y_return     : returned y coordinate relative to root window
 *   RETURN:
 *     int               : returns non-zero on success
 */
int dockapp_to_screen_coords(int x, int y, int *x_return, int *y_return);

/* dockapp_set_background_color
 *  PARAMETERS:
 *    char *: background color name
 *
 */
void dockapp_set_background_color(char *color);

/* dockapp_set_background_pixel
 *  PARAMETERS:
 *    unsigned long pixel : background color
 *
 */
void dockapp_set_background_pixel(unsigned long pixel);

/* dockapp_set_background_pixmap_data
 *  PARAMETERS:
 *    char **data : background pixmap
 *
 */
void dockapp_set_background_pixmap_data(char **data);

/* dockapp_set_background_pixmap_file
 *  PARAMETERS:
 *    char *data : absolute path to background pixmap
 *
 */
void dockapp_set_background_pixmap_file(char *file);


/* dockapp_set_background_pixmap
 *  PARAMETERS:
 *    Pixmap pixmap : background pixmap
 *
 */
void dockapp_set_background_pixmap(Pixmap pixmap);

#ifdef USE_TOOLTIPS

/* dockapp_set_tooltip_foreground
 *  PARAMETERS:
 *    char *: tooltip foreground color name
 *
 */
void dockapp_set_tooltip_foreground(char *color);

/* dockapp_set_tooltip_background
 *  PARAMETERS:
 *    char *: tooltip background color name
 *
 */
void dockapp_set_tooltip_background(char *color);

/* dockapp_set_tooltip
 *	PARAMETERS:
 *		char * (*func) (int x, int y): pointer to tooltip callback function
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_set_tooltip(char * (*func) (int x, int y));


/* dockapp_show_tooltip
 *	PARAMETERS:
 *		int x             : x coordinate of "tip" of the tooltip
 *		int y             : y coordinate of "tip" of the tooltip
 *		char *string      : the string to be displayed in the tooltip
 *		
 *  RETURN:
 *    int : non-zero on success
 */
int dockapp_show_tooltip(int x, int y, char *string);


/* dockapp_hide_tooltip
 *  RETURN:
 *    int : <0 if there is no tooltip to hide
 *          >0 on successful hiding of tooltip
 *          =0 error
 */
int dockapp_hide_tooltip();


/* dockapp_update_tooltip
 */
void dockapp_update_tooltip();

/* dockapp_update_tooltip
 */
void dockapp_process_tooltip();

/* dockapp_tooltip_location
 */
void dockapp_tooltip_location(XSizeHints *sizehints);
#endif

#endif /* not __DOCKAPP_H__ */
