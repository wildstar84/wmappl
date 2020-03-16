
/* xpmbutton.c
 *
 * WMAppl - Window Maker Application Launcher
 * Copyright (C) 1999-2002 Casey Harkins
 *
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

#include "xpmbutton.h"
#include "pixmap.h"

/* create_xpmbutton
 *	PARAMETERS:
 *      char *filename : xpm filename to load 
 *		Display *display: X11 display
 *	RETURN:
 *		XpmButton* : an XpmButton structure
 */
XpmButton      *create_xpmbutton(char *filename,
								 Display * display)
{
	XpmButton      *xpmbutton;
	XpmAttributes   xpmattributes;

	xpmattributes.valuemask = 0;
	xpmbutton = (XpmButton *) malloc(sizeof(XpmButton));
	memset(xpmbutton,0,sizeof(xpmbutton));
	/* allocate an XpmButton structure */
	if(!xpmbutton) {
		fprintf(stderr, "xpmbutton.c (warning): Couldn't allocate space for a new XpmButton.\n");
		return NULL;
	}

	if ( create_button_pixmap (display,filename,
				   &xpmbutton->pixmap, &xpmbutton->pixmask, &xpmattributes) < 0 ) {
	        return NULL;
	}

	/* set the XpmButton defaults */
	xpmbutton->x = 0;
	xpmbutton->y = 0;
	xpmbutton->width = xpmattributes.width + 2;
	xpmbutton->height = xpmattributes.height + 2;
	xpmbutton->pressed = 0;
	xpmbutton->command = NULL;
	xpmbutton->tooltip = NULL;
	xpmbutton->last = NULL;
	xpmbutton->next = NULL;

	return xpmbutton;
}

/* create_include_xpmbutton
 *	PARAMETERS:
 *      static char **data : xpm character array to load
 *		Display *display: X11 display
 *	RETURN:
 *		XpmButton* : an XpmButton structure
 */
XpmButton      *create_include_xpmbutton(char **data,
										 Display * display)
{
	XpmButton      *xpmbutton;
	XpmAttributes   xpmattributes;

	xpmattributes.valuemask = 0;
	xpmbutton = (XpmButton *) malloc(sizeof(XpmButton));
	/* allocate an XpmButton structure */
	if(!xpmbutton) {
		fprintf(stderr, "xpmbutton.c (warning): Couldn't allocate space for a new XpmButton.\n");
		return NULL;
	}
	/* load the xpm data */
	if(XpmCreatePixmapFromData(display,
							   RootWindow(display, DefaultScreen(display)),
							   data, &xpmbutton->pixmap, &xpmbutton->pixmask, &xpmattributes) != XpmSuccess) {
		fprintf(stderr, "xpmbutton.c (error) : Failed to load XPM data.\n");
		return NULL;
	}
	/* set the XpmButton defaults */
	xpmbutton->width = xpmattributes.width + 2;
	xpmbutton->height = xpmattributes.height + 2;
	xpmbutton->pressed = 0;
	xpmbutton->command = NULL;
	xpmbutton->tooltip = NULL;
	xpmbutton->last = NULL;
	xpmbutton->next = NULL;

	return xpmbutton;
}


/* set_xpmbutton_command
 *	PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set command for
 *		char *command        : command to set in xpmbutton
 *	RETURN:
 *		none
 */
void set_xpmbutton_command(XpmButton *xpmbutton,
						   char *command)
{
	if(!xpmbutton)
		return;

	if(command==NULL) {
		xpmbutton->command = NULL;
	}
	else {
		xpmbutton->command = (char *) malloc(strlen(command)+1);
		strncpy(xpmbutton->command, command, strlen(command)+1);
	}
}

/* set_xpmbutton_tooltip
 *	PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set tooltip for
 *		char *tooltip        : tooltip for xpmbutton
 *	RETURN:
 *		none
 */
void set_xpmbutton_tooltip(XpmButton *xpmbutton,
						   char *tooltip)
{
	if(!xpmbutton)
		return;

	if(tooltip==NULL) {
		xpmbutton->tooltip = NULL;
	}
	else {
		xpmbutton->tooltip = (char *) malloc(strlen(tooltip)+1);
		strncpy(xpmbutton->tooltip, tooltip, strlen(tooltip)+1);
	}
}

/* set_xpmbutton_last
 *	PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set list pointer for
 *		XpmButton *last      : the previous XpmButton in the list
 *	RETURN:
 *		none
 */
void set_xpmbutton_last(XpmButton *xpmbutton,
						XpmButton *last)
{
	if(!xpmbutton)
		return;
	xpmbutton->last = last;
}

/* set_xpmbutton_next
 *	PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set list pointer for
 *		XpmButton *next      : the next XpmButton in the list
 *	RETURN:
 *		none
 */
void set_xpmbutton_next(XpmButton *xpmbutton,
						XpmButton *next)
{
	if(!xpmbutton)
		return;
	xpmbutton->next = next;
}

/* set_xpmbutton_position
 *	PARAMETERS:
 *		XpmButton *xpmbutton : XpmButton to set position for
 * 		int x  : x coordinate of xpmbutton     
 *		int y  : y coordinate of xpmbutton
 *	RETURN:
 *		none
 */
void set_xpmbutton_position(XpmButton *xpmbutton,
							int x,
							int y)
{
	if(!xpmbutton)
		return;
	xpmbutton->x = x;
	xpmbutton->y = y;
}

/* set_xpmbutton_pressed
 *  PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to set state for
 *      int pressed  : set to nonzero if button should be
 *                     drawn pressed
 *  RETURN:
 *      none
 */
void set_xpmbutton_pressed(XpmButton *xpmbutton,
						   int pressed)
{
	if(!xpmbutton)
		return;
	xpmbutton->pressed = pressed;
}

/* is_xpmbutton_at
 *  PARAMETERS:
 *      XpmButton *xpmbutton : XpmButton to check 
 *      int x  : x coordinate to check
 *      int y  : y coordinate to check
 *  RETURN:
 *      int : returns nonzero if x,y is within the coordinates
 *            of xpmbutton
 */
int is_xpmbutton_at(XpmButton *xpmbutton,
					int x,
					int y)
{
	if(!xpmbutton)
		return 0;
	if(x >= xpmbutton->x && x <= xpmbutton->x + xpmbutton->width) {
		if(y >= xpmbutton->y && y <= xpmbutton->y + xpmbutton->height) {
			return 1;
		}
	}
	return 0;
}

/* draw_xpmbutton
 *	PARAMETERS:
 *		XpmButton *xpmbutton : XpmButton to draw
 *		Display *display     : display to draw on
 *		Drawable drawable    : drawable to draw into
 *		GC gc                : graphics context to use for drawing
 *	RETURN:
 *		none
 */
void draw_xpmbutton(XpmButton *xpmbutton,
					Display * display,
					Drawable drawable,
					GC gc)
{
	Pixel           bl, wh;
	int             x, y, w, h;

	if(!xpmbutton)
		return;

	bl = BlackPixel(display, DefaultScreen(display));
	wh = WhitePixel(display, DefaultScreen(display));

	x = xpmbutton->x;
	y = xpmbutton->y;
	w = xpmbutton->width;
	h = xpmbutton->height;

	if(xpmbutton->pressed)
		XSetForeground(display, gc, bl);
	else
		XSetForeground(display, gc, wh);

	XDrawLine(display, drawable, gc, x, y, x + (w - 1), y);
	XDrawLine(display, drawable, gc, x, y, x, y + (h - 2));

	if(xpmbutton->pressed)
		XSetForeground(display, gc, wh);
	else
		XSetForeground(display, gc, bl);

	XDrawLine(display, drawable, gc, x, y + (h - 1), x + (w - 1), y + (h - 1));
	XDrawLine(display, drawable, gc, x + (w - 1), y + 1, x + (w - 1), y + (h - 1));

	/* set clipping mask to allow transparency */
	XSetClipOrigin(display, gc, x+1, y+1);
	XSetClipMask(display, gc, xpmbutton->pixmask);

	XCopyArea(display, xpmbutton->pixmap, drawable, gc, 0, 0, w - 2, h - 2, x + 1, y + 1);

	/* clear clipping mask */
	XSetClipMask(display, gc, None);
}
