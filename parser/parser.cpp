//*****************************************************************************
// Edge language recursive descent parser
// written by Josh Hawkins
//*****************************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <map>

using namespace std;

#include "lexer.h"

extern "C"
{
// Instantiate global variables
extern FILE *yyin;         // input stream
extern FILE *yyout;        // output stream
extern int   yyleng;       // length of current lexeme
extern char *yytext;       // text of current lexeme
extern int   yylex();      // the generated lexical analyzer
extern int   yyLine;       // the current source code line
}

// Production functions
void P ( void );
void S ( void );
void A ( string );
void E ( string );
void C ( void );
void M ( void );
void G ( void );
void R ( void );

// Function declarations for checking whether the current token is
// in the first set of each production rule.
bool IsFirstOfP ( void );
bool IsFirstOfS ( void );
bool IsFirstOfA ( void );
bool IsFirstOfE ( void );
bool IsFirstOfC ( void );
bool IsFirstOfM ( void );
bool IsFirstOfG ( void );
bool IsFirstOfR ( void );

// Helper function to help print the tree nesting
string psp ( int );

// Needed global variables
typedef map<string, string> SymbolTableT; // Symbol Table
int iTok;                                 // The current token
SymbolTableT SymbolTable;                 // The symbol table


//*****************************************************************************
// The main processing loop
int main (int argc, char* argv[])
{
  // hold each token code
  int token;

  // Set the input and output streams
  yyin = stdin;
  yyout = stdout;

  // Get the first token
  iTok = yylex();

  // Begin parsing
  try {
    if (!IsFirstOfP())
      throw "unrecognized symbol at start of program";

    // Process P Production
    P();

    if (iTok != TOK_EOF)
      throw "end of file expected, but there is more here!";
  } catch(char const *errmsg) {
    cout << endl << "***ERROR (line " << yyLine << "): "<< errmsg << endl;
    return 1;
  }

  // Print success
  cout << endl
       << "=== Edge language parse was successful ==="
       << endl << endl;;

  // Print out the symbol table
  SymbolTableT::iterator it;
  for (it = SymbolTable.begin(); it != SymbolTable.end(); ++it) {
    cout << "symbol = " << it->first << ", value = " << it->second << endl;
  }

  // Return 0 to indicate successful run
  return 0;
}

//*****************************************************************************
// P --> {S}
void P ()
{
  static int Pcnt = 0;
  int CurPcnt = Pcnt++;

  cout << psp(CurPcnt) << "enter P " << CurPcnt << endl;

  // There might be a series of S's
  while (IsFirstOfS())
    S();

  // Read the next token
  iTok = yylex();

  cout << psp(CurPcnt) << "exit P " << CurPcnt << endl;
}

//*****************************************************************************
// S --> A | E | C | M | G
void S ()
{
  static int Scnt = 0;
  int CurScnt = Scnt++;
  string vertex;
  char const *Serr =
    "statement does not start with 'begin', '|', '|-', or vertex";

  cout << psp(CurScnt) << "enter S " << CurScnt << endl;

  // HACK: Set iTok if _ is found
  if (strcmp(yytext, "_") == 0)
    iTok = TOK_IDENTIFIER;

  // We know we have found a beginning token, but which?
  switch (iTok) {
    case TOK_BEGIN:
      G();
      break;

    case TOK_COMMENT:
      C();
      break;

    case TOK_COMMENT_START:
      M();
      break;

    case TOK_IDENTIFIER:
      // Capture vertex name
      vertex = yytext;

      iTok = yylex();
      if (iTok == TOK_ARROW)
        E(vertex);
      else
        throw "edge does not have arrow following vertex or ID";
      break;

    case TOK_VERTEX:
      // Capture vertex name
      vertex = yytext;

      // Determine if this starts an assignment or edge
      iTok = yylex();
      if (iTok == TOK_CUBE) {
        A(vertex);
      } else {
        // Must be edge
        E(vertex);
      }
      break;

    default:
      // If we made it to here, syntax error
      throw Serr;
  }

  cout << psp(CurScnt) << "exit S " << CurScnt << endl;
}


//*****************************************************************************
// A --> VERTEX :: [ ID ]
void A (string vertex)
{
  static int Acnt = 0;
  int CurAcnt = Acnt++;

  cout << psp(CurAcnt) << "enter A " << CurAcnt << endl;

  // We know we have found :: since we've determined this is assignment
  cout << "-->found " << yytext << endl;

  // Next could be an identifier; save its name
  iTok = yylex();
  string IDname = "_";
  if (iTok == TOK_IDENTIFIER) {
    // Capture and print ID
    IDname = yytext;
    cout << "-->found ID: " << yytext << endl;

    // Read the next token
    iTok = yylex();
  } else {
    cout << "-->Assuming ID _" << endl;
  }

  // If the identifier is not yet in the symbol table, store it there
  SymbolTableT::iterator it = SymbolTable.find(vertex);
  if (it == SymbolTable.end()) {
    SymbolTable.insert(pair<string, string>(vertex, IDname));
  }

  // Update ID in symbol table with value from expression
  it = SymbolTable.find(vertex);
  it->second = IDname;

  cout << psp(CurAcnt) << "exit A " << CurAcnt << endl;
}

//*****************************************************************************
// E --> ID | VERTEX(, Weight)(, Label) -> (ID | VERTEX)(, Weight)(, Label)
void E (string identifier)
{
  static int Ecnt = 0;
  int CurEcnt = Ecnt++;
  string vertexSource;
  string vertexTarget;

  cout << psp(CurEcnt) << "enter E " << CurEcnt << endl;

  // Print source vertex
  cout << "-->found " << identifier << endl;

  // Determine if we have an identifier or vertex, and find vertex if appropriate
  if (iTok == TOK_VERTEX) {
    vertexSource = identifier;
  } else {
    // Lookup vertex from symbol table
    SymbolTableT::iterator it = SymbolTable.find(identifier);
    if (it != SymbolTable.end()) {
      vertexSource = it->second;
    }
  }

  // We have already found an arrow
  cout << "-->found " << yytext << endl;

  // Get next token
  iTok = yylex();
  if (iTok != TOK_VERTEX && iTok != TOK_IDENTIFIER)
    throw "edge target does not begin with vertex or identifier";

  // Determine target vertex
  cout << "-->found " << yytext << endl;
  if (iTok == TOK_VERTEX) {
    vertexTarget = yytext;
  } else {
    // Lookup vertex from symbol table
    SymbolTableT::iterator it = SymbolTable.find(yytext);
    if (it != SymbolTable.end()) {
      vertexTarget = it->second;
    }
  }

  // TODO: Incorporate properties like weight, label

  // Read next token
  iTok = yylex();

  cout << psp(CurEcnt) << "exit E " << CurEcnt << endl;
}

//*****************************************************************************
// C --> \| { . }
void C ()
{
  static int Ccnt = 0;
  int CurCcnt = Ccnt++;

  cout << psp(CurCcnt) << "enter C " << CurCcnt << endl;

  cout << "-->found " << yytext << endl;

  // Read tokens until end of line
  int currentLine = yyLine;
  while (yyLine == currentLine)
    iTok = yylex();

  cout << psp(CurCcnt) << "exit C " << CurCcnt << endl;
}

//*****************************************************************************
// M --> \|- { . } -\|
void M ()
{
  static int Mcnt = 0;
  int CurMcnt = Mcnt++;

  cout << "-->found " << yytext << endl;

  cout << psp(CurMcnt) << "enter M " << CurMcnt << endl;

  // Read tokens until end of comment found
  while (iTok != TOK_COMMENT_END)
    iTok = yylex();

  cout << "-->found " << yytext << endl;


  // Read next token
  iTok = yylex();

  cout << psp(CurMcnt) << "exit M " << CurMcnt << endl;
}

//*****************************************************************************
// G --> begin LABEL { R } end
void G ()
{
  static int Gcnt = 0;
  int CurGcnt = Gcnt++;

  cout << psp(CurGcnt) << "enter G " << CurGcnt << endl;

  // We've found 'begin'
  cout << "-->found " << yytext << endl;

  // TODO: Process section label
  // We expect to find 'config'
  iTok = yylex();
  cout << "-->found " << yytext << endl;

  // We may see many or no R's
  iTok = yylex();
  while (iTok != TOK_END) {
    if (IsFirstOfR())
      R();
    else
      throw "R does not begin with valid property";
  }

  // Read next token
  iTok = yylex();

  cout << psp(CurGcnt) << "exit G " << CurGcnt << endl;
}

//*****************************************************************************
// R --> PROPERTY : true | false
void R ()
{
  static int Rcnt = 0;
  int CurRcnt = Rcnt++;

  cout << psp(CurRcnt) << "enter R " << CurRcnt << endl;

  cout << "-->found property " << yytext << endl;
  string property = yytext;

  // We expect to see :
  iTok = yylex();
  if (iTok == TOK_COLON)
    cout << "-->found " << yytext << endl;
  else
    throw "did not find : after property";

  // TODO: Set properties in some table to value

  // Get true or false value
  iTok = yylex();
  if (iTok == TOK_TRUE) {
    cout << "-->found TRUE" << endl;
  } else if (iTok == TOK_FALSE) {
    cout << "-->found FALSE" << endl;
  } else {
    throw "property not set to true or false";
  }

  // Read next token
  iTok = yylex();

  cout << psp(CurRcnt) << "exit R " << CurRcnt << endl;
}

//*****************************************************************************
// TODO IsFirstOfX Functions
//*****************************************************************************
bool IsFirstOfP()
{
  return iTok == TOK_VERTEX || iTok == TOK_COMMENT || iTok == TOK_COMMENT_START ||
    iTok == TOK_BEGIN;
}
//*****************************************************************************
bool IsFirstOfS()
{
  return iTok == TOK_BEGIN || iTok == TOK_IDENTIFIER || iTok == TOK_VERTEX ||
    iTok == TOK_COMMENT || iTok == TOK_COMMENT_START || strcmp(yytext, "_") == 0;
}
//*****************************************************************************
bool ISFirstOfA()
{
  return iTok == TOK_VERTEX;
}
//*****************************************************************************
bool IsFirstOfE()
{
  cout << yytext << endl;
  cout << strcmp(yytext, "_") << endl;
  return iTok == TOK_IDENTIFIER || iTok == TOK_VERTEX;
}
//*****************************************************************************
bool IsFirstOfC()
{
  return iTok == TOK_COMMENT;
}
//*****************************************************************************
bool IsFirstOfM()
{
  return iTok == TOK_COMMENT_START;
}
//*****************************************************************************
bool IsFirstOfG()
{
  return iTok == TOK_BEGIN;
}
//*****************************************************************************
bool IsFirstOfR()
{
  // Assuming all properties are in range 3000-3999
  return (iTok >= 3000 && iTok < 4000);
}
//*****************************************************************************
// Helper Functions
//*****************************************************************************
string psp(int n) // Stands for p-space, but I want the name short
{
  string str(n, ' ');
  return str;
}
