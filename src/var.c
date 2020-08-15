#include <stdlib.h>
#include <string.h>

#include "comp.h"
#include "error.h"
#include "stringfunc.h"
#include "nasm/nasm.h"

/* 
 * function prototypes
 */
int get_number_of_words(char* s);
char* get_variable_name(char* s);
char* get_variable_value(char* s);

/*
 * Handle compilation of a variable declaration
 */
void comp_declare_variable(char* s, char type, int line_no) {
#define WORDS_DECLARATION_ONLY 1
#define WORDS_DECLARE_INITIALIZE 3

  char* name;
  char* value=NULL;
  char* tmpS = (char*)malloc(strlen(s) * sizeof(char*));
  int number_of_words = get_number_of_words(s);

  strcpy(tmpS, s);

  if (number_of_words != WORDS_DECLARATION_ONLY && 
      number_of_words != WORDS_DECLARE_INITIALIZE) {
    comp_error_at_line("Syntax error", line_no);
  }

  if ((name = get_variable_name(tmpS)) == NULL) {
    comp_error_at_line("Could not get variable name", line_no);
  } else {
    name = trim_string(name);
  }
  
  if ((number_of_words == WORDS_DECLARE_INITIALIZE)) {

    strcpy(tmpS, s);
    if (!(value = get_variable_value(tmpS))) {
      comp_error_at_line("Could not get variable value", line_no);
    } else {
      value = trim_string(value);
    }
  }

  if (type!='i') {
      comp_error_at_line("Unrecognised variable type", line_no);
  } else {
    if (!value)
      declare_variable('i', name);
    else
      declare_and_initialize_variable('i', name, value);
  }

  free(tmpS);

#undef WORDS_DECLARATION_ONLY
#undef WORDS_DECLARE_INITIALIZE
}

/*
 * Get the number of words in the given string
 */
int get_number_of_words(char* s) {
  int c = 0;
  char* tmp = (char*)malloc(strlen(s) * sizeof(char*));
  strcpy(tmp,s);

  if (strtok(tmp, " ")) 
    c++;

  for (;strtok(NULL, " "); c++) 
   ;

  free(tmp);

  return c;
}

/*
 * Get the name of the variable being defined in the given string
 */
char* get_variable_name(char* s) {
  return strtok(s, " ");
}

/*
 * Get the value of the variable being defined in the given string
 */
char* get_variable_value(char* s) {
  if (!strtok(s, "=")) {
    return NULL;
  }

  return strtok(NULL, "=");
}

