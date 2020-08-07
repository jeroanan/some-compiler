#ifndef NASM_H
#define NASM_H
void print(char* s);
void declare_variable(char type, char* name);
void declare_and_initialize_variable(char type, char* name, char* initial_value);

void print_data_items(void);
void prog_exit(void);
void finalise(void);
#endif
