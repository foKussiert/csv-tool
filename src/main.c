#include "csv.h"
#include <argp.h>

const char *argp_programm_version = "Import_Export_1.0";
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
    create_table(&table, 5, 3);
    //csv_set_cell(&table,0,0,"killme");
    csv_import(&table);
    csv_print(&table);
    csv_free(&table);

}
