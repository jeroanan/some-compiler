#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "nasm.h"

FILE* get_file(void);
void write_to_code_file(char* t);

char code_file_template[15] = "/tmp/codeXXXXXX";

struct DataItem {
  char name[80];
  char value[1024];
};

int data_item_counter;
FILE* code_file;

struct DataItem** data_items;

void add_data_item(char* name, char* value) {
  struct DataItem* di;

  di = (struct DataItem*)malloc(sizeof(struct DataItem*));

  strcpy(di->name,  name);
  strcpy(di->value, value);

  if (data_item_counter==0) {
    data_items = (struct DataItem**)malloc(sizeof(struct DataItem));
  } else {
    data_items = (struct DataItem**)realloc(data_items, sizeof(data_items) + sizeof(struct DataItem));
  }

  data_items[data_item_counter] = di;
  data_item_counter++;
}

void print(char* s) {
  char num[80];
  const char* data_name_prefix = "data";
  char* data_label;
  char* data_len_label;

  char* tmp;
  char* tmp2;
  
  const char* ecx_inst = "mov ecx,";
  const char* edx_inst = "mov edx,";
  const char* len_suffix = "len";

  tmp = (char*)malloc(sizeof(char*));
  tmp2 = (char*)malloc(sizeof(char*));

  data_label = (char*)malloc(sizeof(char*));
  sprintf(data_label, "%s%s", data_name_prefix, num);
  sprintf(tmp, "db '%s',10", s);
  free(tmp);

  add_data_item(data_label, tmp);
  
  write_to_code_file("mov eax,4");
  write_to_code_file("mov ebx,1");
  
  tmp = (char*)malloc(strlen(ecx_inst)*sizeof(char*) + strlen(data_label)*sizeof(char*));
  sprintf(tmp, "%s%s", ecx_inst, data_label);
  write_to_code_file(tmp);
  free(tmp);

  data_len_label = (char*)malloc(strlen(data_label)*sizeof(char*) + strlen(len_suffix)*sizeof(char*));
  sprintf(data_len_label, "%s%s", data_label, len_suffix);
  sprintf(tmp2, "equ $-%s", data_label);

  add_data_item(data_len_label, tmp2);
  free(tmp2);

  tmp = (char*) malloc(strlen(edx_inst)*sizeof(char*) + strlen(data_len_label)*sizeof(char*));
  sprintf(tmp, "%s%s", edx_inst, data_len_label);
  write_to_code_file(tmp);
  free(tmp);

  free(data_label);
  free(data_len_label);
  
  write_to_code_file("int 80h");
}

void prog_exit(void) {
  write_to_code_file("mov eax,1");
  write_to_code_file("mov ebx,0");
  write_to_code_file("int 80h");
}

void set_code_file(void) {
  int fd;

  if (code_file==0) {
    fd = mkstemp(code_file_template);
    code_file = fdopen(fd, "w");
  }
}

void write_to_code_file(char* t) {
  set_code_file();

  fprintf(code_file, "%s\n", t);
}

void print_data_items(void) {
  int i;

  for (i=0;i<data_item_counter;i++) {
    printf("%d: name: %s, value: %s\n", i, data_items[i]->name, data_items[i]->value);
  }

  free(data_items);
}
