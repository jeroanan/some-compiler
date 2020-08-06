#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "nasm.h"

FILE* get_file(void);
void write_to_code_file(char* t);
void write_data_items(void);

struct DataItem {
  char* name;
  char* value;
};

int data_item_counter;
int data_item_members;

const char* code_file_name = "output/code.asm";
FILE* code_file;

const char* final_file_name = "output/program.asm";
FILE* final_file;

struct DataItem* data_items;

void add_data_item(char* name, char* value) {

  struct DataItem di; 

  if (data_item_members == 0) {
    data_items = (struct DataItem*)malloc(sizeof (struct DataItem) * 1);
  } else {
    data_items = (struct DataItem*)realloc(data_items, (data_item_members+1) * sizeof (struct DataItem));
  }
  
  di.name = (char*)malloc(strlen(name) * sizeof(char*));
  di.value = (char*)malloc(strlen(value) * sizeof(char*));
  strcpy(di.name, name);
  strcpy(di.value, value);
 
  data_items[data_item_members] = di;
  ++data_item_members;
}

char* join_two_strings(char* s1, char* s2) {
  char* tmp;
  tmp = (char*)malloc(strlen(s1)*sizeof(char*) + strlen(s2)*sizeof(char*));
  sprintf(tmp, "%s%s", s1, s2);

  return tmp;
}

#define DATA_NAME_PREFIX "data"
#define ECX_INST "mov ecx,"
#define EDX_INST "mov edx,"
#define LEN_SUFFIX "len"

void print(char* s) {
  char num[80];
  /*
  char* data_name_prefix = "data";
  */
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

void declare_variable(char type, char* name) {

  char* nasm_type;
  char* initial_value;
  char value_string[80];

  switch (type) {
    case 'i':
      nasm_type = "DW";
      initial_value = "0";
      break;
    default:
      fprintf(stderr, "Unknown type: %s", type);
      exit(EXIT_FAILURE);
  }

  sprintf(value_string, "%s %s", nasm_type, initial_value);
  add_data_item(name, value_string);

}

void prog_exit(void) {
  write_to_code_file("mov eax,1");
  write_to_code_file("mov ebx,0");
  write_to_code_file("int 80h");
}

void write_to_code_file(char* t) {
  if (!code_file) {
    code_file = fopen(code_file_name, "w");
  }

  fprintf(code_file, "%s\n", t);

#ifdef NASM_DEBUG
  printf("%s\n", t);
#endif
}

void write_to_final_file(char* t) {
  if (!final_file) {
    final_file = fopen(final_file_name, "w");
  }

  fprintf(final_file, "%s", t);
}

#define DATA_SECTION_HEADER "SECTION .DATA\n"
#define TEXT_SECTION_HEADER  "SECTION .TEXT\n"
#define START_LABEL "_start:\n"
#define START_GLOBAL "GLOBAL _start\n"

void finalise(void) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int i;
  char* data_line;

  const char* separator = ": ";

  fclose(code_file);

  write_to_final_file(DATA_SECTION_HEADER);

  for (i=0;i<data_item_members;i++) {
    data_line = (char*)malloc(
        strlen(data_items[i].name)*sizeof(char) +
        strlen(separator)*sizeof(char) +
        strlen(data_items[i].value)*sizeof(char));

    sprintf(data_line, "%s%s%s\n", data_items[i].name, separator, data_items[i].value);
    write_to_final_file(data_line);

    free(data_line);
  }

  write_to_final_file(TEXT_SECTION_HEADER);
  write_to_final_file(START_GLOBAL);
  write_to_final_file(START_LABEL);

  code_file = fopen(code_file_name, "r");

  while ((read = getline(&line, &len, code_file)) != -1) {
    write_to_final_file(line);
  }

  fclose(code_file);
  fclose(final_file);

  if (data_item_counter>0) {
    free(data_items);
  }
}

#undef DATA_SECTION_HEADER
#undef TEXT_SECTION_HEADER
#undef START_LABEL
#undef START_GLOBAL

