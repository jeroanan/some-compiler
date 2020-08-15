#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comp.h"
#include "error.h"
#include "stringfunc.h"

#include "nasm/nasm.h"

void compile_file(char* filename);
void dispatch(char* s, int line_no);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    comp_error("Expected filename");
  }

  compile_file(argv[1]);

  exit(EXIT_SUCCESS);
}

/*
 * Are all characters in a string spaces?
 */
bool string_is_empty(char* s) {

  for (;*s;s++) {
    if (!isspace((unsigned char)*s)) {
      return false;
    }
  }

  return true;
}

/* 
 * truncate a string a the point of a comment character '#'
 * unless the '#' is within a single-quoted string
 */
char* remove_comments(char* s) {
  int i;
  bool in_string = false;

  for (i=0;i<strlen(s);i++) {
    if (s[i]=='#' && !in_string) {
      s[i] = '\0';
      break;
    } else if (s[i]=='\'') {
      in_string = !in_string;
    }
  }

  return s;
}

/*
 * Go line-by-line throuhg a file, trim it, remove comments and dispatch it to
 * the relevant function
 */
void compile_file(char* filename) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  FILE *f;
  int line_no;

  f = fopen(filename, "r");

  if (f == NULL) {
    comp_error("Could not open file");
  }

  while ((read = getline(&line, &len, f)) != -1) {
    ++line_no;
    if (!string_is_empty(line)) {
      dispatch(
          trim_string(
            remove_comments(line)), 
          line_no);
    } 
  }
  prog_exit();

  finalise();
  fclose(f);  
}

/*
 * Get the zero-based index of the specified char in the specified string
 */
int get_location_of_character(char* s, char c) {
 
  int i;

  for (i=0; *s!='\0'; i++, s++) {
    if (*s == c) {
      return i;
    }
  }

  return -1;
}

/*
 * Get the first word in a string
 */
char* get_first_word(char* s) {
  int l;

  if ((l = get_location_of_character(s, ' ')) > -1) {
    s[l] = '\0';
  }

  return s;
}

/*
 * Get the contents of a single-quoted string
 */
char* extract_string(char* s, int line_no) {
  if (*s!='\'') {
    comp_error_at_line("Missing opening '", line_no);
  }

  s++;

  if (s[strlen(s)-1]!='\'') {
    comp_error_at_line("Missing closing '", line_no);
  }

  s[strlen(s)-1] = '\0';

  return s;
}

/*
 * Send the given line to the relevant function for compilation
 */
void dispatch(char* s, int line_no) {
  char *msg;
  char* keyword;
  char* line_copy;

  if (!strlen(s)) {
    return;
  }

  line_copy = (char*)malloc(strlen(s) * sizeof(char*));
  strcpy(line_copy, s);

  keyword = get_first_word(s);

  const char *unknown_keyword = "Unknown keyword: ";

  if(strcmp(keyword, "print")==0) {
    s+=6;
    print(extract_string(s, line_no));
  } else if (strcmp(keyword, "int")==0) {
    s+=4;
    comp_declare_variable(s, 'i', line_no);
  } else {
    msg = (char*)malloc(
        strlen(unknown_keyword)*sizeof(char*) + 
        strlen(keyword)*sizeof(char*));

    sprintf(msg, "%s%s", unknown_keyword, keyword);
    comp_error_at_line(msg, line_no);
    free(msg);
  }

  free(line_copy);
}
