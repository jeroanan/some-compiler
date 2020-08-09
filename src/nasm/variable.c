#include <stdio.h>
#include <stdlib.h>

#include "nasm.h"
#include "../stringfunc.h"

void declare_and_initialize_variable(char type, char* name, char* initial_value) {
  char* nasm_type;
  char value_string[80];

  switch (type) {
    case 'i':
      nasm_type = "DW";
      break;
    default:
      fprintf(stderr, "Unknown type: %s", type);
      exit(EXIT_FAILURE);
  }

  sprintf(value_string, "%s", initial_value);
  if (initial_value == "0") {
    add_bss_item(name, "resd", value_string);
  } else {
    sprintf(value_string, "%s %s", nasm_type, initial_value);
    add_data_item(trim_string(name), value_string);
  }
}


void declare_variable(char type, char* name) {
  declare_and_initialize_variable(type, name, "0");
}
