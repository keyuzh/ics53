// DO NOT MODIFY THIS FILE
// This constants can be replaced with alternates during grading

#ifndef CONSTANTS_2_H
#define CONSTANTS_2_H

#define USAGE_MSG "53library [-n NUM] [-a | -d] [-o OUTFILE] -I ISBN | -D DATE | -G GENRE | -N NAME | -K WORD\n"                                     \
                  "\n  -I ISBN     Search by ISBN"\
                  "\n  -D DATE     Search for books written on or after DATE (format mm/dd/yyyyy)"\
                  "\n  -G GENRE    Search by GENRE"\
                  "\n  -N NAME     Search by author with NAME"\
                  "\n  -K KEYWORD  Search by KEYWORD in title or author name"\
                  "\n  -n NUM      Print top NUM results of the search."\
                  "\n  -a |-d      Print the search results in ascending or descending order based on ISBN number"\
                  "\n  -o          Output is written to OUTFILE if specified. If unspecified, output is written to stdout"\
                  "\n  -H          Prints the usage statement to STDOUT. All other arguments are ignored."                                                                      

#endif
