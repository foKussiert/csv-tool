Minimal:

./csvprog -f input.csv

Export:

./csvprog -f input.csv -e out.csv

Semikolon + Standard-Quote:

./csvprog -f input.csv -e out.csv -D ';'

Custom quote:

./csvprog -f input.csv -e out.csv -D ';' -Q "'"

Ausgabe:

./csvprog -f input.csv -p

Add column:

./csvprog -D ';' --add-col=Tag -p

Delete row (0-based):

./csvprog -f input.csv --delete-row 3 -e out.csv

Delete col (0-based):

./csvprog -f input.csv --delete-col 2 -e out.csv

Edit cell (ROW,COL,VALUE):

./csvprog -f input.csv --edit-cell 1,2,"Hello World" -e out.csv

Copy cell (r1,c1,r2,c2):

./csvprog -f input.csv --copy-cell 0,0,4,1 -e out.csv

Kombiniert (Beispiel):

./csvprog -f input.csv -D ';' --add-col X --edit-cell 0,3,42 --delete-row 2 -e out.csv -p