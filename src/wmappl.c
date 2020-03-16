/*
 * WMAppl - Window Maker Application Launcher
 * Copyright (C) 1999-2003 Casey Harkins
 *
 * Contributors:
 *		Casey Harkins (charkins@pobox.com)
 *      Denilson F. de Sá
 *      Olivier Leconte
 *		Richard Eisenman 
 *		Jacob Beardsley	
 *		Steve Akers
 *		Samoylov Olleg
 *		Jarek
 *
 *		Anyone else that I may have missed!
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

#include <stdlib.h>
#include <stdio.h>
#include "../config.h"
#include "interface.xpm"
#include "leftarr.xpm"
#include "rightarr.xpm"
#include "dockapp.h"
#include "args.h"
#include "xpmbutton.h"
#include "rcparser.h"
#include "fileexists.h"
#include "options.h"

#define WMAPPLRC ".wmapplrc"
#define SYSWMAPPLRC "/etc/wmapplrc"
#define ICONPATH "/usr/share/icons/wmappl:/usr/share/pixmaps/mini"

#define SCROLL_WHEEL_UP   4
#define SCROLL_WHEEL_DOWN 5

wmappl_opt *options = NULL;

XpmButton *icons, *sleft, *sright, *pressed;
int clearwindow = 0;
int button_positions[] = {
	5, 5,
	5, 23,
	23, 5,
	23, 23,
	41, 5,
	41, 23
};

int scroll_positions[] = { 
	5, 46,
	42, 46
};


void set_button_positions() {
	XpmButton *b;
	int i;

	for(i = 0, b = icons; i < 6 && b != NULL; i++, b = b->next) {
		set_xpmbutton_position(b, button_positions[i * 2], button_positions[(i * 2) + 1]);
	}
}

/* scroll buttons left / move back 2 icons in list */
int scroll_left() {
	if(icons->last!=NULL && icons->last->last!=NULL) {
		icons=icons->last->last;
		clearwindow=1;
		set_button_positions();
		return 1;
	}
	return 0;
}

/* scroll buttons right / move forward 2 icons in list */
int scroll_right() {
	if(    icons->next!=NULL 
		&& icons->next->next!=NULL 
		&& icons->next->next->next!=NULL
		&& icons->next->next->next->next!=NULL
		&& icons->next->next->next->next->next!=NULL
		&& icons->next->next->next->next->next->next!=NULL) {
			icons=icons->next->next;
			clearwindow=1;
			set_button_positions();
			return 1;
	}
	return 0;
}


#ifdef USE_TOOLTIPS
char * wmappl_tooltip(int x, int y) {
	int i;
	XpmButton *p = NULL;

	/* check visible icons */
	for(i = 0, p = icons; i < 6 && p != NULL; i++, p = p->next) {
		if(is_xpmbutton_at(p, x, y)) return p->tooltip;
	}

	/* check scroll buttons */
	if(is_xpmbutton_at(sleft, x, y)) return sleft->tooltip;
	if(is_xpmbutton_at(sright, x, y)) return sright->tooltip;

	return NULL;
}
#endif

/* paint callback */
void wmappl_paint(Display * dsp, Drawable drw, GC gc) {
	XpmButton *p;
	int i;

	/* clear the window if needed */
	if(clearwindow) {
		dockapp_clear();
		/* reset the clearwindow variable */
		clearwindow = 0;
	}

	/* draw the six visible buttons */
	for(i = 0, p = icons; i < 6 && p != NULL; i++, p = p->next)
		draw_xpmbutton(p, dsp, drw, gc);

	/* draw the scroll buttons */
	draw_xpmbutton(sleft, dsp, drw, gc);
	draw_xpmbutton(sright, dsp, drw, gc);

}

/* mouse callback */
void wmappl_mouse(int x, int y, int z, int s) {
	int i;
	XpmButton *p = NULL, *released = NULL;

	if(z==SCROLL_WHEEL_UP && s==MOUSE_PRESSED) {
		scroll_left();
		dockapp_redraw();
	}
	else if(z==SCROLL_WHEEL_DOWN && s==MOUSE_PRESSED) {
		scroll_right();
		dockapp_redraw();
	}
	else if(s == MOUSE_PRESSED) {
		/* clear pressed marker if it is pointing somewhere */
		if(pressed != NULL) {
			set_xpmbutton_pressed(pressed, 0);
			pressed = NULL;
		}

		/* check visible icons */
		for(i = 0, p = icons; i < 6 && p != NULL; i++, p = p->next) {
			if(is_xpmbutton_at(p, x, y))
				pressed = p;
		}

		/* check scroll buttons */
		if(is_xpmbutton_at(sleft, x, y))
			pressed = sleft;
		if(is_xpmbutton_at(sright, x, y))
			pressed = sright;

		/* if a button was pressed, mark it and request repaint */
		if(pressed != NULL) {
			set_xpmbutton_pressed(pressed, 1);
			dockapp_redraw();
		}
	}
	else if(s == MOUSE_RELEASED) {
		/* ignore releases if a button hasn't been pressed */
		if(!pressed)
			return;

		/* check visible icons */
		for(i = 0, p = icons; i < 6 && p != NULL; i++, p = p->next) {
			if(is_xpmbutton_at(p, x, y))
				released = p;
		}

		/* check scroll buttons */
		if(is_xpmbutton_at(sleft, x, y))
			released = sleft;
		if(is_xpmbutton_at(sright, x, y))
			released = sright;

		/* if the pressed button was released perform action */
		if(pressed == released) {

			/* scroll left */
			if(pressed == sleft) {
				scroll_left();
			}
			/* scroll right */
			else if(pressed == sright) {
				scroll_right();
			}

			/* execute command associated with button */
			else {
				if(pressed->command != NULL) {
					if(options->debugmode) {
						fprintf(stderr, "Execute command: %s\n", pressed->command);
					}
					else {
						system(pressed->command);
					}
				}
			}

		}

		/* unmark button pressed and request repaint */
		set_xpmbutton_pressed(pressed, 0);
		dockapp_redraw();

		/* clear pressed and released pointers */
		pressed = NULL;
		released = NULL;
	}
}

char * get_abspath_to_icon(char *filename, IconPath *iconpath) {
	char *abspath;
	int i;

	if(!filename || !iconpath)
		return (char *) NULL;


	/* check if filename is absolute path */
	if(fileexists(filename)) {

		/* allocate return value */
		abspath = (char *) malloc(sizeof(char) * (strlen(filename) + 1));
		if(!abspath) {
			fprintf(stderr, "wmappl.c: Couldn't allocate string.\n");
			exit(1);
		}

		/* copy the filename into the return value */
		strcpy(abspath, filename);

		return abspath;
	}

	abspath = (char *) malloc(iconpath->dirallocsize + (sizeof(char) * strlen(filename)));

	/* try each directory in the IconPath structure */
	for(i = 0; i < iconpath->count; i++) {

		/* copy the directory into the absolute path */
		strcpy(abspath, iconpath->path[i]);
		strcat(abspath, filename);

		/* check to see if filename exists at the absolute path */
		if(fileexists(abspath)) {

			/* reallocate return value to it's proper size */
			abspath = (char *) realloc(abspath, sizeof(char) * (strlen(abspath) + 1));
			if(!abspath) {
				fprintf(stderr, "wmappl.c: Couldn't reallocate string.\n");
				exit(1);
			}

			return abspath;
		}

	}

	return (char *) NULL;
}

char *default_rcfile() {
    char *s;
                
    /* check for getenv(HOME)/.wmapplrc */
	if(getenv("HOME")!=NULL) {
		s = (char *) malloc(sizeof(char) * (strlen((char *) getenv("HOME")) + 2 + strlen(WMAPPLRC)));
		if(s==NULL) {
			fprintf(stderr, "wmappl.c (error) : malloc()\n");
			exit(1);
		}
		strcpy(s, getenv("HOME"));
		if(s[strlen(s)-1]!='/') { 
			s[strlen(s)]='/';
			s[strlen(s)+1]='\0';
		}
    	strcat(s, WMAPPLRC);
		return s;
	}

	return NULL;

}


RCFile *read_rcfile(char *fn) {
	RCFile *rcfile = NULL;
	char *t;
	int i, j, l;

	if(fn!=NULL) {
		if(!fileexists(fn)) {
			fprintf(stderr, "wmappl could not find the rc file you specified: %s\n", fn);
			exit(1);
		}
		rcfile = parse_rcfile(fn);
	}
	else {
		t=default_rcfile();
		if(t!=NULL && fileexists(t)) {
			/* try HOME/WMAPPLRC */
			rcfile = parse_rcfile(t);
		}
		else if(fileexists(SYSWMAPPLRC)) {
			/* try SYSWMAPPLRC */
			rcfile = parse_rcfile(SYSWMAPPLRC);
		}
		if(t!=NULL) free(t);
	}

	if(rcfile==NULL) {
		fprintf(stderr, "wmappl could not find an rc file.\n\n");
		fprintf(stderr, "Either specify the rc filename on the command line, or use one of the default locations. Type 'wmappl --help' or 'man wmappl' for more information.\n");
		exit(1);
	}

	/* spin through rcfile looking for a wmappl section */
	for(i=0; i<rcfile->section_count; i++) {
		if(strcmp(rcfile->section[i]->section_name, "wmappl")==0) {
			for(j = 0; j < rcfile->section[i]->key_count; j++) {
				if(options->colonpath==NULL && strcmp(rcfile->section[i]->key[j]->key, "iconpath") == 0) {
					l=strlen(rcfile->section[i]->key[j]->value)+1;
					options->colonpath=(char *)malloc(sizeof(char)*l);
					strncpy(options->colonpath, rcfile->section[i]->key[j]->value, l);
				}
				if(options->name==NULL && strcmp(rcfile->section[i]->key[j]->key, "name") == 0) {
					l=strlen(rcfile->section[i]->key[j]->value)+1;
					options->name=(char *)malloc(sizeof(char)*l);
					strncpy(options->name, rcfile->section[i]->key[j]->value, l);
				}
				if(options->bgcolor==NULL && strcmp(rcfile->section[i]->key[j]->key, "background_color") == 0) {
					l=strlen(rcfile->section[i]->key[j]->value)+1;
					options->bgcolor=(char *)malloc(sizeof(char)*l);
					strncpy(options->bgcolor, rcfile->section[i]->key[j]->value, l);
				}
				if(options->bgicon==NULL && strcmp(rcfile->section[i]->key[j]->key, "background_icon") == 0) {
					l=strlen(rcfile->section[i]->key[j]->value)+1;
					options->bgicon=(char *)malloc(sizeof(char)*l);
					strncpy(options->bgicon, rcfile->section[i]->key[j]->value, l);
				}
				if(options->debugmode<0 && strcmp(rcfile->section[i]->key[j]->key, "debug") == 0) {
					options->debugmode=1;
				}
				if(options->withdrawn<0 && strcmp(rcfile->section[i]->key[j]->key, "window") == 0) {
					options->withdrawn=0;
				}
#ifdef USE_TOOLTIPS
				if(options->tooltipDelay<0 && strcmp(rcfile->section[i]->key[j]->key, "tooltip_delay") == 0) {
					options->tooltipDelay=atoi(rcfile->section[i]->key[j]->value);
				}
				if(options->tooltipfg==NULL && strcmp(rcfile->section[i]->key[j]->key, "tooltip_foreground") == 0) {
					l=strlen(rcfile->section[i]->key[j]->value)+1;
					options->tooltipfg=(char *)malloc(sizeof(char)*l);
					strncpy(options->tooltipfg, rcfile->section[i]->key[j]->value, l);
				}
				if(options->tooltipbg==NULL && strcmp(rcfile->section[i]->key[j]->key, "tooltip_background") == 0) {
					l=strlen(rcfile->section[i]->key[j]->value)+1;
					options->tooltipbg=(char *)malloc(sizeof(char)*l);
					strncpy(options->tooltipbg, rcfile->section[i]->key[j]->value, l);
				}
				if(options->tooltipfont==NULL && strcmp(rcfile->section[i]->key[j]->key, "tooltip_font") == 0) {
					l=strlen(rcfile->section[i]->key[j]->value)+1;
					options->tooltipfont=(char *)malloc(sizeof(char)*l);
					strncpy(options->tooltipfont, rcfile->section[i]->key[j]->value, l);
				}
#endif
			}
		}
	}
	return rcfile;
}

void load_icons(RCFile *rcfile, IconPath *iconpath) {
	int i, j;
	XpmButton *first = NULL;
	char *iconname = NULL, *command = NULL;

#ifdef USE_TOOLTIPS
	char *tooltip = NULL;
#endif

	/* load the scroll icons */
	sleft = create_include_xpmbutton(leftarr_xpm, dockapp_get_display());
	sright = create_include_xpmbutton(rightarr_xpm, dockapp_get_display());

	/* error out if scroll arrows couldn't be loaded */
	if(!sleft || !sright) {
		fprintf(stderr, "wmappl.c: Couldn't create scroll buttons.\n");
		exit(1);
	}

	/* set the initial positions of the scroll arrows */
	set_xpmbutton_position(sleft, scroll_positions[0], scroll_positions[1]);
	set_xpmbutton_position(sright, scroll_positions[2], scroll_positions[3]);

	/* traverse the structure, loading each icon */
	for(i = 0; i < rcfile->section_count; i++) {

		/* skip wmappl section if encountered */
		if(strcmp(rcfile->section[i]->section_name, "wmappl") == 0) {
			continue;
		}

		/* reset pointers */
		command = NULL;
		iconname = NULL;

#ifdef USE_TOOLTIPS
		tooltip = NULL;
#endif

		for(j = 0; j < rcfile->section[i]->key_count; j++) {
			if(strcmp(rcfile->section[i]->key[j]->key, "icon") == 0) {
				iconname = get_abspath_to_icon(rcfile->section[i]->key[j]->value, iconpath);
				if(!iconname) {
					fprintf(stderr, "wmappl.c: Couldn't find %s in the iconpath.\n", rcfile->section[i]->key[j]->value);
					exit(1);
				}
			}
			if(strcmp(rcfile->section[i]->key[j]->key, "command") == 0) {
				command = rcfile->section[i]->key[j]->value;
			}

#ifdef USE_TOOLTIPS
			if(strcmp(rcfile->section[i]->key[j]->key, "tooltip") == 0) {
				tooltip = rcfile->section[i]->key[j]->value;
			}
#endif
		}

		if(iconname && command) {
			if(!first) {
				/* create the xpmbutton */
				first = create_xpmbutton(iconname, dockapp_get_display());
				if(!first) {
					fprintf(stderr, "wmappl.c: Couldn't load icon %s.\n", iconname);
					exit(1);
				}

				/* set the command */
				set_xpmbutton_command(first, command);

#ifdef USE_TOOLTIPS
				/* set the tooltip */
				set_xpmbutton_tooltip(first, tooltip);
#endif

				/* warn if icon isn't 16x16 (18x18 counting added border) */
				if(first->width != 18 || first->height != 18) {
					fprintf(stderr, "wmappl.c: Warning, %s is not 16x16 and may cause undesired results.\n", iconname);
				}

				/* set the icons pointer to the first icon */
				icons = first;

			}
			else {
				/* create the xpmbutton */
				icons->next = create_xpmbutton(iconname, dockapp_get_display());
				if(!icons->next) {
					fprintf(stderr, "wmappl.c: Couldn't load icon %s.\n", iconname);
					exit(1);
				}

				/* set the command */
				set_xpmbutton_command(icons->next, command);

#ifdef USE_TOOLTIPS
				/* set the command */
				set_xpmbutton_tooltip(icons->next, tooltip);
#endif

				/* warn if icon isn't 16x16 (18x18 counting added border) */
				if(icons->next->width != 18 || icons->next->height != 18) {
					fprintf(stderr, "wmappl.c: Warning, %s is not 16x16 and may cause undesired results.\n", iconname);
				}

				/* set linked list pointers */
				icons->next->last = icons;
				icons = icons->next;
			}

			/* free allocated absolute icon filename */
			free(iconname);

		}
		else {
			if(iconname)
				fprintf(stderr, "wmappl.c: You must specify a command for the icon %s.\n", iconname);
			exit(1);
		}

	}

	/* set the icons pointer to the first icon */
	icons = first;

	/* set the button positions */
	set_button_positions();


}


int main(int argc, char **argv) {
	char *fn;
	RCFile *rcfile=NULL;
	IconPath *iconpath=NULL;;

	/* create options structure */
	options = opt_create_options();
	if(options==NULL) {
		fprintf(stderr, "wmappl.c: Could not allocate memory for options structure!\n");
		exit(1);
	}

	/* parse the command line arguments, to get the rcfile name if any */
	fn=parse_args(argc, argv, options);

	/* read the rcfile */
	rcfile=read_rcfile(fn);
	if(fn) free(fn);

	/* This is a kludge, but it does the trick! */
	/* re-parse the command line arguments, to override rcfile options */
	fn=parse_args(argc, argv, options);

	/* handle any unset parameters */
	if(options->withdrawn<0) options->withdrawn=1;
	if(options->debugmode<0) options->debugmode=0;
	if(options->name==NULL) {
		options->name=(char *)malloc(sizeof(char)*(strlen(PACKAGE)+1));
		strcpy(options->name, PACKAGE);
	}
	if(options->colonpath==NULL) {
		options->colonpath=(char *)malloc(sizeof(char)*(strlen(ICONPATH)+1));
		strcpy(options->colonpath, ICONPATH);
	}

	/* init display */
	if (dockapp_init_display() < 0) {
	  exit(1);
	}

	/* parse the icon path */
	iconpath = opt_create_iconpath(options->colonpath);

	/* load the icons */
	load_icons(rcfile, iconpath);

	/* free the RCFile structure */
	free_rcfile(rcfile);

	/* create the dockapp */
	if(!dockapp_create(options->name, "64x64+0+0", interface_xpm, options->withdrawn, argc, argv)) {
		fprintf(stderr, "Couldn't create dockapp.\n");
		exit(1);
	}

	/* set background icon */
	if(options->bgicon!=NULL) {
		fn = get_abspath_to_icon(options->bgicon, iconpath);
		if(fn==NULL) {
			fprintf(stderr, "Could not find background icon in iconpath: %s\n", options->bgicon);
			exit(1);
		}
		dockapp_set_background_pixmap_file(fn);
		free(fn);
	}
	else if(options->bgcolor!=NULL) {
		dockapp_set_background_color(options->bgcolor);
	}

	/* free the icon path */
	opt_free_iconpath(iconpath);

	/* set the paint and mouse callbacks */
	dockapp_set_paint(wmappl_paint);
	dockapp_set_mouse(wmappl_mouse);

#ifdef USE_TOOLTIPS
	dockapp_set_tooltip(wmappl_tooltip);
	if(options->tooltipfg!=NULL) {
		dockapp_set_tooltip_foreground(options->tooltipfg);
	}
	if(options->tooltipbg!=NULL) {
		dockapp_set_tooltip_background(options->tooltipbg);
	}
#endif

	/* enter the dockapp event loop */
	dockapp_run();

	return 0;
}
