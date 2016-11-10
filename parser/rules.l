/*******************************************************************
Edge language recursive descent parser rules
written by Josh Hawkins
*******************************************************************/
%option noyywrap
%{
#include "lexer.h"

int yyLine = 1;
%}

%%

 /* Keywords */
begin { return TOK_BEGIN; }
end   { return TOK_END; }
true  { return TOK_TRUE; }
false { return TOK_FALSE; }


 /* Properties */
weighted    { return TOK_WEIGHTED; }
directed    { return TOK_DIRECTED; }
vertexlabel { return TOK_VERTEX_LABEL; }
edgelabel   { return TOK_EDGE_LABEL; }


 /* Punctuation */
->  { return TOK_ARROW; }
::  { return TOK_CUBE; }
:   { return TOK_COLON; }
,   { return TOK_COMMA; }
\|  { return TOK_COMMENT; }
\|- { return TOK_COMMENT_START; }
-\| { return TOK_COMMENT_END; }


 /* Abstractions */
[A-Z]*              { return TOK_VERTEX; }
[a-z][0-9A-Za-z_]*  { return TOK_IDENTIFIER; }


 /* Eat any whitespace */
[ \t\r]+


 /* Eat newline characters, AND increment line counter */
[\n]      { yyLine++; }


 /* Found an unknown character */
.         { return TOK_UNKNOWN; }


 /* Recognize end of file */
<<EOF>>   { return TOK_EOF; }
