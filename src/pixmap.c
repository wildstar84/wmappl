/* pixmap.c
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

#include <stdio.h>
#include "pixmap.h"

#include <X11/xpm.h>
#include <X11/Xlib.h>

#if HAVE_IMLIB2
#  include <Imlib2.h>
#endif



int create_button_pixmap ( Display        *display,
			   char           *filename,
			   Pixmap         *pixmap,
			   Pixmap         *pixmask,
			   XpmAttributes  *xpmattributes )
{
#if HAVE_IMLIB2
  static int    FirstTime = 1;

  if (FirstTime) {
    //    imlib_set_cache_size(128 * 1024);
    //    imlib_set_font_cache_size(32 * 1024);
    //    imlib_set_color_usage(128);

    imlib_context_set_display(display);
    imlib_context_set_visual(DefaultVisual(display,DefaultScreen(display)));
    imlib_context_set_colormap(DefaultColormap(display, DefaultScreen(display)));
    imlib_context_set_color_modifier(NULL);
    imlib_context_set_progress_function(NULL);
    imlib_context_set_operation(IMLIB_OP_COPY);
    imlib_context_set_dither(1);

    FirstTime=0;
  }

  do {
    int width, height;
    Imlib_Image   image;

    image = imlib_load_image(filename);
    imlib_context_set_image(image);

    width  = imlib_image_get_width();
    height = imlib_image_get_height();

    // pixmap and pixmask are required for imlib_render
    *pixmap = XCreatePixmap(display,
			    RootWindow(display,DefaultScreen(display)),
			    width,
			    height,
			    DefaultDepth(display,DefaultScreen(display)));

    *pixmask = XCreatePixmap(display,
			     RootWindow(display,DefaultScreen(display)),
			     width,
			     height,
			     DefaultDepth(display,DefaultScreen(display)));

    imlib_context_set_drawable(*pixmap);
    imlib_context_set_mask(*pixmask);

    if ( width == 16 && height == 16 ) {
      imlib_render_pixmaps_for_whole_image(pixmap,pixmask);
    }
    else {
      imlib_render_pixmaps_for_whole_image_at_size(pixmap,pixmask,16,16);
    }

    imlib_free_image();

    // Fill in some fields in xpmattributes, required by wmappl.
    // Dimensions are always correct, either because the icon had 
    // the correct size, or because it was resized by Imlib2
    xpmattributes->width  = 16;
    xpmattributes->height = 16;

  } while (0);


#else
  /* load the xpm file */
  if(XpmReadFileToPixmap(display,
			 RootWindow(display, DefaultScreen(display)),
			 filename, pixmap, pixmask, xpmattributes) != XpmSuccess) {
    fprintf(stderr, "pixmap.c (error) : Failed to load XPM file %s.\n", filename);

    return -1;
  }
#endif

  return 0;
}
