#ifndef CSV_H
#define CSV_H
//hier in der Headerdatei sind typ und Funktionsdefintionen
#include <stddef.h>
#include <stdio.h>
//typedeff für charpointer als string;
typedef enum 
{
    Type_Empty,
    Type_Int,
    Type_Float,
    Type_String
} CellType;

typedef struct
{
    size_t rows, cols;
    char **header;
    char ***data;
    CellType *col_types;
    int has_header;
} csv_Table;
typedef FILE *CSVFILE;


//2* pointer ist Tabelle(rows*cols), und jede Zeile ist  char* (String)
void create_table(csv_Table *table);
void detect_col_types(csv_Table *table);
CellType detect_type(const char *value);
void csv_set_cell(csv_Table *table, size_t row, size_t col, const char *value);
void csv_print(csv_Table *table);
void csv_free(csv_Table *table);
void csv_edit_cell(csv_Table *table, size_t row, size_t col, const char *value);
void csv_copy_cell(csv_Table *table, size_t srcRow, size_t srcCol, size_t dstRow, size_t dstCol);
void csv_copy_row(csv_Table *table, size_t srcRow, size_t dstRow);
void csv_copy_col(csv_Table *table, size_t srcCol, size_t dstCol);
void csv_delete_row(csv_Table *table, size_t col);
void csv_delete_col(csv_Table *table, size_t col);
void csv_add_row(csv_Table *table);
void csv_add_col(csv_Table *table, const char *header_name);
int csv_import(csv_Table *table, FILE *file);
int csv_export(csv_Table *table, char *export_file_name);
void count_rows_cols(csv_Table *table, FILE *file);
FILE* open_file(char* path, char *rights);
char *my_tokenizer(char **token);
void csv_set_delimiter(char d);
void csv_set_quote(char q);
char *csv_get_cell_by_name(csv_Table *table, size_t row, const char *header);
int csv_find_col_index(csv_Table *table, const char *name);
void csv_edit_cell_by_name(csv_Table *table, size_t row, char* header, char* value);
void csv_write_cell_export(FILE *file, char *value);
int csv_cell_needs_quotes(char *value);

#endif
