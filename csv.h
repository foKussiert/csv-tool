#ifndef CSV_H
#define CSV_H
//hier in der Headerdatei sind typ und Funktionsdefintionen
#include <stddef.h>
//typedeff für charpointer als string;
typedef struct
{
size_t rows, cols;
char ***data;
} csv_Table;



//2* pointer ist Tabelle(rows*cols), und jede Zeile ist  char* (String)

void create_table(csv_Table *table,size_t row, size_t col);
void csv_set_cell(csv_Table *table, size_t row, size_t col, const char *value);
void csv_print(csv_Table *table);
void csv_free(csv_Table *table);
int csv_import(csv_Table *table);

#endif