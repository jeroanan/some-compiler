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
    dispatch(line, ++line_no);
  }
  prog_exit();

  finalise();
  fclose(f);  
}

void dispatch(char* s, int line_no) {
  char *t;
  char *msg;
  /* get first word. this will be our keyword for now. */
  t = strtok(s, " ");
  const char *unknown_keyword = "Unknown keyword: ";

  if(strcmp(t, "print")==0) {
    print("ah");
  } else {
    msg = (char*)malloc(strlen(unknown_keyword)*sizeof(char*) + strlen(t)*sizeof(char*));
    sprintf(msg, "%s%s", unknown_keyword, t);
    comp_error_at_line(msg, line_no);
    free(msg);
  }
}


