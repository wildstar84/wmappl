/* dockapp.c - support functions for a WindowMaker docked application
 *  
 * From WMAppl (http://www.pobox.com/~charkins/wmappl.html)
 *  
 * Copyright (C) 2002 Casey Harkins (charkins@pobox.com)
 *
 * Authors:
 * 	Casey Harkins (charkins@pobox.com)
 * 	Jarek (talen@charybda.icm.edu.pl)
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

#include "dockapp.h"
#ifdef USE_TOOLTIPS
#  ifdef HAVE_GETTIMEOFDAY
#    include <sys/time.h>
#  else
#    include <time.h>
#  endif
#endif
#include "options.h"
#include "pixmap.h"

/* some globals */
Display *display;
Window draw_window = 0;
Window normal_window = 0;
Window root_window = 0;
GC gc;
int paint_requested;

extern wmappl_opt *options;

/* callback function pointers */
void (*paint_ptr) (Display * dsp, Drawable drw, GC g); 
void (*mouse_ptr) (int x, int y, int b, int s);
void (*keyboard_ptr) (char kc, KeySym ks, int s);

/* tooltip specific info */
#ifdef USE_TOOLTIPS
#  ifdef HAVE_GETTIMEOFDAY

typedef struct timeval   wmappl_time_t;
#    define DEFAULT_TOOLTIP_DELAY  200  // 200 ms
#    define wmappl_currenttime(x)  gettimeofday((x),NULL)
#    define wmappl_timerclear(x)   timerclear(x)
#    define wmappl_timerisset(x)   timerisset(x)
#    define wmappl_timerdiff(x,y)  ((((y)->tv_sec - (x)->tv_sec) * 1000000 + ((y)->tv_usec - (x)->tv_usec)) / 1000)

#  else

typedef time_t           wmappl_time_t;
#    define DEFAULT_TOOLTIP_DELAY  1
#    define wmappl_currenttime(x)  (time(x))
#    define wmappl_timerclear(x)   ((*x)=(time_t)0)
#    define wmappl_timerisset(x)   ((*x)!=(time_t)0)
#    define wmappl_timerdiff(x,y)  ((*y)-(*x))
#  endif

	#define TOOLTIP_MAX_WIDTH 45
	#define TOOLTIP_GUTTER 5
	#define CURSOR_WIDTH 8
	#define CURSOR_HEIGHT 16	

	char * (*tooltip_ptr) (int x, int y);

	Tooltip tooltip;
	int tooltip_x = 0;
	int tooltip_y = 0;
	unsigned long tooltip_fg;
	unsigned long tooltip_bg;

	int           tooltipDelay = DEFAULT_TOOLTIP_DELAY;
	wmappl_time_t still_time;
#endif


/* dockapp_create
 *	PARAMETERS:
 *		char * appname    : name of application to be registered with X
 *		char * geometry   : X geometry string to use when creating the window
 *		char **interface  : the XPM to use for the background and shape mask
 *      int withdrawn     : if withdrawn is > 0 then the app is drawn in
 *                          withdrawn state (WindowMaker DockApps)
 *                          if withdrawn is zero then it is drawn as a
 *                          normal window
 *		int argc          : number of command line parameters
 *		char **argv		  : array of command line parameters
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_create(char *appname, char *geometry, char **interface,
                   int withdrawn, int argc, char **argv) {
	/* some X structs */
	XSizeHints      xsh;
	XWMHints       *xwmh;
	XClassHint      xch;
	XGCValues       xgcv;
	XTextProperty   xtp;
	Window          win = 0, iconwin = 0;
	Pixmap          pixmap, pixmask;
	XpmAttributes   pixattr;
	int             screen, win_x, win_y, win_w, win_h, garbage;
	unsigned long   blackpixel, whitepixel;

	paint_requested = 0;

	/* set callbacks to NULL */
	mouse_ptr = NULL;
	keyboard_ptr = NULL;
	paint_ptr = NULL;

#ifdef USE_TOOLTIPS
	tooltip_ptr = NULL;
	tooltip.win = 0;
	if (options->tooltipDelay > 0) {
#  ifdef HAVE_GETTIMEOFDAY
	  tooltipDelay = options->tooltipDelay;
	  if ( tooltipDelay < 10 ) {
	    tooltipDelay = 10;
	  }
#  else
	  tooltipDelay = options->tooltipDelay / 1000;
	  if ( options->tooltipDelay % 1000 >= 500 ) {
	    tooltipDelay++;
	  }
	  if ( tooltipDelay == 0 ) {
	    tooltipDelay++;
	  }
#  endif
	}
#endif


	/* get screen and root window */
	screen = DefaultScreen(display);
	root_window = RootWindow(display, screen);

	/* get black and white pixel values */
	blackpixel = BlackPixel(display, screen);
	whitepixel = WhitePixel(display, screen);

	/* set Window Manager size hints */
	xsh.flags = USSize;
	xsh.width = 64;
	xsh.height = 64;

	XWMGeometry(display, screen, geometry, "64x64+0+0", 0, &xsh, &win_x, &win_y, &win_w, &win_h, &garbage);

	/* create the normal window */
	win = XCreateSimpleWindow(display, root_window, win_x, win_y, win_w, win_h, 0, blackpixel, whitepixel);
	if(!win) {
		fprintf(stderr, "dockapp:dockapp_create() - Couldn't create normal window.\n");
		return (0);
	}

	/* set the draw window to the normal window */
	draw_window = win;

	/* store the normal window */
	normal_window = win;

	/* if we're using withdrawn state, create the icon window */
	if(withdrawn) {
		/* create the icon window */
		iconwin = XCreateSimpleWindow(display, root_window, win_x, win_y, win_w, win_h, 0, blackpixel, whitepixel);
		if(!iconwin) {
			fprintf(stderr, "dockapp:dockapp_create() - Couldn't create icon window.\n");
			return (0);
		}

		/* set the draw window to the icon window */
		draw_window = iconwin;
	}

	/* set the valuemask on the XpmAttributes structure */
	pixattr.valuemask = 0;

	/* load interface pixmap */
	if(XpmCreatePixmapFromData(display, root_window, interface, &pixmap, &pixmask, &pixattr) != XpmSuccess) {
		fprintf(stderr, "dockapp:dockapp_create() - Couldn't create interface pixmap.\n");
		return (0);
	}

	/* set background pixmap for the draw window */
	dockapp_set_background_pixmap(pixmap);

	/* shape the draw window if supported */
	if(XShapeQueryExtension(display, &garbage, &garbage)) {
		XShapeCombineMask(display, draw_window, ShapeBounding, 0, 0, pixmask, ShapeSet);
	}
	else {
		fprintf(stderr, "dockapp:dockapp_create() - Shaped window extension not supported.\n");
	}

	/* setup Window Manager hints */
	xwmh = XAllocWMHints();
	xwmh->flags = InputHint | WindowGroupHint;
	xwmh->input = True;
	xwmh->window_group = win;

	/* add withdrawn state and icon window hints if we're using withdrawn */
	if(withdrawn) {
		xwmh->flags = xwmh->flags | IconWindowHint | StateHint;
		xwmh->icon_window = iconwin;
		xwmh->initial_state = WithdrawnState;
	}
	XSetWMHints(display, win, xwmh);

	/* set class hints */
	xch.res_name = appname;
	xch.res_class = appname;
	XSetClassHint(display, win, &xch);

	/* set size hints */
	XSetWMNormalHints(display, win, &xsh);

	/* tell window manager app name */
	if(!XStringListToTextProperty(&appname, 1, &xtp)) {
		fprintf(stderr, "dockapp:dockapp_create() - Couldn't create text property.\n");
		return (0);
	}
	XSetWMName(display, win, &xtp);

	/* create a graphics context */
	gc = XCreateGC(display, draw_window, GCForeground | GCBackground, &xgcv);
	if(!gc) {
		fprintf(stderr, "dockapp:dockapp_create() - Couldn't create graphics context.\n");
		return (0);
	}

	/* select events to catch */
	XSelectInput(display, draw_window, ExposureMask | ButtonPressMask | ButtonReleaseMask |
#ifdef USE_TOOLTIPS
				 PointerMotionMask | EnterWindowMask | LeaveWindowMask |
#endif
				 StructureNotifyMask | KeyPressMask | KeyReleaseMask);

	/* set default tooltip foreground and background colors */
#ifdef USE_TOOLTIPS
	tooltip_fg = dockapp_black_pixel();
	tooltip_bg = dockapp_white_pixel();
#endif

	/* set the command line for restarting */
	XSetCommand(display, win, argv, argc);

	/* map the main window */
	XMapWindow(display, win);

	return 1;
}								/* end dockapp_create */

/* dockapp_run
 *	PARAMETERS:
 *		none
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_run() {
	int close = 0;
	XEvent e;
	KeySym ks;  /* JWT:NEXT 2 FOR HANDLING KEYBOARD NAVIGATION: */
	static char buf[10], n;

#ifdef USE_TOOLTIPS
	wmappl_time_t	now;
#endif

	while(!close) {

		/* sleep while there are no events pending */
		while(XPending(display) == 0) {

#ifdef USE_TOOLTIPS
			if(tooltip_ptr!=NULL) {
			        wmappl_currenttime(&now);
				
#if 0
				printf ( "Delay = %d, Still = %d, Now = %d\n", tooltipDelay, still_time, now );

				printf ( "TimerIsSet = %d, TimerDiff = %d\n", 
					 wmappl_timerisset(&still_time), 
					 wmappl_timerdiff(&still_time,&now) );
#endif

				if(wmappl_timerisset(&still_time) && wmappl_timerdiff(&still_time,&now) > tooltipDelay ) {
				  //				        printf ( "*Debug* OK to show tooltip\n" );
					wmappl_timerclear(&still_time);
					dockapp_show_tooltip(tooltip_x, tooltip_y, tooltip_ptr(tooltip_x, tooltip_y));
				}
			}
#endif

			usleep(_DELAY_TIME);
		}

		/* process all pending events */
		while(XPending(display)) {

			/* get next event ifrom queue */
			XNextEvent(display, &e);

			/* handle events */
			switch (e.type) {
			case ClientMessage:
				paint_requested = 1;
				break;
			case ButtonPress:
				if(mouse_ptr)
					mouse_ptr(e.xbutton.x, e.xbutton.y, e.xbutton.button, MOUSE_PRESSED);

#ifdef USE_TOOLTIPS
				/* if visible, hide tooltip */
				if(tooltip.win) {
					dockapp_hide_tooltip();
				}
#endif
				break;
			case ButtonRelease:
				if(mouse_ptr)
					mouse_ptr(e.xbutton.x, e.xbutton.y, e.xbutton.button, MOUSE_RELEASED);

#ifdef USE_TOOLTIPS
				/* if visible, hide tooltip */
				if(tooltip.win) {
					dockapp_hide_tooltip();
				}
#endif
				break;
			case KeyPress:  /* JWT:HANDLE KEYBOARD NAVIGATION: */
				if (keyboard_ptr)
				{
					n = XLookupString (&(e).xkey, buf, 10, &ks, NULL);
					keyboard_ptr(buf[0], ks, MOUSE_PRESSED);
				}
				break;
			case KeyRelease:  /* JWT:HANDLE KEYBOARD NAVIGATION */
				if (keyboard_ptr)
				{
					n = XLookupString (&(e).xkey, buf, 10, &ks, NULL);
					keyboard_ptr(buf[0], ks, MOUSE_RELEASED);
				}
				break;
#ifdef USE_TOOLTIPS
			case EnterNotify:
				break;
			case MotionNotify:
				if(tooltip_ptr==NULL) {
					break;
				}

				wmappl_timerclear(&still_time);

				if(wmappl_currenttime(&still_time)<0) {
					fprintf(stderr, "dockapp:dockapp_run() - Could not get time since epoch.\n");
					break;
				}

				tooltip_x=e.xbutton.x;
				tooltip_y=e.xbutton.y;

				/* if visible, hide tooltip */
				if(tooltip.win) {
					dockapp_hide_tooltip();
				}

				break;

			case LeaveNotify:
				if(tooltip_ptr==NULL) {
					break;
				}

				wmappl_timerclear(&still_time);
		
				/* if visible, hide tooltip */
				if(tooltip.win) {
					dockapp_hide_tooltip();
				}

				break;
#endif
			case Expose:
				if(e.xexpose.count != 0)
					break;
				paint_requested = 1;
				break;
			case ConfigureNotify:
				paint_requested = 1;
				break;
			case DestroyNotify:
				close = 1;
				break;
			}					/* switch */
			/* call paint function if it has been requested */
			if(paint_requested) {
				paint_requested = 0;
				if(paint_ptr)
					paint_ptr(display, draw_window, gc);
#ifdef USE_TOOLTIPS
				if(tooltip.win)
					dockapp_update_tooltip();
#endif
			}
		}						/* while XPending */
	}							/* while not close */

	return 1;
} /* end dockapp_run */

/* dockapp_redraw - requests that the paint function be called
 *  PARAMETERS:
 *      none
 *  RETURN:
 *      none
 */
void dockapp_redraw() {
	paint_requested = 1;
}

/* dockapp_clear - clears the draw window
 *  PARAMETERS:
 *      none
 *  RETURN:
 *      none
 */
void dockapp_clear() {
	XClearWindow(display, draw_window);
}

/* dockapp_set_paint
 *  PARAMETERS:
 *      void (*func)(Display dsp, Drawable drw, GC g) : pointer to paint 
 *                                                      function
 *  RETURN:
 *      int : returns non-zero on success
 */
int dockapp_set_paint(void (*func) (Display * dsp, Drawable drw, GC g)) {
	if(func) {
		paint_ptr = func;
		return 1;
	}
	return 0;
}

/* dockapp_set_mouse
 *	PARAMETERS:
 *		void (*func)(int x, int y, int b, int s) : pointer to mouse function
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_set_mouse(void (*func) (int x, int y, int b, int s)) {
	if(func) {
		mouse_ptr = func;
		return 1;
	}
	return 0;
}

/* dockapp_set_keyboard
 *	PARAMETERS:
 *		void (*func)(char keychar, KeySym ks, int s) : pointer to keyboard function
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_set_keyboard(void (*func) (char kc, KeySym ks, int s)) {
	if(func) {
		keyboard_ptr = func;
		return 1;
	}
	return 0;
}

/* dockapp_init_display
 *
 * RETURN: status. <0 on error, 0 when OK
 */
int  dockapp_init_display(void) {
  /* connect to default display */
  display = XOpenDisplay((char *) getenv("DISPLAY"));
  if(!display) {
    fprintf(stderr, "dockapp:dockapp_init_display() - Couldn't connect to display.\n");
    return -1;
  }
  return 0;
}

/* dockapp_get_display()
 *  RETURN:
 *      Display * : returns display
 */
Display *dockapp_get_display() {
	return display;
}

/* dockapp_get_screen()
 *  RETURN:
 *      int : returns screen
 */
int dockapp_get_screen() {
	return DefaultScreen(display);
}

/* dockapp_black_pixel()
 *  RETURN:
 *      unsigned long : black pixel value
 */
unsigned long dockapp_black_pixel() {
	return BlackPixel(display, dockapp_get_screen());

}

/* dockapp_white_pixel()
 *  RETURN:
 *      unsigned long : white pixel value
 */
unsigned long dockapp_white_pixel() {
	return WhitePixel(display, dockapp_get_screen());

}

/* dockapp_to_screen_coords
 *   PARAMETERS:
 *     int x             : x coordinate relative to origin of draw_window
 *     int y             : y coordinate relative to origin of draw_window
 *     int *x_return     : returned x coordinate relative to root window
 *     int *y_return     : returned y coordinate relative to root window
 *   RETURN:
 *     int               : returns non-zero on success
 */
int dockapp_to_screen_coords(int x, int y, int *x_return, int *y_return) {
	Window c;

	if(XTranslateCoordinates(display, draw_window, root_window, x, y, x_return, y_return, &c) == 0) {
		fprintf(stderr, "dockapp:get_screen_coordinates() - Translate error!\n");
	}
	return 1;
}

/* dockapp_set_background_color
 *  PARAMETERS:
 *	  char *: background color name
 *
 */
void dockapp_set_background_color(char *color) {
	XColor xcolor;
	Colormap colormap = XDefaultColormap(display, dockapp_get_screen());

	if(XParseColor(display, colormap, color, &xcolor)==0) {
		fprintf(stderr, "Could not parse color: %s\n", color);
		return;
	}
	if(XAllocColor(display, colormap, &xcolor)==0) {
		fprintf(stderr, "Could not allocate color: %s\n", color);
		return;
	}
	dockapp_set_background_pixel(xcolor.pixel);
}

/* dockapp_set_background_pixel
 *  PARAMETERS:
 *	  unsigned long pixel : background color
 *
 */
void dockapp_set_background_pixel(unsigned long pixel) {
	XSetWindowBackground(display, draw_window, pixel);
	dockapp_clear();
}

/* dockapp_set_background_pixmap_data
 *  PARAMETERS:
 *	  char **data : background pixmap
 *
 */
void dockapp_set_background_pixmap_data(char **data) {
	XpmAttributes   pixattr;
	Pixmap pixmap, pixmask;

	/* set the valuemask on the XpmAttributes structure */
	pixattr.valuemask = 0;

	/* load pixmap */
	if(XpmCreatePixmapFromData(display, root_window, data, &pixmap, &pixmask, &pixattr) != XpmSuccess) {
		return;
	}

	dockapp_set_background_pixmap(pixmap);
}

/* dockapp_set_background_pixmap_file
 *  PARAMETERS:
 *    char *data : absolute path to background pixmap
 *
 */
void dockapp_set_background_pixmap_file(char *file) {
	XpmAttributes   pixattr;
	Pixmap pixmap, pixmask;

	/* set the valuemask on the XpmAttributes structure */
	pixattr.valuemask = 0;

	/* load pixmap */
	create_button_pixmap(display, file, &pixmap, &pixmask, &pixattr);

	dockapp_set_background_pixmap(pixmap);
}

/* dockapp_set_background_pixmap
 *  PARAMETERS:
 *	  Pixmap pixmap : background pixmap
 *
 */
void dockapp_set_background_pixmap(Pixmap pixmap) {
	XSetWindowBackgroundPixmap(display, draw_window, pixmap);
	dockapp_clear();
}

#ifdef USE_TOOLTIPS

/* dockapp_set_tooltip_foreground
 *  PARAMETERS:
 *	  char *: tooltip foreground color name
 *
 */
void dockapp_set_tooltip_foreground(char *color) {
	XColor xcolor;
	Colormap colormap = XDefaultColormap(display, dockapp_get_screen());

	if(XParseColor(display, colormap, color, &xcolor)==0) {
		fprintf(stderr, "Could not parse color: %s\n", color);
		return;
	}
	if(XAllocColor(display, colormap, &xcolor)==0) {
		fprintf(stderr, "Could not allocate color: %s\n", color);
		return;
	}
	tooltip_fg=xcolor.pixel;
}

/* dockapp_set_tooltip_background
 *  PARAMETERS:
 *	  char *: tooltip background color name
 *
 */
void dockapp_set_tooltip_background(char *color) {
	XColor xcolor;
	Colormap colormap = XDefaultColormap(display, dockapp_get_screen());

	if(XParseColor(display, colormap, color, &xcolor)==0) {
		fprintf(stderr, "Could not parse color: %s\n", color);
		return;
	}
	if(XAllocColor(display, colormap, &xcolor)==0) {
		fprintf(stderr, "Could not allocate color: %s\n", color);
		return;
	}
	tooltip_bg=xcolor.pixel;
}

/* dockapp_set_tooltip
 *	PARAMETERS:
 *		char * (*func)(int x, int y) : pointer to tooltip function
 *
 *	RETURN:
 *		int : returns non-zero on success
 */
int dockapp_set_tooltip(char * (*func) (int x, int y)) {
	if(func) {
		tooltip_ptr = func;
		return 1;
	}
	return 0;
}								/* end tooltip_func */

/* dockapp_show_tooltip(int x, int y, char *string)
 *  PARAMETERS:
 *    int x             : x coordinate of "tip" of the tooltip
 *    int y             : y coordinate of "tip" of the tooltip
 *    char *string      : the string to be displayed in the tooltip
 *    
 *  RETURN:
 *    int : non-zero on success
 */
int dockapp_show_tooltip(int x, int y, char *string) {
	XGCValues       gcvals;
	XSizeHints      sizehints;
	XSetWindowAttributes winattribs;

	if(tooltip.win) {
		dockapp_hide_tooltip();
	}


	/* return if string is NULL */
	if(string==NULL) {
		return 0;
	}

	/* query the font if it hasn't been already */
	if(!tooltip.font) {
		if(options->tooltipfont) {
			tooltip.font = XLoadQueryFont(display, options->tooltipfont);
		}
		if(tooltip.font == NULL) {
			tooltip.font = XLoadQueryFont(display, "fixed");
		}
		if(tooltip.font == NULL) {
			fprintf(stderr, "dockapp:dockapp_show_tooltip() - Couldn't load fixed font!\n");
			return (0);
		}
	}

	/* process string into lines of appropriate width */
	dockapp_process_tooltip(string);

	/* map the tooltip window */
	/* set the size hints structure */
	sizehints.flags = USSize | USPosition;

	dockapp_to_screen_coords(x, y, &sizehints.x, &sizehints.y);

	/* sizehints.x = x;
	   sizehints.y = y; */

	if(tooltip.lines == 1) {
		sizehints.width = XTextWidth(tooltip.font, tooltip.text[0], strlen(tooltip.text[0])) + 2;
	}
	else if(tooltip.lines > 1) {
		sizehints.width = XTextWidth(tooltip.font, tooltip.text[0], strlen(tooltip.text[0])) + 4;
	}
	else {
		sizehints.width = 0;
	}
	sizehints.height = ((tooltip.font->ascent + tooltip.font->descent + 4) * tooltip.lines);

	/* ensure tooltip is entirely on screen */
	dockapp_tooltip_location(&sizehints);

	/* create the window */
	tooltip.win = XCreateSimpleWindow(display, DefaultRootWindow(display),
									  sizehints.x, sizehints.y, sizehints.width, sizehints.height,
									  0, dockapp_black_pixel(), dockapp_white_pixel());
	if(!tooltip.win) {
		fprintf(stderr, "dockapp:dockapp_show_tooltip() - Couldn't create tooltip window!\n");
		return (0);
	}

	/* set the size hints */
	XSetWMNormalHints(display, tooltip.win, &sizehints);

	/* set the window attributes */
	winattribs.save_under = True;
	winattribs.override_redirect = True;
	XChangeWindowAttributes(display, tooltip.win, CWSaveUnder | CWOverrideRedirect, &winattribs);

	/* set the window name */
	XStoreName(display, tooltip.win, "tooltip");

	/* select to receive exposure events */
	XSelectInput(display, tooltip.win, ExposureMask);

	/* create the graphics context if it hasn't been created */
	if(!tooltip.gc) {
		gcvals.foreground = tooltip_fg;
		gcvals.background = tooltip_bg;
		gcvals.graphics_exposures = 0;
		tooltip.gc = XCreateGC(display, tooltip.win, GCForeground | GCBackground | GCGraphicsExposures, &gcvals);
		if(tooltip.gc == NULL) {
			fprintf(stderr, "dockapp:dockapp_show_tooltip() - Couldn't create graphics context!\n");
			return (0);
		}

		XSetFont(display, tooltip.gc, tooltip.font->fid);
	}

	XSetWindowBackground(display, tooltip.win, tooltip_bg);

	XMapRaised(display, tooltip.win);

	return 1;
}

int dockapp_hide_tooltip() {
	int i = 0;

	if(!tooltip.win)
		return -1;
	XUnmapWindow(display, tooltip.win);
	for(i = 0; i < tooltip.lines; i++) {
		free(tooltip.text[i]);
	}
	free(tooltip.text);
	tooltip.lines = 0;
	tooltip.win = 0;
	XFlush(display);
	return 1;
}

void dockapp_update_tooltip() {
	int i=0;

	if(!tooltip.win || !tooltip.gc || !tooltip.text) {
		return;
	}

	for(i = 0; i < tooltip.lines; i++) {
		XDrawString(display, tooltip.win, tooltip.gc, 1,
					(i + 1) * (tooltip.font->ascent + tooltip.font->descent + 4) - 4,
					tooltip.text[i], strlen(tooltip.text[i]));
	}


	XFlush(display);
}


void dockapp_process_tooltip(char *string)
{
	int             length = 0;
	int             size = 0;
	int             i = 0;
	char           *pointer = NULL;

	length = strlen(string);

	/* calculate number of lines */
	tooltip.lines = length / TOOLTIP_MAX_WIDTH;
	if(length % TOOLTIP_MAX_WIDTH != 0)
		tooltip.lines++;

	/* allocate pointers to the lines */
	tooltip.text = (char **) malloc(sizeof(char *) * tooltip.lines);

	pointer = string;

	for(i = 0; i < tooltip.lines; i++) {
		/* allocate the line */
		if(length < TOOLTIP_MAX_WIDTH) {
			tooltip.text[i] = (char *) malloc(sizeof(char) * (length + 1));
			size = length;
		}
		else {
			tooltip.text[i] = (char *) malloc(sizeof(char) * (TOOLTIP_MAX_WIDTH + 1));
			size = TOOLTIP_MAX_WIDTH;
		}

		strncpy(tooltip.text[i], pointer, size);
		tooltip.text[i][size] = '\0';

		length = length - size;
		pointer = &pointer[size];

	}
}


void dockapp_tooltip_location(XSizeHints *sizehints) {

	XWindowAttributes rootattr;

	if(XGetWindowAttributes(display, root_window, &rootattr) == 0) {
		fprintf(stderr, "dockapp:set_tooltip_location() - Couldn't determine screen size, tooltips may run off of the screen.\n");
		return;
	}

	/* check if tooltip should go to the right or left of the mouse cursor */
	if(sizehints->x < rootattr.width/2) {
		/* tooltip goes to the right, need to account for mouse cursor width */
		sizehints->x=sizehints->x+TOOLTIP_GUTTER+CURSOR_WIDTH;
	}
	else {
		/* tooltip goes to the left, need to account for the tooltip width */
		sizehints->x=sizehints->x-TOOLTIP_GUTTER-sizehints->width;
	}

	/* check if tooltip should go above or below the mouse cursor */
	if(sizehints->y < rootattr.height/2) {
		/* tooltip goes below, need to account for mouse cursor height */
		sizehints->y=sizehints->y+TOOLTIP_GUTTER+CURSOR_HEIGHT;
	}
	else {
		/* tooltip goes above, need to account for tooltip height */
		sizehints->y=sizehints->y-TOOLTIP_GUTTER-sizehints->height;
	}

}



#endif
