    #include "csv.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
        //table[row][col]
        //Zeile(row) Spalte (col)
        //           Spalte col(0) Spalte col(1)
        //Zeile(row)0
        //Zeile(row)1
        //TODO ich hab keine Fehlerbehandlungen :(
    FILE* open_file(char *file_name, char *rights)
    {
        FILE *file = fopen(file_name, rights);
        return file;
    }
    typedef struct {
        char delimiter;
        char quote;
    } configs;
    configs config = {
        .delimiter = ',',
        .quote     = '"'
    };
    void csv_set_delimiter(char d)
    {
        config.delimiter = d;
    }

    void csv_set_quote(char q)
    {
        config.quote = q;
    }
    //TODO muss nochmal neu gemacht werden, wegen den quotes und delimitern, 
    int csv_export(csv_Table *table, char *export_file_name)
    {
        FILE *exportfile = fopen(export_file_name, "w");
        if (!exportfile) 
        {
            printf("Datei konnte nicht geoeffnet werden\n");
            return 1;
        }
        char delimiter = config.delimiter;
        char quote = config.quote;
        //header
        if(table->has_header)
        {
            for (int j = 0; j < table->cols; j++)
            {
                csv_write_cell_export(exportfile, table->header[j]);
                //wenn nicht null dann value raushauen
                if (j+1 < table->cols)
                {   
                    fputc(delimiter, exportfile);
                }
                //dann delimiter bis auf das letzte value, value, value
            }
            fputc('\n', exportfile);
        }
        for (int i = 0; i < table->rows; i++)
        {
            for (int j = 0; j < table->cols; j++)   
            {
                csv_write_cell_export(exportfile, table->data[i][j]);
                if (j + 1 < table->cols) 
                {
                    fputc(delimiter, exportfile);           
                }
            }
            fputc('\n',exportfile);
        }
        fclose(exportfile);
        return 0;
    }
    void count_rows_cols(csv_Table *table, FILE *file)
    {
        //zeiger in den getline reinschreibt()
        char *zeile = NULL; 
        //dynmaischer puffer
        size_t grenze = 0;
        //ruckgabewert von getline also lange der zeile bei -1 wenn leer
        long len;

        size_t rows = 0;
        size_t max_cols = 0;

        //bis es -1 erreicht = eof
        while((len = getline(&zeile, &grenze, file)) != -1)
        {
            //zeilenumbruch switchen 
            if(zeile[len -1] == '\n')
            {
                zeile[len -1] = '\0';
            }
            //gehe durch jeden buchstaben und zahle die kommas bis ende also \0. mit 1 einfangen weil ja dann die letzte auch dazu gehoert
            size_t col_count = 0;
            //leere zeilen gehe naechste iteration  <- continue
            if (zeile[0] == '\0')
            {
                continue;
            }
            //zeiger auf start der zeile
            char *token = zeile;
            
            while(1)
            {
                //einzelen token rausholen, wenn keine mehr dann dass raus aus der while und dann zählen
                char *value = my_tokenizer(&token);
                //wwenn fertig
                if(!value)
                {
                    break;
                }
                col_count++;
                free(value);
            }
                if(col_count > max_cols)
                {
                    max_cols = col_count;
                }
                rows++;
        }
        free(zeile);
        if (rows == 0)
        {
            table->rows = 0;
            table->cols = 0;
        }
        //-1 weil header
        else
        {
            if(table->has_header)
            {
                table->rows = rows-1;
            }
            else
            {
                table->rows = rows;
            }

            table->cols = max_cols;
        }
        //wieder an anfang von zeile für spaeter
        rewind(file);
        }

    void create_table(csv_Table *table)
    {
        //1 ebene, zeiger auf arrays(sind ja auch zeiger) 
        // groese von tabelle die aus zeilenzeiger besteht ist doppel pointer * zeilen
        //table->data = char***
        table->data = malloc(sizeof(char**)*table->rows);
        for (int i = 0; i < table->rows; i++)
        {
            //reservierung von arrays also 2 ebene, jede Zeile zeigt auf seinen spalten array
            //table->data[i] ist vom Typ char** -> eine Zeile bestehend aus einem array von char* zellzeigern
            table->data[i]= malloc(sizeof(char*)*table->cols);
            for (int j = 0; j < table->cols;j++)
            {
                table->data[i][j] = NULL;
                
            }
        }
        //types
        //für jede spalte einmal platz für ein Celltype reservieren
        table->col_types = malloc(sizeof(CellType) *table->cols);
        for (int z = 0; z < table->cols; z++)
        {
            table->col_types[z] = Type_Empty;
        }
        //header
        table->header = malloc(sizeof(char*)*table->cols);
        for (int z = 0; z < table->cols; z++)
        {
            table->header[z] = NULL;
        }
    }
    CellType detect_type(const char* value)
        {
            //checks
            if(value == NULL || *value == '\0')
            {
                return Type_Empty;
            }
            //vorzeichen für negative zahlen z.b
            if(*value == '+' || *value == '-')
            {
                value++;
            }

            int dotcount = 0;
            //solange != \0
            while(*value)
            {
                if(*value == '.')
                {
                    dotcount++;
                }
                //wenn kein punkt und kein digit dann string
                else if(!isdigit(*value))
                {
                    return Type_String; 
                }
                value++;
            }
            if (dotcount == 0)
            {
                return Type_Int;
            }
            if (dotcount == 1)
            {
                return Type_Float;
            }
            return Type_String;
        }
    void detect_col_types(csv_Table *table)
    {
        for(int i = 0; i < table->cols;i++)
        {
            //startwert empty
            CellType detected = Type_Empty;
            for(int j = 0; j < table->rows; j++)
            {            
                detected =detect_type(table->data[j][i]);
                //sobald eine nicht leere zeile ist abbrechen da nur pro spalte ein typ
                if(detected != Type_Empty)
                {
                    break;
                }
            }
            table->col_types[i] = detected;
        }
        //logging
    }
    void csv_free(csv_Table *table)
    {
        for(int k = 0; k < table->rows; k++)
        {
            for(int l = 0; l<table->cols;l++ )
            {
                free(table->data[k][l]);
            }
            free(table->data[k]);
        }
        free(table->data);
        //header
        for (int z = 0; z < table->cols; z++)
        {
            free(table->header[z]);
        }
        free(table->header);
        //types
        free(table->col_types);
    }
    void csv_print(csv_Table *table)
    {
        for(int t = 0; t < table->cols; t++) 
        {
            printf("Spalte %d: Typ = %d\n", t, table->col_types[t]);
        }   
        printf("delimiter ist %c\n", config.delimiter);

        for (int h = 0; h < table->cols; h++)
            {
                if(table->header[h])
                {
                    printf("%d. %s", h, table->header[h]);
                }
                else
                {
                    printf("%d. empty", h);
                }
                if(h < table->cols -1)
                {
                    printf("\t");
                }
            
            }
        printf("\n");
        for (int i = 0; i < table->rows; i++)
        {

            for (int j = 0; j < table->cols; j++)
            {
                if (table->data[i][j])
                {
                    printf("%d. %s", j, table->data[i][j]);
                }
                else
                {
                    printf("%d. empty", j);
                }
                if (j < table->cols - 1) 
                {
                // tab zwischen den Spalten ausser am ende
                    printf("\t"); 
                }
            }
            printf("\n");
        }
        printf("rows=%zu, cols=%zu\n", table->rows, table->cols);

        //printf("%s",table->data[8][2]);
    }

    void csv_edit_cell(csv_Table *table, size_t row, size_t col, const char* value)
    {     
        //checks 
        if(row >= table->rows)
        {
            printf("Rows zu groß");

            return;
        }
        if(col >= table->cols)
        {
            printf("cols zu groß");

            return;

        }
        //wenn zeile ungleich leer alten string freigeben und auf null setzten
        if(table->data[row][col] != NULL)
        {
            free(table->data[row][col]);
            table->data[row][col] = NULL;
        }
        //wenn neuer wert bzw nicht leer
        if(value != NULL)
        {
            table->data[row][col] = strdup(value);
            printf("hab value gesetzt %s \n", value);
        }
        else
        {
            table->data[row][col] = NULL;
                    printf("hab null gesetzt");

        }
    }
    void csv_copy_cell(csv_Table *table, size_t srcRow, size_t srcCol, size_t dstRow, size_t dstCol)
    {
        //checks
        if(srcRow >= table->rows)
        {
            return;
        }
        if(srcCol >= table->cols)
        {
            return;
        }
        if(dstRow >= table->rows)
        {
            return;
        }
        if(dstCol >= table->cols)
        {
            return;
        }
        //wenn zielzeile nicht leer ->free und nullen
        if(table->data[dstRow][dstCol] != NULL)
        {
            free(table->data[dstRow][dstCol]);
            table->data[dstRow][dstCol] = NULL;
        }
        //wenn quelle wert hat dann copy sonst null setzen
        if(table->data[srcRow][srcCol] != NULL)
        {
            table->data[dstRow][dstCol] = strdup(table->data[srcRow][srcCol]);
        }
        else
        {
            table->data[dstRow][dstCol] = NULL;
        }
    }
    void csv_copy_row(csv_Table *table, size_t srcRow, size_t dstRow)
    {
        //checks
        if(srcRow >= table->rows || dstRow >= table->rows)
        {
            return;
        }
        if(srcRow == dstRow)
        {
            return;
        }
        for (int i = 0; i < table->cols ; i++)
        {
            //wenn zielzelle nicht null dann leeren
            if(table->data[dstRow][i]!= NULL)
            {
                free(table->data[dstRow][i]);
                table->data[dstRow][i] = NULL;             
            }
            //wenn quelle nicht null dann kopieren
            if(table->data[srcRow][i]!= NULL)
            {
                table->data[dstRow][i] = strdup(table->data[srcRow][i]);
            }
            else
            {
                table->data[dstRow][i] = NULL;
            }
        }
    }
    void csv_copy_col(csv_Table *table, size_t srcCol, size_t dstCol)
    {
        //checks
        if(srcCol >= table->cols || dstCol >= table->cols)
        {
            return;
        }
        if(srcCol == dstCol)
        {
            return;
        }
        //header
        //check ob header nicht leer
        if(table->header[dstCol] != NULL)
        {
            free(table->header[dstCol]);
            table->header[dstCol] = NULL;
        }
        if(table->header[srcCol] != NULL)
        {
            table->header[dstCol] = strdup(table->header[srcCol]); 
        }
        else
        {
            table->header[dstCol] = NULL;
        }
        //Types
        table->col_types[dstCol]  = table->col_types[srcCol];

        for (int i = 0; i < table->rows; i++)
        {
            //wenn ziel nicht null dann leeeren
            if(table->data[i][dstCol]!= NULL)
            {
                free(table->data[i][dstCol]);
                table->data[i][dstCol] = NULL;
            }
            //wenn quell nicht null dann kopieren sonst auf null setzen
            if(table->data[i][srcCol]!= NULL)
            {
                table->data[i][dstCol] = strdup(table->data[i][srcCol]);
            }
            else
            {
                table->data[i][dstCol] = NULL;
            }
        }
    }
        //Zeile(row) Spalte (col)

        //              Spalte col(0) Spalte col(1)
        //Zeile(row)0
        //Zeile(row)1
    void csv_delete_row(csv_Table *table, size_t row)
    {
        //checks
        if(row >= table->rows)
        {
            printf("row zu hoch %ld \n", row);
            return;
        }
        for (int i = 0; i < table->cols; i++)
        {
            free(table->data[row][i]);
        }
        free(table->data[row]);
        //alle zeilen eins nach oven schieben
        for (int r = row; r < table->rows -1; r++)
        {
            table->data[r] = table->data[r + 1];
        }
        //wert bei rows neu setzen -1
        table->rows = table->rows -1;
        //realloc(*pointer, size) weil table ja neue groese hat kleiner machen
        table->data = realloc(table->data,table->rows *sizeof(char**));
    }

    void csv_delete_col(csv_Table *table, size_t col)
    {
        //checks
        if(col >= table->cols)
        {
            return;
        }
        //erst alle zellen freen
        for (int i = 0; i < table->rows;i++)
        {
            free(table->data[i][col]);
        }
        //alle spalten nach 1 nacah links schieben 
        for (int r = 0; r < table->rows; r++)
        {
            for (int c = col; c < table->cols -1; c++)
            {
                table->data[r][c] = table->data[r][c + 1];
            }
        }
        //col -1 setzen
        table->cols = table->cols -1;
        //jede Zeile verkleinern um eine spalte
        for (int newr = 0; newr < table->rows; newr++)
        {
            table->data[newr] = realloc(table->data[newr],table->cols *sizeof(char*));
        }
        //header
        free(table->header[col]);
        //header nach links verschieben
        for(int h = col; h < table->cols; h++)
        {
            table->header[h] = table->header[h + 1];
        }
        //1 kleiner neu allokieren
        table->header = realloc(table->header,table->cols*sizeof(char*));
        //typenarray eins nach links
        for(int t = col; t < table->cols; t++)
        {
            table->col_types[t] = table->col_types[t +1 ];
        }
        table->col_types = realloc(table->col_types, table->cols*sizeof(CellType));
    }
        //Zeile(row) Spalte (col)
        //              Spalte col(0) Spalte col(1)
        //Zeile(row)0
        //Zeile(row)1
        //TODO ich muss die coltypes was gesetzt werden aktuell neu einlesen. 
    void csv_add_col(csv_Table *table, const char *headername)
    {
        //global
        table->cols = table-> cols + 1;
        //neue spalte ganz am ende auf 0 setzen
        for (int i = 0; i < table->rows; i++)
        {
            table->data[i] = realloc(table->data[i],table->cols *sizeof(char*));
            table->data[i][table->cols - 1] = NULL;
        }
        //header
        //vergroessern und headername falls da 

        table->header = realloc(table->header, table->cols*sizeof(char*));
        if(headername == NULL)
        {
            table->header[table->cols - 1] = NULL;
        }
        else
        {
            table->header[table->cols - 1] = strdup(headername);
        }
        //typen groesser neu machen um und auf empty setzen
        table->col_types = realloc(table->col_types, table->cols*sizeof(CellType));
        table->col_types[table->cols -1] = detect_type(headername);
        printf("rows=%zu, cols=%zu\n", table->rows, table->cols);


    } 
    void csv_add_row(csv_Table *table)
    {
        //global eins groesser und tabelle um eine zeile groesser
        table->rows = table-> rows + 1;
        table->data = realloc(table->data, table->rows *sizeof(char**));
        //fur die letzte Zeile neue groesse
        table->data[table->rows - 1] = malloc(sizeof(char*)*table->cols);
        for (int i = 0; i < table->cols; i++)
        {
            table->data[table->rows - 1][i] = NULL;
        } 
    }
    //mit getline jetzt und mytokenzier
    int csv_import(csv_Table *table, FILE *file)
    {
        if (file == NULL)
        {
            printf("Datei konnte nicht geoeffnet werden");
            return 1;
        }
        //zeilenpuffer für getline
        char *zeile = NULL;
        //dynamischer puffergroesse 
        //0 weil dieser immer von getline gesetzt wird, also dynmaisch.
        size_t grenze = 0;
        //wenn -1 dann fehler deswegen long damit - zahl auch okey ist
        long len;
        // HEADER
        size_t col;
        size_t row = 0;
        //nur header zeile - wegen dem if ;) getline liest ganze zeile bis zeilenumbruch
        //getline(&buffer,&size,file)
        len = getline(&zeile, &grenze, file);
        if(len == -1 )
        {
            printf("Fehler beim Lesen der Header-Zeile oder leere Datei.\n");
            free(zeile);
            fclose(file);
            return 1;
        }
        //ende tauschen zu \0
        if (zeile[len - 1] == '\n')
        {
            zeile[len - 1] = '\0';
        }
        //zeigeer auf anfang von header string
        //zeile ist buch, *token ist das lesezeichen
        char *token = zeile;
        if(table->has_header)
        {
            col = 0;
            while(col < table->cols)
            {
            //nimm mein wirkliches lesezeichen und keine kopie (token) wäre nur kopie 
                char *value = my_tokenizer(&token);
                //ende wenn NULL
                if(!value)
                {
                    break;
                }
                //fur die leeren zellen
                if(value[0] == '\0')
                {
                    free(value);
                    table->header[col] = NULL;
                }
                else
                //kopiert den aktuellen token der geht ja bis zum ersten \0
                {
                    table->header[col] = value;
                }
                col++;
            }
            //Header Zeilen ausfüllen wenn ich halt leere Headers habe wenn ich ja schon beim zeilenende bin aber noch col immernoch niedriger ist als wirkliche cols in table
            while (col < table->cols)
            {
                table->header[col] = NULL;
                col++;
            }
            //Daten
            row = 0; 
        }
        else
        {
            col = 0;
            while (col < table->cols)
            {
                char *value = my_tokenizer(&token);
                if (!value)
                    break;
                if (*value == '\0')
                {
                    free(value);
                    table->data[0][col] = NULL;
                }
                else
                {
                    table->data[0][col] = value;
                }
                col++;
            }
            while (col < table->cols)
            {
                table->data[0][col++] = NULL;
            }
            row = 1;
        }

        while ((len = getline(&zeile,&grenze, file)) != -1)
        {
            if (zeile[len - 1] == '\n')
            {
                zeile[len - 1] = '\0';
            }
            if (zeile[0] == '\0')
            {
                continue;
            }
            token = zeile;
            col = 0;

            while (col < table->cols)
            {
                char *value = my_tokenizer(&token);
                if (!value)
                {
                    break;
                }
                if(value[0] == '\0')
                {
                    free(value);
                    table->data[row][col] = NULL;
                }
                else
                {
                    table->data[row][col] = value;
                }
                col++;
            }

            while (col < table->cols)
            {
                table->data[row][col] = NULL;
                col++;
            }
            row++;
        }
        free(zeile);
        fclose(file);
        return 0;
    }
    int csv_cell_needs_quotes(char *value)
    {
        char delimiter = config.delimiter;
        char quote = config.quote;
        if(!value || *value == '\0')
        {
            return 0;
        }

        for(char *check = value; *check != '\0'; ++check)
        {
            if(*check == delimiter || *check == quote)
            {
                return 1;
            }
        }
        return 0;    
    }
    void csv_write_cell_export(FILE *file, char *value)
    {
        char delimiter = config.delimiter;
        char quote = config.quote;

        if(!value)
        {
            return;
        }
        int needs_quotes = csv_cell_needs_quotes(value);
        if(needs_quotes)
        {
            fputc(quote, file);
        }
        for(char *check = value; *check != '\0'; check++)
        {
            if(*check == quote)
            {
                fputc(quote, file);
                fputc(quote, file);
            }
            else
            {
                fputc(*check, file);
            }
        }
        if(needs_quotes)
        {
            fputc(quote, file);
        }
    }
    char *my_tokenizer(char **token)
    {
        //zeigt auf aktuelle position des tokens
        char *p = *token;
        if(*p == '\0')
        {
            return NULL;
        }
        char delimiter = config.delimiter;
        char quote = config.quote;
        int quoted = 0;
        int len = 0;
        int cap = 16;
        //hier kommt der value rein der rausgeschrieben wird
        char *out = malloc(cap);
        if(!out)
        {
            return NULL;
        }
        //überhaupt quotes
        if(*p == quote)
        {
            quoted = 1;
            p++;
        }
        while(*p)
        {
            //quote fall
            if(quoted)
            {
                if(*p == quote)
                { 
                    //check ob das übernächste zeichen auch ein " ist
                    if(p[1] == quote)
                    {
                        if(len + 1 >= cap)
                        {
                            cap*=2;
                            out = realloc(out,cap);
                            if(!out)
                            {
                                return NULL;
                            }
                        }
                            out[len]= quote;
                            len++;
                            p = p +2;
                            continue;
                        }
                        else
                        {
                            p++;
                            if(*p == delimiter)
                            {
                                p++;
                            }
                            break;
                            
                        }

                }
                else
                {
                    if (len + 1 >= cap)
                    {
                        cap *= 2;
                        out = realloc(out, cap);
                        if (!out)
                        {
                            return NULL;
                        }
                    }
                    out[len++] = *p++;

                }
            }
            else
            {
                if(*p == delimiter)
                {
                    p++;
                    break;
                }
                else
                {
                    if (len + 1 >= cap)
                    {
                        cap *= 2;
                        out = realloc(out, cap);
                        if (!out)
                        {
                            return NULL;
                        }
                    }
                    out[len] = *p;
                    len++;
                    p++;
                }
            }
        }
        out[len] = '\0';
        //printf("%s", out);
        *token = p;
        return out;
    }
    //TODO Zugriff auf Spalten mittels Index und Spaltennamen 
    //nochmal anschauen
    int csv_find_col_index(csv_Table *table, const char *name)
    {
        if(!table->has_header || name == NULL)
        {
            return -1;
        }
        for(int i = 0; i < table->cols; i++)
        {
            //erster check weil ja auch NUll drinnen sein kann, das will strcmp net
            if(table->header[i] && (strcmp(table->header[i],name)==0))
            {
                return i;
            }
        }
        return -1;
    }
    char *csv_get_cell_by_name(csv_Table *table, size_t row, const char* header)
    {
        //checks
        if(header == NULL)
        {
            return NULL;
        }
        //0basiert 4 ist eig 3
        if(row >= table->rows)
        {
            return NULL;
        }
        int col = csv_find_col_index(table,header);
        if(col < 0)
        {
            return NULL;
        }
        return table->data[row][col];
    }
    void csv_edit_cell_by_name(csv_Table *table, size_t row, char* header, char* value)
    {
        if(row >= table->rows)
        {
            return;
        }
        if(header == NULL)
        {
            return;
        }
        int col = csv_find_col_index(table, header);
        if(col<0)
        {
            return;
        }
        csv_edit_cell(table, row, (size_t)col, value);
    }
