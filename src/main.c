#include "csv.h"
#include "options.h"
#include <argp.h>

const char *argp_program_version = "Import_Export_1.0";
const char doc[] = "Read csv Files and also can export them";
char args_doc[] = "";

//Optionen fuer die Ausfuehrung der Anwendung
struct argp_option options[] = 
{
{"debug", 'd', 0,0, "Prints debug text"},
    {0}
};

error_t parse_options(int key, char *arg, struct argp_state *state)
{
    switch(key)
    {
        case 'd':
            printf("Debug Option\n");
            break;
        case 'f':
            printf("Filepath to import\n");
            break;
        case 'e':
            printf("Name of exportfile - .csv\n");
    }
    return 0;
}
int parse_cmdline(int argc, char **argv)
{
    struct argp parse = {options, parse_options, args_doc, doc};
    int ret = argp_parse (&parse, argc, argv, 0,0, NULL);
    return ret;
}
int main (int argc, char **argv)
{

    parse_cmdline(argc, argv);
    csv_Table table;
    //TODO hier ubergabe mit argp -- spaeter
    FILE *import_csvfile = open_file("/home/kuss/backup/prpr-tabellen-import-export/src/test_data_basic.csv", "r");
    count_rows_cols(&table,import_csvfile);
    create_table(&table);
    csv_import(&table,import_csvfile);
    //TODO auch ubergabe -- spaeter
    //csv_edit_cell(&table,2,2,"test");
    //csv_copy_cell(&table, 1, 0, 2, 0);
    //csv_set_cell(&table,0,0,"killme");
    //csv_empty_cell(&table,3,1);
    //TODO auch ubergabe -- spaeter
    csv_export(&table, "/home/kuss/backup/prpr-tabellen-import-export/src/export_test.csv");
    csv_print(&table);
    csv_free(&table);
}
