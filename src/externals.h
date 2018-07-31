#pragma once
#include <stdio.h>

extern "C" {
  extern FILE *yyin;         // input stream
  extern FILE *yyout;        // output stream
  extern int   yyleng;       // length of current lexeme
  extern char *yytext;       // text of current lexeme
  extern int   yylex();      // the generated lexical analyzer
  extern int   yyLine;       // the current source code line
}


