//*****************************************************************************
// Edge language recursive descent parser\
// written by Josh Hawkins
//*****************************************************************************

#ifndef LEXER_H
#define LEXER_H

// Maximum lexeme length
#define MAX_LEXEME_LEN 100

//*****************************************************************************
// List of token codes
//*****************************************************************************

// Keywords
#define TOK_BEGIN         1000  // begin
#define TOK_END           1001  // end
#define TOK_TRUE          1002  // true
#define TOK_FALSE         1003  // false

// Punctuation
#define TOK_ARROW         2000  // ->
#define TOK_CUBE          2001  // ::
#define TOK_COLON         2002  // :
#define TOK_COMMA         2003  // ,
#define TOK_COMMENT       2010  // |
#define TOK_COMMENT_START 2011  // |-
#define TOK_COMMENT_END   2012  // -|

// Properties
#define TOK_WEIGHTED      3000  // weighted
#define TOK_DIRECTED      3001  // directed
#define TOK_VERTEX_LABEL  3002  // vertexlabel
#define TOK_EDGE_LABEL    3003  // edgelabel

// Useful abstractions
#define TOK_IDENTIFIER    4000  // identifier
#define TOK_VERTEX        5000  // vertex
#define TOK_EOF           6000  // end of file
#define TOK_UNKNOWN       7000  // unknown lexeme

#endif
