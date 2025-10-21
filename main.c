#include "csv.h"
//
int main ()
{
    csv_Table table;
    create_table(&table, 5, 3);
    //csv_set_cell(&table,0,0,"killme");
    csv_import(&table);
    csv_print(&table);
    csv_free(&table);

}
