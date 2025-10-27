#include "csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* open_file(char *file_name, char *rights)
{
   FILE *file = fopen(file_name, rights);

   return file;
}
void csv_export(csv_Table *table, char *export_file_name)
{
    FILE *exportfile = fopen(export_file_name, "w");
    if (!exportfile) 
    {
        printf("Datei konnte nicht geoeffnet werden\n");
        return;
    }
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
    //row könnte table->rows, noch testen
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
}

void csv_free(csv_Table *table)
{
    for (int k = 0; k<table->rows; k++)
    {
        for (int l = 0; l<table->cols;l++ )
        {
            free(table->data[k][l]);
        }
        free(table->data[k]);
    }
    free(table->data);

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

    while(fgets(buffer, sizeof(buffer), file))
    {
        int col = 0;
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