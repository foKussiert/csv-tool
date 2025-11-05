#include "csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Zeile(row) Spalte (col)

//              Spalte col(0) Spalte col(1)
//Zeile(row)0
//Zeile(row)1
FILE* open_file(char *file_name, char *rights)
{
   FILE *file = fopen(file_name, rights);

   return file;
}
int csv_export(csv_Table *table, char *export_file_name)
{
    FILE *exportfile = fopen(export_file_name, "w");
    if (!exportfile) 
    {
        printf("Datei konnte nicht geoeffnet werden\n");
        return 1;
    }
    for (int j = 0; j < table->cols; j++)
    {
        if (table->header[j])
        {   
            fprintf(exportfile, "%s", table->header[j]);
        }
        if (j < table->cols - 1)
        {
            fprintf(exportfile, ",");
        }
    }
    fprintf(exportfile, "\n");
    for (int i = 0; i < table->rows; i++)
    {
        for (int j = 0; j < table->cols; j++)
        {
            if (table->data[i][j])
            {
                fprintf(exportfile, "%s", table->data[i][j]);
            }
            if (j < table->cols - 1)
            {
                fprintf(exportfile, ",");
            }
            
        }
        fprintf(exportfile, "\n");
    }
    fclose(exportfile);
    return 0;
}

void count_rows_cols(csv_Table *table, FILE *file)
{
    char buffer[1000];  
    size_t rows = 0;
    size_t max_cols = 0;

    while(fgets(buffer, sizeof(buffer), file))
    {
        size_t col_count = 0;
        char *token = strtok(buffer,",");
        while(token!= NULL)
        {                
            col_count++;    
            token = strtok(NULL,",");
        }
        if(col_count > max_cols)
        {
            max_cols = col_count;
        }
        rows++;
    }
    table->rows = rows;
    table->cols = max_cols;
    //muss gemacht werden weil der datei zeiger gerade am ende steht und am csv_import muss ja von anfang wieder beginnen
    rewind(file);
}
void create_table(csv_Table *table)
{

    //1 ebene, zeiger auf arrays(sind ja auch zeiger) muss ich aber
    table->data = malloc(sizeof(char**)*table->rows);
    
    for (int i = 0; i < table->rows; i++)
    {
        //reservierung von arrays also 2 ebene
        table->data[i]= malloc(sizeof(char*)*table->cols);
        for (int j = 0; j < table->cols;j++)
            {
                table->data[i][j] = NULL;
        
            }
    }
//header
    table->header = malloc(sizeof(char*)*table->cols);
    for (int z = 0; z < table->cols; z++)
    {
        table->header[z] = NULL;
    }
}

void csv_free(csv_Table *table)
{
    for (int k = 0; k < table->rows; k++)
    {
        for (int l = 0; l<table->cols;l++ )
        {
            free(table->data[k][l]);
        }
        free(table->data[k]);
    }
    free(table->data);
    
    for (int z = 0; z < table->cols; z++)
    {
        free(table->header[z]);
    }
    free(table->header);

}
void csv_print(csv_Table *table)
{
    for (int i = 0; i < table->rows; i++)
    {
        for (int j = 0; j < table->cols; j++)
        {
            if (table->data[i][j])
            {
                printf("%s", table->data[i][j]);
            }
            if (j < table->cols - 1) 
            {
            // tab zwischen den Spalten ausser am ende
                printf("\t"); 
            }
        }
        printf("\n");
    }
}
void csv_set_cell(csv_Table *table, size_t row, size_t col, const char* value)
{

    for (int i = 0; i < table->rows ;i++)
    {
        for (int j = 0; j < table->cols; j++)
            {
                //reservierung für die values also theoretisch 3 Ebene eig, strdup macht malloc(strlen() + 1);
                
                table->data[i][j] = strdup(value);
        
            }
    }
}
void csv_edit_cell(csv_Table *table, size_t row, size_t col, const char* value)
{
    table->data[row][col] = strdup(value);
}
void csv_copy_cell(csv_Table *table, size_t srcRow, size_t srcCol, size_t dstRow, size_t dstCol)
{
    //TODOhier hat strdup funktioniert, aber das muss ich mir nochmal anschauen, weil in meinem kopf eig doppelte speicherreservierung, aber vllt muss der es ja nochmal neu machen, so klappts auf jedenf all
    table->data[srcRow][srcCol] = strdup(table->data[dstRow][dstCol]);
}
void csv_copy_row(csv_Table *table, size_t row)
{

    for (int i = 0; i < table->cols ; i++)
    {
        
    }

}
void csv_copy_col(csv_Table *table, size_t col)
{

}
void csv_empty_cell(csv_Table *table, size_t row, size_t col)
{
    table->data[row][col] = NULL;
}
void csv_empty_row(csv_Table *table, size_t row)
{
    
}
void csv_empty_col(csv_Table *table, size_t col)
{

}
int csv_import(csv_Table *table, FILE *file)
{
    char buffer[1000];
    char *token;
    if (file == NULL)
    {
        printf("Datei konnte nicht geoeffnet werden");
        return 1;
    }
    int row = 0;
    int col = 0;
    if (!fgets(buffer, sizeof(buffer), file)) 
    {
        printf("Fehler beim Lesen der Header-Zeile oder leere Datei.\n");
        fclose(file);
        return 1;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
    token = strtok(buffer,","); 
    while(token!= NULL)
        {                
            //reservierung für die values also theoretisch 3 Ebene eig ist es das malloc(strlen() + 1); strcpy(...);
            table->header[col] = strdup(token);
            token = strtok(NULL,",");
            col++;          
        }
    while(fgets(buffer, sizeof(buffer), file))
    {
        col = 0;
        //Das \n durch \0 ersetzen, wichtig für den export dann -.-
        buffer[strcspn(buffer, "\n")] = '\0';
        token = strtok(buffer,",");

        while(token!= NULL)
        {                
            //reservierung für die values also theoretisch 3 Ebene eig ist es das malloc(strlen() + 1); strcpy(...);
            table->data[row][col] = strdup(token);
            token = strtok(NULL,",");
            col++;          
        }
        row++;

    }   
    fclose(file);
    return 0;
}
