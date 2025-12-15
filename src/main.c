#include "csv.h"
#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


const char *argp_program_version = "Import_Export_1.0";
const char doc[] = "Read csv Files and also can export them";
char args_doc[] = "";
typedef struct
{  
    const char *import_path;
    const char *export_path;
    int do_print;
    int do_add_row;
    int do_add_col;
    const char* add_col_header;
    int do_delete_row;
    size_t delete_row;
} arg_Configs;

//Optionen fuer die Ausfuehrung der Anwendung
struct argp_option options[] = 
{
    {"delete-row", 1003, "ROW", 0, "Delete row (0-based)"},
    {"add-col", 1002, "NAME", OPTION_ARG_OPTIONAL, "Add a new column with header if no header its empty"},
    {"add-row", 1001, 0, 0, "Add one empty row"},
    {"print",  'p', 0, 0, "Print table after import"},
    {"file",   'f', "FILE", 0, "CSV file to import"},
    {"export", 'e', "FILE", 0, "CSV file to export"},
    {"delim",  'D', "CHAR", 0, "Delimiter character (default ,)"},
    {"quote",  'Q', "CHAR", 0, "Quote character (default \")"},
    {0}
};

error_t parse_options(int key, char *arg, struct argp_state *state)
{
    arg_Configs *cfg = state->input;
    switch(key)
    {

        case 1003:
        //TODO 2c wird noch angemommen, sonst passt 
            cfg->do_delete_row = 1;
            //cfg->delete_row = size_t_or_die("--delete-row",arg,state);
            break;
            
            /*if((strcmp(arg,"0")) == 0)
                {
                    cfg->delete_row = 0;
                    printf("Wert ist 0 \n");
                    break;
                }
            cfg->delete_row = (size_t)strtoul(arg,NULL,10);
            if(cfg->delete_row == 0)
            {
                printf("Wert ist ungültig\n");
                return 1;
            }
                */

        case 1002:
            cfg->do_add_col = 1;
            if(!arg)
            {
                cfg->add_col_header = NULL;
            }
            else
            {
                cfg->add_col_header = arg;
            }
            break;
        case 1001:
            cfg->do_add_row = 1;
            break;
        case 'p':
            cfg->do_print = 1;
            break;
        case 'f':
            cfg->import_path = arg;
            break;
        case 'e':
            cfg->export_path = arg;
            break;
        case 'D':
            if(!arg || arg[0] =='\0' || arg[1] != '\0')
            {
                argp_error(state, "Delimiter must be one char!");
            }
            csv_set_delimiter(arg[0]);  
            break;
        case 'Q':
            if (!arg || arg[0] == '\0' || arg[1] != '\0')
            {
                argp_error(state, "Quote must be one char!");
            }
            csv_set_quote(arg[0]);
            break;
        }
 
        return 0;
}
int parse_cmdline(int argc, char **argv, arg_Configs *cfg)
{
    struct argp parse = {options, parse_options, args_doc, doc};
    int ret = argp_parse (&parse, argc, argv, 0,0, cfg);
    return ret;
}

size_t size_t_or_die(const char *value, const char *arg, struct argp_state *state)
{
    if(!arg||arg[0] == '\0')
    {
        argp_error(state, "requiers a value!");
    }
    errno = 0;
    char *end = NULL;
    unsigned long tmp = strtoul(arg, &end, 10);

    if(errno != 0 || end == arg || *end != '\0')
    {
        argp_error(state, "negative integer");
    }    
    return (size_t)tmp;
}
int test()
{
    printf("%ld\n", (size_t)strtol("-10",NULL,10));
    printf("%ld\n", strtoul("test",NULL,10));
    printf("%ld\n", strtoul("20",NULL,10));
    printf("%ld\n", strtoul("",NULL,10));
}

int main (int argc, char **argv)
{



    //TODO import/export gerade fest, aber ich glaub das lass ich als default
    arg_Configs cfg = 
    {
        .import_path = "/home/kuss/backup/prpr-tabellen-import-export/src/test_data_basic.csv",
        .export_path = "/home/kuss/backup/prpr-tabellen-import-export/src/export_test.csv",
        .do_print = 0,
        .do_add_row = 0,
        .do_add_col = 0,
        .add_col_header = NULL,
        .do_delete_row = 0,
        .delete_row = 0,
    };
    parse_cmdline(argc, argv, &cfg);
    csv_Table table;
    FILE *import_csvfile = open_file((char*)cfg.import_path, "r");
    count_rows_cols(&table,import_csvfile);
    create_table(&table);
    csv_import(&table,import_csvfile);
    detect_col_types(&table);    
    if (cfg.do_add_col) 
    {
        csv_add_col(&table, cfg.add_col_header);
    }
    if(cfg.do_delete_row)
    {
        csv_delete_row(&table, cfg.delete_row);
    }
    //csv_copy_cell(&table, 1, 0, 2, 0);
    //csv_copy_col(&table, 1,2);
    //csv_delete_row(&table,1); 
    //csv_delete_col(&table,1);
    //csv_add_row(&table);  check
    //csv_add_col(&table, "Tag"); check
    //csv_edit_cell(&table,2,table.cols-1,"test");
    if(cfg.do_add_row)
    {
        csv_add_row(&table);
    }   
    csv_export(&table,(char*)cfg.export_path);
    if(cfg.do_print)
    {
        csv_print(&table);
    }
    csv_free(&table);
}
