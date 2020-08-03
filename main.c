#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void comp_error(char* msg);
void comp_error_at_line(char* msg, int line_no);
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

  fclose(f);  
}

void dispatch(char* s, int line_no) {
  char *t;
  char *msg;
  /* get first word. this will be our keyword for now. */
  t = strtok(s, " ");
  const char *unknown_keyword = "Unknown keyword: ";

  if(strcmp(t, "print")==0) {
  } else {
    msg = (char*)malloc(strlen(unknown_keyword)*sizeof(char*) + strlen(t)*sizeof(char*));
    sprintf(msg, "%s%s", unknown_keyword, t);
    comp_error_at_line(msg, line_no);
    free(msg);
  }
}

void comp_error_at_line(char* msg, int line_no) {
  const char* m1 = "Error at line ";
  const char* m2 = ": ";
  char num[50];
  char* full_line;

  sprintf(num, "%d", line_no);
  full_line = (char*)malloc(
      strlen(m1)*sizeof(char*) + 
      strlen(num)*sizeof(char*) + 
      strlen(m2)*sizeof(char) + 
      strlen(msg));
  sprintf(full_line, "%s%s%s%s", m1, num, m2, msg);

  comp_error(full_line);
  free(full_line);
}

void comp_error(char* msg) {
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}
