#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

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
