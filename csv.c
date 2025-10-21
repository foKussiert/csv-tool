#include "csv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create_table(csv_Table *table,size_t row,size_t col)
{
    //
    table->rows = row;
    table->cols = col;
    //1 ebene, zeiger auf arrays(sind ja auch zeiger) 
    table->data = malloc(sizeof(char**)*row);
    for (int i = 0; i <table->rows;i++)
    {
        //reservierung von arrays also 2 ebene
        table->data[i]= malloc(sizeof(char*)*table->cols);
        for (int j = 0; j<table->cols;j++)
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
    for (int i = 0; i <table->rows;i++)
    {
        for (int j = 0; j<table->cols;j++)
            {

                printf("Row %d, Col %d → %s\n", i, j, table->data[i][j]);
        
            }
    }
}
void csv_set_cell(csv_Table *table, size_t rows, size_t cols, const char* value)
{

    for (int i = 0; i <table->rows;i++)
    {
        for (int j = 0; j<table->cols;j++)
            {
                //reservierung für die values also theoretisch 3 Ebene eig ist es das malloc(strlen() + 1); strcpy(...);
                
                table->data[i][j] = strdup(value);
        
            }
    }
}
int csv_import(csv_Table *table)
{
    char buffer[1000];
    char *token;

    FILE *file = fopen("test_data_basic.csv", "r");
    if (file == NULL)
    {
        printf("Datei konnte nicht geoeffnet werden");
        return 1;
    }
    int row = 0;

    while(fgets(buffer, sizeof(buffer), file))
    {
        int col = 0;
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