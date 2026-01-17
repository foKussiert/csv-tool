# CSV Tabellen Import / Export
## Features

- Import von CSV-Dateien
- Unterstützung von Delimiter- und Quote-Zeichen
- Optionaler Header (erste Zeile als Spaltennamen)
- Zugriff auf Zellen per Index oder Spaltenname
- Tabellenmanipulation:
  - Zeilen hinzufügen / löschen
  - Spalten hinzufügen / löschen
  - Zeilen / Spalten kopieren
- Export der Tabelle in eine CSV-Datei
- Konsolenbasierte Ausgabe der Tabelle

## Wichtige Optionen 
| Option                 | Beschreibung                      |
| ---------------------- | --------------------------------- |
| `-f <datei>`           | CSV-Datei importieren             |
| `-e <datei>`           | Tabelle als CSV exportieren       |
| `-p`                   | Tabelle auf der Konsole ausgeben  |
| `-D <char>`            | Delimiter setzen (z. B. `;`)      |
| `-Q <char>`            | Quote-Zeichen setzen (z. B. `"`)  |
| `--no-header`          | CSV ohne Header verarbeiten       |
| `--add-row`            | Leere Zeile hinzufügen            |
| `--add-col <name>`     | Neue Spalte mit Header hinzufügen |
| `--delete-row <index>` | Zeile löschen (0-basiert)         |
| `--delete-col <index>` | Spalte löschen (0-basiert)        |
| `--copy-row SRC->DST`   | Zeile SRC nach DST kopieren       |
| `--copy-col SRC->DST`   | Spalte SRC nach DST kopieren      |
| `--copy-cell A,B,C,D`     | Zelle (A,B) nach (C,D) kopieren (0-basiert)       |

## Anwendungsbeispiele:
CSV einlesen und ausgeben:

./csvprog -f tmp.csv -p


CSV mit Semikolon und Quotes:

./csvprog -f tmp.csv -D ";" -Q '"' -p


Zeile löschen und exportieren:

./csvprog -f tmp.csv --delete-row 2 -e output.csv


Spalte hinzufügen:

./csvprog --add-col="test" -p -D ";"


Einzelne Zelle kopieren und exportieren:

./csvprog -f tmp.csv --copy-cell 0,0,2,1 -e output.csv -p -D ";"

