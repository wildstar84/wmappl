/* rcparser.h -- support functions for parsing rc files
 *
 * Copyright (C) 2002 Casey Harkins (charkins@pobox.com)
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
#ifndef __RCPARSER_H__
#define __RCPARSER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- RCFile, RCSection and RCKeyValue structures ----- */
typedef struct _RCKeyValue_ {
    char *key;
    char *value;
} RCKeyValue;

typedef struct _RCSection_ {
    char *section_name;
    int key_count;
    RCKeyValue **key;
} RCSection;

typedef struct _RCFile_ {
    char *filename;
    int section_count;
    RCSection **section;
} RCFile;


/* ----- USER FUNCTIONS ----- */

/* parse_rcfile
 *	PARAMETERS:
 *      char *filename    : name of rc file to parse
 *	RETURN:
 *      RCFile *          : returns RCFile structure for parsed file
 */
RCFile*
parse_rcfile(char *filename);

/* save_rcfile
 *  PARAMETERS:
 *      RCFile *rcfile    : RCFile structure to save
 *  RETURN:
 *      none
 *
 *  NOTES:
 *      This function will save an rcfile to the filename stored
 *      within the RCFile structure. If the file already exists
 *      it will be overwritten. This function does saves only a
 *      single comment at the beginning of the file. If the 
 *      RCFile structure was loaded from a file using parse_rcfile,
 *      then any comments in the original file will be lost.
 */
void
save_rcfile(RCFile *rcfile);

/* free_rcfile
 *	PARAMETERS:
 *      RCFile *rcfile    : RCFile structure to free
 *	RETURN:
 *      none
 */
void
free_rcfile(RCFile *rcfile);

/* dump_rcfile
 *  PARAMETERS:
 *      RCFile *rcfile    : RCFile structure to dump to stderr
 *
 *  RETURN:
 *      none
 */
void
dump_rcfile(RCFile *rcfile);


/* ----- INTERNAL FUNCTIONS ----- */

/* read_line
 *  PREREQUISITES:
 *      A file must have first been opened using the open_file
 *      function.
 *
 *	RETURN:
 *      char *            : returns next line from file (see note)
 *
 *  NOTE:
 *      The read_line function will remove whitespace from each end
 *      of the line read. It will also ignore anything from a '#'
 *      character through the end of a line. If a line, after
 *      removing whitespace from both ends, ends in a '\' character
 *      then the following line will be read in the same manner and
 *      will be appended to the first_line, overwriting the '\'
 *      character. This is acheived through a recursive call to
 *      read_line, therefore if the following line ends in a '\'
 *      it too will have it's following line appended. 
 */
char*
read_line();

/* open_file
 *	PARAMETERS:
 *      char *filename    : name of file to open
 *
 *  NOTE:
 *      If the file is opened successfully, the global in FILE
 *      handle will point to the opened file. The file_open
 *      variable will also be set to 1.
 */
void
open_file(char *filename, char *mode);

/* close_file
 *	PARAMETERS:
 *      char *filename    : name of file to open
 *      char *mode        : the mode to open the file in
 *                          (see the fopen man page)
 *
 *  NOTE:
 *     After this function is executed the FILE handle in will
 *     be closed and file_open will be set to 0. 
 */
void
close_file();

/* new_rcfile
 *  PARAMETERS:
 *     char *filename : the name of the file associated with
 *                      the rcfile (used for saving)
 *
 *  NOTE:
 *     Allocates a new RCFile structure and allocates
 *     space for RCSection pointers
 */
void
new_rcfile(char *filename);

/* new_rcsection
 *  PREREQUISITES:
 *     The new_rcfile function must have been called to create
 *     the working RCFile structure
 *
 *  PARAMETERS:
 *     char *name : the name of the section to be added
 *
 *  NOTE:
 *     Allocates a new RCSection structure and allocates
 *     space for RCKeyValue pointers, the RCSection pointer is
 *     added to the RCFile structure
 */
void
new_rcsection(char *name);

/* new_rckeyvalue
 *  PREREQUISITES:
 *     The new_rcsection function must have been called to create
 *     the working RCSection structure
 *
 *  PARAMETERS:
 *     char *name  : the name of the key to be added
 *     char *value : the value to be associated with the key
 *
 *  NOTE:
 *     Allocates a new RCKeyValue structure, the RCKeyValue pointer
 *     is added to the RCSection structure
 */
void
new_rckeyvalue(char *name, char *value);

/* set_rcfile_filename
 *  PARAMETERS:
 *      RCFile *rcfile    : the RCFile structure to set the filename for
 *      char *filename    : the new filename for the RCFile structure
 *
 *  RETURN:
 *      none
 *
 *  NOTES:
 *      This function free's the old filename from the structure and
 *      allocates and stores the new filename in the structure.
 */
void
set_rcfile_filename(RCFile *rcfile, char *filename);

/* is_whitespace
 *  PARAMETERS:
 *      char test_char    : the character to test
 *
 *  RETURN:
 *      int  : returns 1 if the test_char is considered whitespace,
 *             0 if it is not considered whitespace
 */
int
is_whitespace(char test_char);

/* trim_string
 *  PARAMETERS:
 *      char *string     : the string to trim
 *
 *  RETURN:
 *      char * : returns the trimmed string (the original string
 *               is NOT freed)
 */
char*
trim_string(char *string);

/* realloc_rcfile
 *  DESCRIPTION:
 *      This function crawls the RCFile structure and reallocates each array
 *      to the appropriate size. This frees temporary space allocated by the
 *      parsing functions.
 *
 *  PARAMETERS:
 *      RCFile *rcfile    : RCFile structure reallocate
 *
 *  RETURN:
 *      none
 */
void
realloc_rcfile(RCFile *rcfile);


#endif /* not __RCPARSER_H__ */
