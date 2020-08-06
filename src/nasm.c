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

const char* data_file_name = "output/data.asm";
FILE* data_file;

const char* code_file_name = "output/code.asm";
FILE* code_file;

const char* final_file_name = "output/program.asm";
FILE* final_file;

struct DataItem** data_items;

void set_data_file(void) {
  if (!data_file) {
    data_file = fopen(data_file_name, "w");
  }
}

void add_data_item(char* name, char* value) {
  set_data_file();
  fprintf(data_file, "%s: %s\n", name, value);
}

char* join_two_strings(char* s1, char* s2) {
  char* tmp;
  tmp = (char*)malloc(strlen(s1)*sizeof(char*) + strlen(s2)*sizeof(char*));
  sprintf(tmp, "%s%s", s1, s2);

  return tmp;
}

void print(char* s) {
  char num[80];
  char* data_name_prefix = "data";
  char* data_label;
  char* data_len_label;

  char* tmp;
  char* tmp2;
  
  char* ecx_inst = "mov ecx,";
  char* edx_inst = "mov edx,";
  char* len_suffix = "len";

  tmp = (char*)malloc(sizeof(char*));
  tmp2 = (char*)malloc(sizeof(char*));

  sprintf(num, "%d", data_item_counter);
  data_label = join_two_strings(data_name_prefix, num);
  sprintf(tmp, "db '%s',10", s);
  add_data_item(data_label, tmp);
  free(tmp);

  write_to_code_file("mov eax,4");
  write_to_code_file("mov ebx,1");
  
  tmp = join_two_strings(ecx_inst, data_label);
  write_to_code_file(tmp);
  free(tmp);

  data_len_label = join_two_strings(data_label, len_suffix);
  sprintf(tmp2, "equ $-%s", data_label);

  add_data_item(data_len_label, tmp2);
  free(tmp2);

  ++data_item_counter;

  tmp = join_two_strings(edx_inst, data_len_label);
  write_to_code_file(tmp);
  free(tmp);

  free(data_label);
  free(data_len_label);
  
  write_to_code_file("int 80h");
}

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
  fprintf(final_file, "%s", t);
}

void finalise(void) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  char* data_section_header = "SECTION .DATA\n";
  char* text_section_header = "SECTION .TEXT\n";
  char* start_global = "GLOBAL _start\n";
  char* start_label = "_start:\n";

  fclose(code_file);
  fclose(data_file);

  write_to_final_file(data_section_header);

  data_file = fopen(data_file_name, "r");

  while ((read = getline(&line, &len, data_file)) != -1) {
    write_to_final_file(line);
  }

  fclose(data_file);

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

