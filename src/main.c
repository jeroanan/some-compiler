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

char* get_variable_name(char* s) {
  return strtok(s, " ");
}

char* get_variable_value(char* s) {
  if (!strtok(s, "=")) {
    return NULL;
  }

  return strtok(NULL, "=");
}

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
    comp_declare_variable(s, 'i', line_no);
  } else {
    msg = (char*)malloc(strlen(unknown_keyword)*sizeof(char*) + strlen(keyword)*sizeof(char*));
    sprintf(msg, "%s%s", unknown_keyword, keyword);
    comp_error_at_line(msg, line_no);
    free(msg);
  }
}
