#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

/*
 * Emit a compiler error for a source code line no. with the given error 
 * message
 */
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

/*
 * Emit the given error message and terminate
 */
void comp_error(char* msg) {
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}
