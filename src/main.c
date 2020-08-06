#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "error.h"
#include "nasm.h"

void compile_file(char* filename);
void dispatch(char* s, int line_no);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    comp_error("Expected filename");
  }

  compile_file(argv[1]);

  exit(EXIT_SUCCESS);
}

bool string_is_empty(char* s) {

  while (*s) {
    if (!isspace((unsigned char)*s)) {
      return false;
    }
    s++;
  }

  return true;
}

char* trim_string(char* s) {

  int i;

  while(*s) {
    if (!isspace((unsigned char)*s)) {
      break;
    }

    s++;
  }

  for (i=strlen(s)-1;i>0;i--) {
    if (isspace((unsigned char)s[i])) {
      s[i] = '\0';
    } else {
      break;
    }
  }

  return s;
}

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

int get_location_of_character(char* s, char c) {
 
  int i;

  for (i=0; *s!='\0'; i++, s++) {
    if (*s == c) {
      return i;
    }
  }

  return -1;
}

char* get_first_word(char* s) {
  int l;

  if ((l = get_location_of_character(s, ' ')) > -1) {
    s[l] = '\0';
  }

  return s;
}

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

void dispatch(char* s, int line_no) {
  char *msg;
  char* keyword;

  if (!strlen(s)) {
    return;
  }

  keyword = get_first_word(s);

  const char *unknown_keyword = "Unknown keyword: ";

  if(strcmp(keyword, "print")==0) {
    s+=6;
    print(extract_string(s, line_no));
  } else if (strcmp(keyword, "int")==0) {
    s+=4;
    declare_variable('i', s);
  } else {
    msg = (char*)malloc(strlen(unknown_keyword)*sizeof(char*) + strlen(keyword)*sizeof(char*));
    sprintf(msg, "%s%s", unknown_keyword, keyword);
    comp_error_at_line(msg, line_no);
    free(msg);
  }
}
