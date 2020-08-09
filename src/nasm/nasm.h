#ifndef NASM_H
#define NASM_H
void add_bss_item(char* name, char *keyword, char* value);
void add_data_item(char* name, char* value);
void declare_and_initialize_variable(char type, char* name, char* initial_value);
void declare_variable(char type, char* name);
void finalise(void);
char* join_two_strings(char* s1, char* s2);
void print(char* s);
void prog_exit(void);
void write_to_code_file(char* t);

extern int data_item_counter;
#endif
