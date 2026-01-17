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
    int do_delete_col;
    size_t delete_col;
    int do_copy_row;
    size_t source_row_to_copy;
    size_t dst_row_to_copy;
    int do_copy_col;
    size_t source_col_to_copy;
    size_t dst_col_to_copy;
    int do_copy_cell;
    size_t source_row_for_cell_to_copy;
    size_t source_col_for_cell_to_copy;
    size_t dst_row_for_cell_to_copy;
    size_t dst_col_for_cell_to_copy;
    int has_header;

} arg_Configs;

//Optionen fuer die Ausfuehrung der Anwendung
struct argp_option options[] = 
{
    { "no-header", 1008, 0, 0, "CSV has no header row" },
    {"copy-cell", 1007, "SRC-ROW,SRC-COL,DST-ROW,DST-COL", 0, "cory src cell to dst cell (0-based)"},
    {"copy-col", 1006, "SRC,DST", 0, "copy src to dst col (0-based)"},
    {"copy-row", 1005, "SRC,DST", 0, "Copy src to dst row (0-based)"},
    {"delete-col", 1004, "COL", 0, "Delete col (0-based)"},
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
size_t size_t_or_die(struct argp_state *state, const char *what, const char *arg)
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
        argp_error(state, "must be not negative integer");
    }    
    return (size_t)tmp;
}

error_t parse_options(int key, char *arg, struct argp_state *state)
{
    arg_Configs *cfg = state->input;
    switch(key)
    {
        case 1008:
            cfg->has_header = 0;
            break;
        case 1007:
            if(!arg)
            {
                argp_error(state, "copy cell nees 4 values comma seperated");
            }
            char *tmpcopycell = strdup(arg);
            if(!tmpcopycell)
            {
                argp_error(state, "memory");
            }

            char *kommacell1 = strchr(tmpcopycell, ',');
            if(!kommacell1)
            {
                free(tmpcopycell);  
                argp_error(state, "use , between values");
            }
            *kommacell1 = '\0';
            char *kommacell2 = strchr(kommacell1 +1, ',');
            if(!kommacell2)
            {
                free(tmpcopycell);  
                argp_error(state, "use , between values");
            }
            *kommacell2 ='\0';
            char *kommacell3 = strchr(kommacell2 +1, ',');
            if(!kommacell3)
            {
                free(tmpcopycell);  
                argp_error(state, "use , between values");
            }
            *kommacell3 = '\0';
            char* srcRow = tmpcopycell;
            char* srcCol = kommacell1 + 1;
            char* dstRow = kommacell2 + 1;
            char* dstCol = kommacell3 + 1;

            cfg->source_row_for_cell_to_copy = size_t_or_die(state, "--copy-cell-src-row",srcRow);
            cfg->source_col_for_cell_to_copy = size_t_or_die(state, "--copy-cell-src-col",srcCol);
            cfg->dst_row_for_cell_to_copy = size_t_or_die(state, "--copy-cell-dst-row", dstRow);
            cfg->dst_col_for_cell_to_copy = size_t_or_die(state,"--copy-cell-dst-col", dstCol);
            free(tmpcopycell); 
            cfg->do_copy_cell = 1;           
            break;
        case 1006:
            if(!arg)
            {
                argp_error(state, "copy_col needs a src and dst");
            }
            char *tmpcol = strdup(arg);
            if(!tmpcol)
            {
                argp_error(state, "memory");
            }
            char *kommarow =strchr(tmpcol, ',');
            if(!kommarow)
            {
                free(tmpcol);
                argp_error(state, "use , between src and dst");
            }
            *kommarow = '\0';
            char *srccp = tmpcol;
            char *dstcp = kommarow +1;
            cfg->source_col_to_copy = size_t_or_die(state, "--copy-col SRC", srccp);
            cfg->dst_col_to_copy = size_t_or_die(state, "--copy-col DST", dstcp);
            cfg->do_copy_col = 1;
            free(tmpcol);
            break;
        case 1005:
            if(!arg)
            {
                argp_error(state, "copy-row needs src and dst");
            }
            char *tmp = strdup(arg);
            if(!tmp)
            {
                argp_error(state, "memory");
            }
            //zeigt auf die stelle nach
            char *komma = strchr(tmp,',');
            if(!komma)
            {
                free(tmp);
                argp_error(state, "use , between src and dst");
            }
            *komma = '\0';
            char *src = tmp;
            char *dst = komma + 1;
            cfg->source_row_to_copy = size_t_or_die(state, "--copy-row SRC", src);
            cfg->dst_row_to_copy = size_t_or_die(state,"--copy-row DST", dst);
            free(tmp);
            cfg->do_copy_row = 1;
            break;
        case 1004:
            cfg-> delete_col = size_t_or_die(state, "--delete-col", arg);
            cfg->do_delete_col = 1;
            break;
        case 1003:
            cfg->delete_row = size_t_or_die(state,"--delete-row", arg);
            cfg->do_delete_row = 1;
            break;
        case 1002:
            if(!arg)
            {
                cfg->add_col_header = NULL;
            }
            else
            {
                cfg->add_col_header = arg;
            }
            cfg->do_add_col = 1;

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
        .do_delete_col = 0,
        .delete_col = 0,
        .do_copy_col = 0,
        .source_col_to_copy = 0,
        .dst_col_to_copy = 0,
        .do_copy_cell = 0,
        .source_row_for_cell_to_copy = 0,
        .source_col_for_cell_to_copy = 0,
        .dst_row_for_cell_to_copy = 0,
        .dst_col_for_cell_to_copy = 0,
        .has_header = 1,

    };
    parse_cmdline(argc, argv, &cfg);
    csv_Table table;
    table.has_header = cfg.has_header;
    FILE *import_csvfile = open_file((char*)cfg.import_path, "r");
    count_rows_cols(&table,import_csvfile);
    create_table(&table);
    csv_import(&table,import_csvfile);
    detect_col_types(&table);   

    if(cfg.do_copy_cell)
    {
        csv_copy_cell(&table, cfg.source_row_for_cell_to_copy, cfg.source_col_for_cell_to_copy, cfg.dst_row_for_cell_to_copy, cfg.dst_col_for_cell_to_copy);
    } 
    if (cfg.do_add_col) 
    {
        csv_add_col(&table, cfg.add_col_header);
    }
    if(cfg.do_delete_row)
    {
        csv_delete_row(&table, cfg.delete_row);
    }
    if(cfg.do_delete_col)
    {
        csv_delete_col(&table, cfg.delete_col);
    }
    if(cfg.do_copy_col)
    {
        csv_copy_col(&table, cfg.source_col_to_copy, cfg.dst_col_to_copy);
    }
    if(cfg.do_copy_row)
    {
        csv_copy_row(&table, cfg.source_row_to_copy, cfg.dst_row_to_copy);
    }
    //csv_copy_row(&table, 1, 2) check
    //csv_copy_cell(&table, 1, 0, 2, 0); check
    //csv_copy_col(&table, 1,2);check
    //csv_delete_row(&table,1);  check
    //csv_delete_col(&table,1); check
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
    return 0;
}
