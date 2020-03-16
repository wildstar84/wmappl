
/* args.c
 *
 * WMAppl - Window Maker Application Launcher
 * Copyright (C) 1999-2003 Casey Harkins
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

#include "args.h"

/* parse_args
 *  PARAMETERS:
 *      int argc          : number of command line parameters
 *      char **argv       : array of command line parameters
 *      wmappl_opt*       : wmappl_opt structure to store parameters in
 *  RETURN:
 *      char *            : returns rcfile name if specified on command line
 *                          or NULL if none was specified
 */
char *parse_args(int argc, char **argv, wmappl_opt *opt) {
	int c, slen;
	char *tmpiconpath = NULL;
	char *rcfile = NULL;

	if(opt==NULL) {
		fprintf(stderr, "args.c (error) : options structure was NULL!\n");
		exit(1);
	}

	while(1) {
		int             option_index = 0;

#ifdef HAVE_GETOPT_LONG
		/* setup long options structure */
		static struct option long_options[] = {
			{"rcfile", 1, 0, 'f'},
			{"iconpath", 1, 0, 'i'},
			{"window", 0, 0, 'w'},
			{"debug", 0, 0, 'd'},
			{"version", 0, 0, 'V'},
			{"help", 0, 0, 'h'},
			{0, 0, 0, 0}
		};

		c = getopt_long(argc, argv, "f:i:n:wdVh", long_options, &option_index);
#else
		c = getopt(argc, argv, "f:i:n:wdVh");
#endif

		if(c == -1)
			break;

		switch (c) {
			case 'f':
				if(rcfile) free(rcfile);
				rcfile = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
				if(!rcfile) {
					fprintf(stderr, "args.c (error) : malloc() rcfile\n");
					exit(1);
				}
				strcpy(rcfile, optarg);
				break;

			case 'i':
				if(opt->colonpath) free(opt->colonpath);
				opt->colonpath = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
				if(!opt->colonpath) {
					fprintf(stderr, "args.c (error) : malloc() opt->colonpath\n");
					exit(1);
				}
				strcpy(opt->colonpath, optarg);
				break;

			case 'n':
				if(opt->name) free(opt->name);
				opt->name = (char *) malloc(sizeof(char) * (strlen(optarg) + 1));
				if(!opt->name) {
					fprintf(stderr, "args.c (error) : malloc() opt->name\n");
					exit(1);
				}
				strcpy(opt->name, optarg);
				break;

			case 'w':
				opt->withdrawn=0;
				break;

			case 'd':
				opt->debugmode = 1;
				break;

			case 'V':
				show_version();
				break;

			case 'h':
				show_usage();
				break;

			default:
				show_usage();
				break;

		} /* end switch */

	} /* end while */

	return rcfile;

} /* end parse args */

/* show_version
 *  PARAMETERS:
 *      none
 *  RETURN:
 *      none
 */
void show_version()
{
	printf("\n%s version %s\n", PACKAGE, VERSION);
	printf("\n");
	printf("This is free software; you can redistribute it and/or\n");
 	printf("modify it under the terms of the GNU General Public\n");
  	printf("License as published by the Free Software Foundation; either\n");
  	printf("version 2 of the License, or (at your option) any later version.\n");
  	printf("\n");
  	printf("This software is distributed in the hope that it will be useful,\n");
  	printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
  	printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n");
  	printf("General Public License for more details.\n");
  	printf("\n");
  	printf("You should have received a copy of the GNU General Public\n");
  	printf("License along with this software; if not, write to the Free\n");
  	printf("Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n");
	printf("\n"); 
	printf("Copyright (c) 2000-2003 Casey Harkins (charkins@pobox.com)\n");
	printf("\n"); 
	printf("Contributors:\n");
	printf("    Denilson F. de Sá\n");
	printf("    Olivier Leconte\n");
	printf("    Richard Eisenman\n");
	printf("    Jacob Beardsley\n");
	printf("    Steve Akers\n");
	printf("    Samoylov Olleg\n");
	printf("    Jarek\n");
	printf("\n"); 
	printf("http://wmappl.sourceforge.net/\n");
	exit(1);
}								/* end show version */

/* show_usage
 *  PARAMETERS:
 *      none
 *  RETURN:
 *      none
 */
void show_usage()
{
	printf("\nUsage:\n");
	printf("  wmappl [OPTIONS]\n");
	printf("\n");

#ifdef HAVE_GETOPT_LONG
	printf("  -f, --rcfile [arg]  \tabsolute path of configuration file\n");
	printf("  -i, --iconpath [arg]\tpath to search for icons (paths separated by :'s)\n");
	printf("  -n, --name [arg]    \twindow name (specify different names to run multiple instances)\n");
	printf("  -w, --window        \twindowed mode (not withdrawn)\n");
	printf("  -d, --debug         \tdump debug information to stderr\n");
	printf("  -V, --version       \tshow version information\n");
	printf("  -h, --help          \tshow usage information (this info)\n");
#else
	printf("  -f [arg]  \tabsolute path of configuration file\n");
	printf("  -i [arg]  \tpath to search for icons (paths separated by :'s)\n");
	printf("  -n [arg]  \twindow name (specify different names to run multiple instances)\n");
	printf("  -w        \twindowed mode (not withdrawn)\n");
	printf("  -d        \tdump debug information to stderr\n");
	printf("  -V        \tshow version information\n");
	printf("  -h        \tshow usage information (this info)\n");
#endif

	printf("\n");

#ifdef USE_TOOLTIPS
	printf("  Tooltips enabled.\n");
#endif
	exit(1);
}								/* end show usage */
