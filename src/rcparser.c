
/* rcparser.c -- support functions for parsing rc files
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

#include "rcparser.h"

/* ----- TO DO -----
 * Error-handling : A system needs to be chosen for error handling.
 *                  Currently errors send a message to stderr and
 *                  either return from the function or call exit().
 * Comments       : Should comments be stored in the structures?
 *                  The advantage is the save_rcfile function could
 *                  write these comments along with the rest of the
 *                  data. The format of these comments would have to
 *                  be defined. Maybe *ONLY* comments on the end of
 *                  a Section or Key-Value line would be saved.
 */


/* ----- TEMPORARY MEMORY ALLOCTION -----
 *     The following macros define how memory is allocated
 *     during file parsing (parse_rcfile).
 *
 *     ALLOC_SECTION  : the number of RCSection pointers allocated for
 *                      an RCFile structure, if an RCFile contains more
 *                      sections than ALLOC_SECTION, another
 *                      ALLOC_SECTION pointers will be allocated
 *     ALLOC_KEYVALUE : the number of RCKeyValue pointers allocated for
 *                      an RCSection structure, if an RCSection contains
 *                      more key-value pairs than ALLOC_KEYVALUE,
 *                      another ALLOC_KEYVALUE pointers will be
 *                      allocated
 *
 *     ALLOC_LINE     : the number of characters allocated for each
 *                      line read, if the line contains more 
 *                      characters than ALLOC_LINE, another ALLOC_LINE
 *                      characters will be allocated
 *
 *     NOTE: When parse_rcfile returns an RCFile structure, that
 *           structure will only have memory allocated for its
 *           contents. The above macros only pertain to temporary
 *           memory used while parsing. When the parse_rcfile
 *           function is called, it calls realloc() to adjust
 *           the allocation size to the necessary sizes.
 *
 *     The following global variables track the multiplier for the
 *     above allocation sizes. When the ALLOC size from the macro
 *     is exceeded, the multiplier is incremented. Allocation
 *     size is determined by multiplying the ALLOC macro with the
 *     increment.
 *
 *     int mult_section  : the number of RCSection pointers used
 *     int mult_keyvalue : the number of RCKeyValue pointers used
 */

#define ALLOC_SECTION     32
#define ALLOC_KEYVALUE    16
#define ALLOC_LINE        1024

int             mult_section = 1;
int             mult_keyvalue = 1;

/* ----- INTERNAL GLOBAL VARIABLES ----- */

FILE           *filehandle;		/* file handle for opened rcfile */
int             end_of_file = 0;	/* set to 1 when eof is encountered */
int             file_open = 0;	/* set to 1 in is opened */

RCFile         *rcFile = NULL;
RCSection      *rcSection = NULL;
RCKeyValue     *rcKeyValue = NULL;


/* ----- USER FUNCTIONS ----- */

/* parse_rcfile
 *	PARAMETERS:
 *      char *filename    : name of rc file to parse
 *	RETURN:
 *      RCFile *          : returns RCFile structure for parsed file
 */
RCFile         *parse_rcfile(char *filename)
{
	char           *line, *name, *value;
	int             i, j, line_length;
	RCFile         *return_rcfile = NULL;

	/* open the file */
	open_file(filename, "r");
	if(!file_open)
		exit(1);

	/* create the RCFile structure */
	new_rcfile(filename);

	while(!end_of_file) {
		line = read_line();

		line_length = strlen(line);

		/* check to see if this line is a section */
		if(line_length > 0 && line[0] == '[' && line[line_length - 1] == ']') {

			/* allocate space for section name */
			name = (char *) malloc(sizeof(char) * (line_length - 1));
			if(!name) {
				fprintf(stderr, "rcparser.c: Couldn't allocate space for section name.\n");
				exit(1);
			}

			/* copy section name into allocated space */
			for(i = 0; i < line_length - 2; i++) {
				name[i] = line[i + 1];
			}

			/* terminate section name */
			name[i] = '\0';

			/* add section to RCFile */
			new_rcsection(name);

			/* free section name */
			free(name);
		}
		/* assume this is a key-value pair */
		else if(line_length > 0) {
			/* find first '=' character */
			for(i = 0; i < line_length && line[i] != '='; i++);

			if(i >= line_length) {
				fprintf(stderr, "rcparser.c: Expected Key=Value, invalid file format.\n");
				fprintf(stderr, "\t%s\n", line);
				exit(1);
			}

			/* allocate space for key name */
			name = (char *) malloc(sizeof(char) * (i + 1));
			if(!name) {
				fprintf(stderr, "rcparser.c: Couldn't allocate space for key name.\n");
				exit(1);
			}

			/* copy key name into allocated space */
			for(j = 0; j < i; j++) {
				name[j] = line[j];
			}

			/* terminate key name */
			name[j] = '\0';

			/* trim key name */
			name = trim_string(name);

			if(!name) {
				fprintf(stderr, "rcparser.c: Couldn't trim key name.\n");
				exit(1);
			}

			/* allocate space for key value */
			value = (char *) malloc(sizeof(char) * (line_length - i));
			if(!value) {
				fprintf(stderr, "rcparser.c: Couldn't allocate space for key value.\n");
				exit(1);
			}

			/* copy key value into allocated space */
			for(j = i + 1; j < line_length; j++) {
				value[j - i - 1] = line[j];
			}

			/* terminate key value */
			value[j - i - 1] = '\0';

			/* trim key value */
			value = trim_string(value);

			if(!value) {
				fprintf(stderr, "rcparser.c: Couldn't trim key value.\n");
				exit(1);
			}

			/* add key-value pair to RCSection */
			new_rckeyvalue(name, value);

			/* free key name and key value */
			free(name);
			free(value);

		}
	}

	close_file();

	return_rcfile = rcFile;

	/* clear global rcFile, rcSection and rcKeyValue structures */
	rcFile = NULL;
	rcSection = NULL;
	rcKeyValue = NULL;

	/* reallocate the RCFile structure before returning */
	realloc_rcfile(return_rcfile);

	return return_rcfile;

}

/* save_rcfile
 *	PARAMETERS:
 *      RCFile *rcfile    : RCFile structure to save
 *	RETURN:
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
void save_rcfile(RCFile *rcfile)
{
	int             i, j;

	if(!rcfile)
		return;

	/* open the file */
	open_file(rcfile->filename, "w");
	if(!file_open)
		exit(1);

	/* write a simple comment at the top */
	fprintf(filehandle, "# %s : This file automatically generated by rcparser\n", rcfile->filename);

	for(i = 0; i < rcfile->section_count; i++) {
		fprintf(filehandle, "\n[%s]\n", rcfile->section[i]->section_name);
		for(j = 0; j < rcfile->section[i]->key_count; j++)
			fprintf(filehandle, "%s = %s\n", rcfile->section[i]->key[j]->key, rcfile->section[i]->key[j]->value);
	}

	close_file();

}

/* free_rcfile
 *	PARAMETERS:
 *      RCFile *rcfile    : RCFile structure to free
 *	RETURN:
 *      none
 */
void free_rcfile(RCFile *rcfile)
{
	int             i, j;

	if(!rcFile)
		return;

	for(i = 0; i < rcfile->section_count; i++) {
		for(j = 0; j < rcfile->section[i]->key_count; j++) {
			free(rcfile->section[i]->key[j]->key);
			free(rcfile->section[i]->key[j]->value);
			free(rcfile->section[i]->key[j]);
		}
		free(rcfile->section[i]->section_name);
		free(rcfile->section[i]->key);
		free(rcfile->section[i]);
	}

	free(rcfile->section);
	free(rcfile->filename);
	free(rcfile);

	rcfile = NULL;

}

/* dump_rcfile
 *  PARAMETERS:
 *      RCFile *rcfile    : RCFile structure to dump to stderr
 *
 *  RETURN:
 *      none
 */
void dump_rcfile(RCFile *rcfile)
{
	int             i, j;

	fprintf(stderr, "Dumping RCFile structure ...\n");

	for(i = 0; i < rcfile->section_count; i++) {
		fprintf(stderr, "\tSection: %s\n", rcfile->section[i]->section_name);
		for(j = 0; j < rcfile->section[i]->key_count; j++) {
			fprintf(stderr, "\t\tKey: %s\t\t\tValue:%s\n", rcfile->section[i]->key[j]->key,
					rcfile->section[i]->key[j]->value);
		}
	}

}



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
char           *read_line()
{
	char           *line = NULL;
	char           *return_line = NULL;
	char           *this_line = NULL;

	int             c;
	int             char_count;
	int             inquote = 0;

	/* initialize multiplier to be used with ALLOC_LINE macro */
	int             mult_line = 1;

	/* zero char_count */
	char_count = 0;

	/* check to see if a file has been opened */
	if(!file_open) {
		fprintf(stderr, "rcparser.c: A file must be opened before read_line() may be called.\n");
		exit(1);
	}

	/* check to see that we have not already read EOF */
	if(end_of_file) {
		fprintf(stderr, "rcparser.c: EOF file has already been reached, cannot read_line().\n");
		return NULL;
	}

	/* allocate space for line */
	line = (char *) malloc(sizeof(char) * (ALLOC_LINE + 1));
	if(!line) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space for line buffer.\n");
		exit(1);
	}

	/* read in first character */
	c = fgetc(filehandle);

	/* read characters into line until '#', '\n', or EOF */
	while(c != EOF && c != '\n' && !(c == '#' && !inquote)) {

		/* track if we are in a quoted string */
		if(c == '\"') {
			if(inquote)
				inquote = 0;
			else
				inquote = 1;
		}

		/* grow line if needed */
		if(char_count >= ALLOC_LINE * mult_line) {
			mult_line++;
			line = (char *) realloc(line, sizeof(char) * ((ALLOC_LINE * mult_line) + 1));
			if(!line) {
				fprintf(stderr, "rcparser.c: Couldn't grow line buffer.\n");
				exit(1);
			}
		}

		/* add character to the line buffer */
		line[char_count] = c;

		/* increment character count */
		char_count++;

		/* read in next character */
		c = fgetc(filehandle);
	}

	if(c == EOF)
		end_of_file = 1;

	/* terminate string with NULL character */
	line[char_count] = '\0';

	/* if the last character read was a '#' read until '\n' or EOF */
	if((!end_of_file) && c == '#') {
		do {
			c = fgetc(filehandle);
		} while(c != EOF && c != '\n');
	}

	if(c == EOF)
		end_of_file = 1;

	return_line = trim_string(line);

	if(!return_line) {
		fprintf(stderr, "rcparser.c: Couldn't trim line from file.\n");
		exit(1);
	}

	free(line);

	/* if return_line ends in a '\' character, recurse and concatenate */
	if(return_line[strlen(return_line) - 1] == '\\') {

		/* change the '\' character to a '\0' */
		return_line[strlen(return_line) - 1] = '\0';

		/* do not recurse if we have already read the EOF */
		if(end_of_file)
			return return_line;

		/* save return_line as this_line */
		this_line = return_line;

		/* recurse */
		line = read_line();

		/* if recursion returned NULL, return without concatenating */
		if(!line)
			return return_line;

		/* allocate space for this line and the line returned from recusive call */
		return_line = (char *) malloc(sizeof(char) * (strlen(this_line) + strlen(line) + 1));
		if(!return_line) {
			fprintf(stderr, "rcparser.c: Couldn't allocate space for line buffer.\n");
			exit(1);
		}

		/* concatenate this line and the line returned from recursive call */
		strcpy(return_line, this_line);
		strcat(return_line, line);

		/* free all but the return_line */
		free(line);
		free(this_line);

	}

	return return_line;

}

/* open_file
 *	PARAMETERS:
 *      char *filename    : name of file to open
 *      char *mode        : the mode to open the file in 
 *                          (see fopen man page)
 *
 *  NOTE:
 *      If the file is opened successfully, the global in FILE
 *      handle will point to the opened file. The file_open
 *      variable will also be set to 1.
 */
void open_file(char *filename,
			   char *mode)
{
	if(file_open) {
		fprintf(stderr, "rcparser.c: A file has already been opened, call close_file() first.\n");
		return;
	}

	filehandle = fopen(filename, mode);
	if(!filehandle) {
		fprintf(stderr, "rcparser.c: Couldn't open %s using mode %s.\n", filename, mode);
		return;
	}

	file_open = 1;
	end_of_file = 0;

}

/* close_file
 *	PARAMETERS:
 *      char *filename    : name of file to open
 *
 *  NOTE:
 *     After this function is executed the FILE handle in will
 *     be closed and file_open will be set to 0. 
 */
void close_file()
{
	if(file_open) {
		fclose(filehandle);
		file_open = 0;
	}
}

/* new_rcfile
 *  PARAMETERS:
 *     char *filename : the name of the file associated with
 *                      the rcfile (used for saving)
 *  NOTE:
 *     Allocates a new RCFile structure and allocates 
 *     space for RCSection pointers
 */
void new_rcfile(char *filename)
{
	/* reset the allocation multipliers */
	mult_section = 1;
	mult_keyvalue = 1;

	/* allocate the new RCFile strucutre */
	rcFile = (RCFile *) malloc(sizeof(RCFile));
	if(!rcFile) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space for an RCFile structure.\n");
		exit(1);
	}

	/* allocate space for RCSection pointers */
	rcFile->section = (RCSection **) malloc(sizeof(RCSection *) * ALLOC_SECTION);
	if(!rcFile->section) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space in RCFile structure for sections.\n");
		exit(1);
	}

	/* allocate and store the filename */
	rcFile->filename = (char *) malloc(sizeof(char) * (strlen(filename) + 1));
	if(!rcFile->filename) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space in RCFile structure for the filename.\n");
		exit(1);
	}
	strcpy(rcFile->filename, filename);

	/* set section_count to zero */
	rcFile->section_count = 0;

}


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
void new_rcsection(char *name)
{
	int             section_index;

	/* if new_rcfile hasn't been called error out */
	if(!rcFile) {
		fprintf(stderr, "rcparser.c: You must call new_rcfile before calling new_rcsection.\n");
		exit(1);
	}

	/* check to see if rcFile.section needs to grow */
	if(rcFile->section_count >= ALLOC_SECTION * mult_section) {
		mult_section++;
		rcFile->section = (RCSection **) realloc(rcFile->section, sizeof(RCSection *) * (ALLOC_SECTION * mult_section));
		if(!rcFile->section) {
			fprintf(stderr, "rcparser.c: Couldn't grow section buffer.\n");
			exit(1);
		}
	}

	/* allocate a new RCSection */
	rcSection = (RCSection *) malloc(sizeof(RCSection));
	if(!rcSection) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space for an RCSection structure.\n");
		exit(1);
	}

	/* reset the allocation multiplier */
	mult_keyvalue = 1;

	/* allocate space for RCKeyValue pointers */
	rcSection->key = (RCKeyValue **) malloc(sizeof(RCKeyValue *) * ALLOC_KEYVALUE);
	if(!rcSection->key) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space in RCSection structure for keys.\n");
		exit(1);
	}

	/* allocate and store section name */
	rcSection->section_name = (char *) malloc(sizeof(char) * (strlen(name) + 1));
	if(!rcSection->section_name) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space in RCSection structure for the section name.\n");
		exit(1);
	}
	strcpy(rcSection->section_name, name);

	/* set key_count to zero */
	rcSection->key_count = 0;

	/* add the section to rcFile */
	section_index = rcFile->section_count;
	rcFile->section[section_index] = rcSection;
	rcFile->section_count++;

}


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
void new_rckeyvalue(char *name,
					char *value)
{
	/* if new_rcsection hasn't been called error out */
	if(!rcSection) {
		fprintf(stderr, "rcparser.c: An error occurred while reading rcfile.\n");
		fprintf(stderr, "rcparser.c: Key=Value pairs must follow a [SECTION] line.\n");
		exit(1);
	}

	/* check to see if rcSection.key needs to grow */
	if(rcSection->key_count >= ALLOC_KEYVALUE * mult_keyvalue) {
		mult_keyvalue++;
		rcSection->key =
				(RCKeyValue **) realloc(rcSection->key, sizeof(RCKeyValue *) * (ALLOC_KEYVALUE * mult_keyvalue));
		if(!rcSection->key) {
			fprintf(stderr, "rcparser.c: Couldn't grow key buffer for section [%s].\n", rcSection->section_name);
			exit(1);
		}
	}

	/* allocate a new RCKeyValue */
	rcKeyValue = (RCKeyValue *) malloc(sizeof(RCKeyValue));
	if(!rcKeyValue) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space for an RCKeyValue structure.\n");
		exit(1);
	}

	/* allocate and store key */
	rcKeyValue->key = (char *) malloc(sizeof(char) * (strlen(name) + 1));
	if(!rcKeyValue->key) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space in RCKeyValue structure for the key.\n");
		exit(1);
	}
	strcpy(rcKeyValue->key, name);

	/* allocate and store value */
	rcKeyValue->value = (char *) malloc(sizeof(char) * (strlen(value) + 1));
	if(!rcKeyValue->value) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space in RCKeyValue structure for the value.\n");
		exit(1);
	}
	strcpy(rcKeyValue->value, value);

	/* add the key-value pair to rcSection */
	rcSection->key[rcSection->key_count] = rcKeyValue;
	rcSection->key_count++;

}

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
void set_rcfile_filename(RCFile *rcfile,
						 char *filename)
{
	if(!rcfile)
		return;
	if(!filename)
		return;

	if(rcfile->filename)
		free(rcfile->filename);

	/* allocate and store the filename */
	rcfile->filename = (char *) malloc(sizeof(char) * (strlen(filename) + 1));
	if(!rcfile->filename) {
		fprintf(stderr, "rcparser.c: Couldn't allocate space in RCFile structure for the filename.\n");
		exit(1);
	}
	strcpy(rcfile->filename, filename);

	return;

}


/* is_whitespace
 *  PARAMETERS:
 *      char test_char    : the character to test
 *
 *  RETURN:
 *      int  : returns 1 if the test_char is considered whitespace,
 *             0 if it is not considered whitespace
 */
int is_whitespace(char test_char)
{
	switch (test_char) {
		/* cases for each whitespace character */
	case ' ':
	case '\t':
	case '\r':
		return 1;
		break;

		/* default case for non-whitespace characters */
	default:
		return 0;
		break;
	}
}

/* trim_string
 *  PARAMETERS:
 *      char *string     : the string to trim
 *
 *  RETURN:
 *      char * : returns the trimmed string (the original string
 *               is NOT freed)
 */
char           *trim_string(char *string)
{
	int             i, firstchar, lastchar, stringlen;
	char           *return_string;

	/* get the length of the string */
	stringlen = strlen(string);

	/* find the first non-whitespace character */
	for(i = 0; i < stringlen && (is_whitespace(string[i])); i++);

	/* store the index of the first non-whitespace character */
	if(i < stringlen)
		firstchar = i;
	/* if the entire string is whitespace return an empty string */
	else {
		return_string = (char *) malloc(sizeof(char));
		if(!return_string) {
			fprintf(stderr, "rcparser.c: Couldn't allocate memory for return_string.\n");
			exit(1);
		}
		return_string[0] = '\0';
		return return_string;
	}

	/* find the last non-whitespace character */
	for(i = stringlen - 1; i >= firstchar && (is_whitespace(string[i])); i--);

	/* store the index of the last non-whitespace character */
	lastchar = i;

	/* allocate space for the return_string */
	return_string = (char *) malloc(sizeof(char) * ((lastchar - firstchar) + 2));
	if(!return_string) {
		fprintf(stderr, "rcparser.c: Couldn't allocate memory for return_string.\n");
		exit(1);
	}

	/* fill the return string */
	for(i = firstchar; i <= lastchar; i++) {
		return_string[i - firstchar] = string[i];
	}

	/* add terminating null character */
	return_string[lastchar - firstchar + 1] = '\0';

	/* return the trimmed string */
	return return_string;

}

/* realloc_rcfile
 *  DESCRIPTION:
 *      This function crawls the RCFile structure and reallocates each array
 *      to the appropriate size. This frees temporary space allocated by the
 *      parsing functions.
 *
 *	PARAMETERS:
 *      RCFile *rcfile    : RCFile structure reallocate
 *
 *	RETURN:
 *      none
 */
void realloc_rcfile(RCFile *rcfile)
{
	int             i;

	if(rcfile == rcFile) {
		fprintf(stderr, "rcparser.c: realloc_rcfile() may not be called on an RCFile currently being parsed.\n");
		return;
	}

	/* reallocate rcfile.section */
	rcfile->section = (RCSection **) realloc(rcfile->section, sizeof(RCSection *) * (rcfile->section_count));
	if(!rcfile->section) {
		fprintf(stderr, "rcparser.c: Couldn't reallocate section array.\n");
		exit(1);
	}

	/* reallocate each section.key */
	for(i = 0; i < rcfile->section_count; i++) {
		rcfile->section[i]->key =
				(RCKeyValue **) realloc(rcfile->section[i]->key, sizeof(RCKeyValue *) * rcfile->section[i]->key_count);
		if(!rcfile->section[i]->key) {
			fprintf(stderr, "rcparser.c: Couldn't reallocate key array.\n");
			exit(1);
		}
	}
}
