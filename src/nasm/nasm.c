#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "nasm.h"
#include "../stringfunc.h"

FILE* get_file(void);
void write_to_code_file(char* t);
void write_data_items(void);

struct DataItem {
  char* name;
  char* value;
};

int data_item_counter;
int data_item_members;

struct BssItem {
  char* name;
  char* keyword;
  char* value;
};

const char* code_file_name = "output/code.asm";
FILE* code_file;

const char* final_file_name = "output/program.asm";
FILE* final_file;

struct DataItem* data_items;

struct BssItem* bss_items;
int bss_counter=0;

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

void add_bss_item(char* name, char *keyword, char* value) {
  struct BssItem bi;

  if (!bss_counter) {
    bss_items = (struct BssItem*)malloc(sizeof(struct BssItem));
  } else {
    bss_items = (struct BssItem*)realloc(bss_items, (bss_counter+1) * sizeof (struct BssItem));
  }

  bi.name = (char*)malloc(strlen(name) * sizeof(char*));
  bi.keyword = (char*)malloc(strlen(keyword) * sizeof(char*));
  bi.value = (char*)malloc(strlen(value) * sizeof(char*));

  strcpy(bi.name, trim_string(name));
  strcpy(bi.keyword, keyword);
  strcpy(bi.value, value);
  
  bss_items[bss_counter] = bi;

  printf("name: %s, keyword: %s, value: %s\n", bi.name, bi.keyword, bi.value);
  ++bss_counter;
}

char* join_two_strings(char* s1, char* s2) {
  char* tmp;
  tmp = (char*)malloc(strlen(s1) * sizeof(char*) + strlen(s2) * sizeof(char*));
  sprintf(tmp, "%s%s", s1, s2);

  return tmp;
}

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
#define BSS_SECTION_HEADER "SECTION .BSS\n"
#define TEXT_SECTION_HEADER  "SECTION .TEXT\n"
#define START_LABEL "_start:\n"
#define START_GLOBAL "GLOBAL _start\n"

void finalise(void) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int i;
  char* data_line; // TODO: Rename

  const char* separator = ": ";
  const char* space = " ";

  fclose(code_file);

  write_to_final_file(BSS_SECTION_HEADER);
  for(i=0;i<bss_counter;i++) {
    data_line = (char*)malloc(
        strlen(bss_items[i].name) * sizeof(char*) +
        strlen(separator) * sizeof(char*) +
        strlen(bss_items[i].keyword) * sizeof(char*) +
        strlen(space) * sizeof(char*) +
        strlen(bss_items[i].value) * sizeof(char*));

    sprintf(data_line, "%s%s%s%s%s\n", 
        bss_items[i].name, 
        separator, 
        bss_items[i].keyword, 
        space, 
        bss_items[i].value);

    write_to_final_file(data_line);
  }

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

