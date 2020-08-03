#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "nasm.h"

FILE* get_file(void);
void write_to_file(char* t);

struct DataItem {
  char* name[50];
  char* value[1025];
};

int data_item_counter;
FILE* code_file;

struct DataItem** data_items;

void print(char* s) {
  char num[80];
  const char* data_name_prefix = "data";
  char* data_label;
  char* data_len_label;

  char* tmp;

  const char* ecx_inst = "mov ecx,";
  const char* edx_inst = "mov edx,";
  const char* len_suffix = "len";

  sprintf(num, "%d", data_item_counter++);

  data_label = (char*)malloc(sizeof(char*));
  sprintf(data_label, "%s%s", data_name_prefix, num);

  write_to_file("mov eax,4");
  write_to_file("mov ebx,1");
  
  tmp = (char*)malloc(sizeof(char*));
  sprintf(tmp, "%s%s", ecx_inst, data_label);
  write_to_file(tmp);
  free(tmp);

  sprintf(tmp, "%s%s%s", edx_inst, data_label, len_suffix);
  write_to_file(tmp);
  
  write_to_file("int 80h");
}

void set_code_file(void) {
  char template[10] = "codeXXXXXX";
  int fd;

  if (code_file==0) {
    printf("file was null.\n");
    fd = mkstemp(template);
    code_file = fdopen(fd, "w");
  }
}

void write_to_file(char* t) {
  set_code_file();

  fprintf(code_file, "%s\n", t);
}
