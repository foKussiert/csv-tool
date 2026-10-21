#  CSV-Tool – Tabellen importieren, bearbeiten, exportieren

**Kommandozeilen-Tool in C zum Einlesen, Bearbeiten und Exportieren von CSV-Dateien.**

Studienprojekt an der OTH Regensburg. Der CSV-Parser ist komplett selbst geschrieben, ohne externe Bibliothek, mit dynamischer Speicherverwaltung (`malloc`/`realloc`/`free`).

---

## Features

- **Eigener CSV-Parser** mit frei wählbarem Trennzeichen (`,` `;` …) und Quote-Zeichen
- Korrekte Behandlung von Sonderfällen: Trennzeichen innerhalb von Anführungszeichen, escapte Quotes (`""`), leere Zellen, unterschiedlich lange Zeilen
- Optionale Kopfzeile (Header) und Zugriff auf Spalten per Index oder Spaltenname
- **Automatische Typerkennung** pro Spalte: leer, Ganzzahl, Kommazahl oder Text
- Tabelle bearbeiten: Zeilen und Spalten hinzufügen, löschen und kopieren, einzelne Zellen kopieren
- Export zurück in eine CSV-Datei, Werte werden dabei bei Bedarf automatisch wieder in Quotes gesetzt
- Kommandozeilen-Optionen mit `argp` inklusive `--help` und Eingabeprüfung

## Beispiel

**Eingabe** (`beispiel.csv`):
```
Name;Age;City
Ali;30;Boston
Bob;25;New York
Charlie;40;"Los Angeles; CA"
```

**Aufruf:**
```bash
./csvprog -f beispiel.csv -D ";" -e ausgabe.csv -p
```

**Ausgabe:**
```
Spalte 0: Typ = 3
Spalte 1: Typ = 1
Spalte 2: Typ = 3
delimiter ist ;
0. Name     1. Age  2. City
0. Ali      1. 30   2. Boston
0. Bob      1. 25   2. New York
0. Charlie  1. 40   2. Los Angeles; CA
rows=3, cols=3
```
Das Semikolon in `"Los Angeles; CA"` wird richtig als Teil des Werts erkannt und nicht als Trennzeichen. Spalte 1 wird als Ganzzahl erkannt (Typ 1), die anderen als Text (Typ 3).

**Zeile löschen, Spalte hinzufügen und exportieren:**
```bash
./csvprog -f beispiel.csv -D ";" --delete-row 1 --add-col=Land -e ergebnis.csv
```
```
Name;Age;City;Land
Ali;30;Boston;
Charlie;40;"Los Angeles; CA";
```

## Kompilieren

Benötigt wird ein C-Compiler unter **Linux** (oder unter Windows über WSL), da `argp` und `getline` Teil der GNU C Library sind.

```bash
gcc -o csvprog main.c csv.c
```

## Optionen

| Option | Beschreibung |
|---|---|
| `-f <datei>` | CSV-Datei importieren |
| `-e <datei>` | Tabelle als CSV exportieren |
| `-p` | Tabelle auf der Konsole ausgeben |
| `-D <zeichen>` | Trennzeichen setzen (Standard: `,`) |
| `-Q <zeichen>` | Quote-Zeichen setzen (Standard: `"`) |
| `--no-header` | CSV ohne Kopfzeile verarbeiten |
| `--add-row` | Leere Zeile anhängen |
| `--add-col=<name>` | Neue Spalte mit Namen anhängen |
| `--delete-row <index>` | Zeile löschen |
| `--delete-col <index>` | Spalte löschen |
| `--copy-row SRC,DST` | Zeile SRC nach DST kopieren |
| `--copy-col SRC,DST` | Spalte SRC nach DST kopieren |
| `--copy-cell A,B,C,D` | Zelle (Zeile A, Spalte B) nach (Zeile C, Spalte D) kopieren |
| `--help` | Alle Optionen anzeigen |

Alle Indizes sind 0-basiert, gezählt wird ohne die Kopfzeile. Beim Aufruf sollten immer `-f` und `-e` angegeben werden.

## Aufbau

| Datei | Inhalt |
|---|---|
| `csv.h` | Datentypen (`csv_Table`, `CellType`) und Funktionsdeklarationen |
| `csv.c` | Parser (`my_tokenizer`), Import/Export, Typerkennung, Tabellenoperationen |
| `main.c` | Auswertung der Kommandozeilen-Optionen mit `argp` |

Die Tabelle wird als dynamisches zweidimensionales Array von Strings gespeichert (`char ***data`). Beim Löschen oder Hinzufügen von Zeilen und Spalten wird der Speicher per `realloc` angepasst.

## Technik

C · GNU C Library (`argp`, `getline`) · Autotools (`Makefile.am`) · Git
