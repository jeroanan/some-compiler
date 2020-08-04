#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#include "nasm.h"

FILE* get_file(void);
void write_to_code_file(char* t);
void write_data_items(void);

struct DataItem {
  char name[80];
  char value[1024];
};

int data_item_counter;

const char* code_file_name = "code.asm";
FILE* code_file;

const char* final_file_name = "program.asm";
FILE* final_file;

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

  add_data_item(data_label, tmp);
  free(tmp);

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
  if (!code_file) {
    code_file = fopen(code_file_name, "w");
  }
}

void write_to_code_file(char* t) {
  set_code_file();

  fprintf(code_file, "%s\n", t);

#ifdef NASM_DEBUG
  printf("%s\n", t);
#endif
}

void set_final_file(void) {
  if (!final_file) {
    final_file = fopen(final_file_name, "w");
  }
}

void write_to_final_file(char* t) {
  set_final_file();
  fprintf(final_file, "%s\n", t);
}

void finalise(void) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char* data_section_header = "SECTION .DATA";
  char* text_section_header = "SECTION .TEXT";
  char* start_global = "GLOBAL _start";
  char* start_label = "_start:";

  fclose(code_file);

  write_to_final_file(data_section_header);
  write_data_items();
  write_to_final_file(text_section_header);
  write_to_final_file(start_global);
  write_to_final_file(start_label);

  code_file = fopen(code_file_name, "r");

  while ((read = getline(&line, &len, code_file)) != -1) {
    write_to_final_file(line);
  }

  fclose(code_file);

  fclose(final_file);
}

void write_data_items(void) {
  int i;
  char* to_write;
  const char* separator = ": ";

  for (i=0;i<data_item_counter;i++) {
    to_write = (char*)malloc(
        strlen(data_items[i]->name)*sizeof(char*) +
        strlen(separator)*sizeof(char*) +
        strlen(data_items[i]->value)*sizeof(char*));

    sprintf(to_write, "%s%s%s", data_items[i]->name, separator, data_items[i]->value);

    write_to_final_file(to_write);
    free(to_write);
  }
}

void print_data_items(void) {
#ifdef NASM_DEBUG
  int i;

  for (i=0;i<data_item_counter;i++) {
    printf("%d: name: %s, value: %s\n", i, data_items[i]->name, data_items[i]->value);
  }

#endif
}
