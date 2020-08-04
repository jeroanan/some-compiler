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
      dispatch(trim_string(line), line_no);
    } 
  }
  prog_exit();

  print_data_items();
  finalise();
  fclose(f);  
}

char* get_keyword(char* s) {
  const int max_size = 10;

  int i;
  char kw[max_size];
  char* out;

  for (i=0; i<strlen(s) && i<max_size; i++) {
    if (!isspace((unsigned char)s[i])) {
      kw[i]=s[i];
    }
  }

  out = (char*)malloc(strlen(kw) * sizeof(char*));
  strcpy(out, kw);
  printf("%s\n", out);
  return out;
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

  keyword = get_keyword(s);

  const char *unknown_keyword = "Unknown keyword: ";

  if(strcmp(keyword, "print")==0) {
    s+=6;
    print(extract_string(s, line_no));
  } else {
    msg = (char*)malloc(strlen(unknown_keyword)*sizeof(char*) + strlen(keyword)*sizeof(char*));
    sprintf(msg, "%s%s", unknown_keyword, keyword);
    comp_error_at_line(msg, line_no);
    free(msg);
  }

  free(keyword);

}
