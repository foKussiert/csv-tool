#ifndef CSV_H
#define CSV_H
//hier in der Headerdatei sind typ und Funktionsdefintionen
#include <stddef.h>
#include <stdio.h>
//typedeff für charpointer als string;
typedef struct
{
size_t rows, cols;
char **header;
char ***data;
} csv_Table;
typedef FILE *CSVFILE;



//2* pointer ist Tabelle(rows*cols), und jede Zeile ist  char* (String)

void create_table(csv_Table *table);
void csv_set_cell(csv_Table *table, size_t row, size_t col, const char *value);
void csv_print(csv_Table *table);
void csv_free(csv_Table *table);
int csv_import(csv_Table *table, FILE *file);
int csv_export(csv_Table *table, char *export_file_name);
void count_rows_cols(csv_Table *table, FILE *file);
FILE* open_file(char* path, char *rights);
#endif
