#include <stdio.h>
#include <stdlib.h>

#include "nasm.h"

#define DATA_NAME_PREFIX "data"
#define ECX_INST "mov ecx,"
#define EDX_INST "mov edx,"
#define LEN_SUFFIX "len"

extern int data_item_counter;

/*
 * Write instructions for printing the given string to std out.
 */
void print(char* s) {
  char num[80];
  char* data_label;
  char* data_len_label;

  char* tmp;
  char* tmp2;
  
  tmp = (char*)malloc(sizeof(char*));
  tmp2 = (char*)malloc(sizeof(char*));

  sprintf(num, "%d", data_item_counter);
  data_label = join_two_strings(DATA_NAME_PREFIX, num);
  sprintf(tmp, "db '%s',10", s);
  add_data_item(data_label, tmp);
  free(tmp);

  write_to_code_file("mov eax,4");
  write_to_code_file("mov ebx,1");
  
  tmp = join_two_strings(ECX_INST, data_label);
  write_to_code_file(tmp);
  free(tmp);

  data_len_label = join_two_strings(data_label, LEN_SUFFIX);
  sprintf(tmp2, "equ $-%s", data_label);

  add_data_item(data_len_label, tmp2);
  free(tmp2);

  ++data_item_counter;

  tmp = join_two_strings(EDX_INST, data_len_label);
  write_to_code_file(tmp);
  free(tmp);

  free(data_label);
  free(data_len_label);
  
  write_to_code_file("int 80h");
}

#undef DATA_NAME_PREFIX
#undef ECX_INST
#undef EDX_INST
#undef LEN_SUFFIX


