//*****************************************************************************
// Edge language recursive descent parser
// written by Josh Hawkins
//*****************************************************************************
#include <map>
#include <iostream>

#include "externals.h"
#include "constants.h"
#include "parser.h"
#include "lexer.h"

namespace {
  // Helper functions
  void addNodeToSet(std::string vertex, std::string label, bool update);
  
  std::string psp(int n)
  {
    std::string str(n, ' ');
    return str;
  }

  // Global parser state
  typedef std::map<std::string, std::string> SymbolTableT; // Symbol Table
  int iTok;                                 // The current token
  SymbolTableT SymbolTable;                 // The symbol table
}

Parser::Parser()
  : JS(1),
    INPUT(0),
    PARSE_TREE(0),
    DIRECTED(0),
    defaultLabel("_")
{
}

std::vector<std::string> Parser::get_node_lines() {
  return nodeLines;
}
std::vector<std::string> Parser::get_edge_lines() {
  return edgeLines;
}

void Parser::set_opt(int FLAG, int value) {
  switch(FLAG) {
    case(EDGE_OPT_PARSETREE):
      PARSE_TREE = value;
      break;
    case(EDGE_OPT_JS):
      JS = value;
      break;
    case(EDGE_OPT_INPUT):
      INPUT = value;
      break;
    case(EDGE_OPT_DIRECTED):
      DIRECTED = value;
      break;
    default:
      throw "Invalid EDGE_OPT flag";
  }
}

int Parser::get_opt(int FLAG) {
  switch(FLAG) {
    case(EDGE_OPT_PARSETREE):
      return PARSE_TREE;
    case(EDGE_OPT_JS):
      return JS;
    case(EDGE_OPT_INPUT):
      return INPUT;
    case(EDGE_OPT_DIRECTED):
      return DIRECTED;
    default:
      throw "Invalid EDGE_OPT flag";
  }
}

bool Parser::parse() { 
  // Get the first token
  iTok = yylex();  
  
  try {
    if (!IsFirstOfP())
      throw "unrecognized symbol at start of program";

    // Process P Production
    P();

    if (iTok != TOK_EOF)
      throw "end of file expected, but there is more here!";
  } catch(char const *errmsg) {
    std::cout << std::endl << "***ERROR (line " << yyLine << "): "<< errmsg << std::endl;

    if (INPUT)
      fclose(yyin);

    return false;
  }

  // Close input file
  if (INPUT)
    fclose(yyin);

  if (PARSE_TREE) {
    // Print success
    std::cout << std::endl
         << "=== Edge parse was successful ==="
         << std::endl << std::endl;

    // Print out the symbol table
    for (SymbolTableT::iterator it = SymbolTable.begin(); it != SymbolTable.end(); ++it) {
      std::cout << "Vertex = " << it->first << ", Label = " << it->second << std::endl;
    }
  }

  return true;
}

//*****************************************************************************
// P --> {S}
void Parser::P()
{
  static int Pcnt = 0;
  int CurPcnt = Pcnt++;

  if (PARSE_TREE)
    std::cout << psp(CurPcnt) << "enter P " << CurPcnt << std::endl;

  // There might be a series of S's
  while (IsFirstOfS())
    S();

  // Read the next token
  iTok = yylex();

  if (PARSE_TREE)
    std::cout << psp(CurPcnt) << "exit P " << CurPcnt << std::endl;
}

//*****************************************************************************
// S --> A | E | C | M | G
void Parser::S ()
{
  static int Scnt = 0;
  int CurScnt = Scnt++;
  std::string vertex;
  int lTok;
  char const *Serr =
    "statement does not start with 'begin', '|', '|-', or vertex";

  if (PARSE_TREE)
    std::cout << psp(CurScnt) << "enter S " << CurScnt << std::endl;

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
      lTok = iTok;

      iTok = yylex();
      if (iTok == TOK_ARROW)
        E(vertex, lTok);
      else
        throw "edge does not have arrow following vertex or ID";
      break;

    case TOK_VERTEX:
      // Capture vertex name
      vertex = yytext;
      lTok = iTok;

      // Determine if this starts an assignment or edge
      iTok = yylex();
      if (iTok == TOK_CUBE) {
        A(vertex);
      } else {
        // Must be edge
        E(vertex, lTok);
      }
      break;

    default:
      // If we made it to here, syntax error
      throw Serr;
  }

  if (PARSE_TREE)
    std::cout << psp(CurScnt) << "exit S " << CurScnt << std::endl;
}


//*****************************************************************************
// A --> VERTEX :: [ ID ]
void Parser::A (const std::string& vertex)
{
  static int Acnt = 0;
  int CurAcnt = Acnt++;

  if (PARSE_TREE)
    std::cout << psp(CurAcnt) << "enter A " << CurAcnt << std::endl;

  // We know we have found :: since we've determined this is assignment
  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;

  // Next could be an identifier; save its name
  iTok = yylex();
  std::string IDname = defaultLabel;
  if (iTok == TOK_IDENTIFIER) {
    // Capture and print ID
    IDname = yytext;
    if (PARSE_TREE)
      std::cout << "-->found ID: " << yytext << std::endl;

    // Read the next token
    iTok = yylex();
  } else {
    if (PARSE_TREE)
      std::cout << "-->Assuming ID: _" << std::endl;
  }

  // If the identifier is not yet in the symbol table, store it there
  SymbolTableT::iterator it = SymbolTable.find(vertex);
  if (it == SymbolTable.end()) {
    SymbolTable.insert(std::pair<std::string, std::string>(vertex, IDname));
  }

  // Update ID in symbol table with value from expression
  it = SymbolTable.find(vertex);
  it->second = IDname;

  // Add the node to our output
  addNodeToSet(vertex, IDname, true);

  if (PARSE_TREE)
    std::cout << psp(CurAcnt) << "exit A " << CurAcnt << std::endl;
}

//*****************************************************************************
// E --> ID | VERTEX(, Weight)(, Label) -> (ID | VERTEX)(, Weight)(, Label)
void Parser::E (const std::string& identifier, int lTok)
{
  static int Ecnt = 0;
  int CurEcnt = Ecnt++;
  std::string vertexSource;
  std::string vertexTarget;

  if (PARSE_TREE)
    std::cout << psp(CurEcnt) << "enter E " << CurEcnt << std::endl;

  // Determine if we have an identifier or vertex, and find vertex if appropriate
  if (lTok == TOK_VERTEX)
    vertexSource = identifier;
  else
    throw "Unidentified symbol before arrow in edge";

  // Print source vertex
  if (PARSE_TREE)
    std::cout << "-->found " << identifier << std::endl;

  // We have already found an arrow
  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;

  // Get next token
  iTok = yylex();
  if (iTok != TOK_VERTEX && iTok != TOK_IDENTIFIER)
    throw "edge target does not begin with vertex or identifier";

  // Determine target vertex
  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;
  if (iTok == TOK_VERTEX)
    vertexTarget = yytext;
  else
    throw "Unidentified symbol after arrow in edge";

  // Create vertices if they do not yet exist
  addNodeToSet(vertexSource, defaultLabel, false);
  addNodeToSet(vertexTarget, defaultLabel, false);

  // Write to edgeLines
  std::string edge = "\n  graph.linkNodes("
    + vertexSource
    + ", "
    + vertexTarget;
  if (DIRECTED) {
    edge = edge + ", { $directedTowards: "
      + vertexTarget
      + " }";
  }
  edge = edge + ");";
  edgeLines.push_back(edge);

  // Read next token
  iTok = yylex();

  if (PARSE_TREE)
    std::cout << psp(CurEcnt) << "exit E " << CurEcnt << std::endl;
}

//*****************************************************************************
// C --> \| { . }
void Parser::C ()
{
  static int Ccnt = 0;
  int CurCcnt = Ccnt++;

  if (PARSE_TREE)
    std::cout << psp(CurCcnt) << "enter C " << CurCcnt << std::endl;

  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;

  // Read tokens until end of line
  int currentLine = yyLine;
  while (yyLine == currentLine)
    iTok = yylex();

  if (PARSE_TREE)
    std::cout << psp(CurCcnt) << "exit C " << CurCcnt << std::endl;
}

//*****************************************************************************
// M --> \|- { . } -\|
void Parser::M ()
{
  static int Mcnt = 0;
  int CurMcnt = Mcnt++;

  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;

  if (PARSE_TREE)
    std::cout << psp(CurMcnt) << "enter M " << CurMcnt << std::endl;

  // Read tokens until end of comment found
  while (iTok != TOK_COMMENT_END)
    iTok = yylex();

  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;


  // Read next token
  iTok = yylex();

  if (PARSE_TREE)
    std::cout << psp(CurMcnt) << "exit M " << CurMcnt << std::endl;
}

//*****************************************************************************
// G --> begin LABEL { R } end
void Parser::G ()
{
  static int Gcnt = 0;
  int CurGcnt = Gcnt++;

  if (PARSE_TREE)
    std::cout << psp(CurGcnt) << "enter G " << CurGcnt << std::endl;

  // We've found 'begin'
  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;

  // TODO: Process section label
  // We expect to find 'config'
  iTok = yylex();
  if (PARSE_TREE)
    std::cout << "-->found " << yytext << std::endl;

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

  if (PARSE_TREE)
    std::cout << psp(CurGcnt) << "exit G " << CurGcnt << std::endl;
}

//*****************************************************************************
// R --> PROPERTY : true | false
void Parser::R ()
{
  static int Rcnt = 0;
  int CurRcnt = Rcnt++;

  if (PARSE_TREE)
    std::cout << psp(CurRcnt) << "enter R " << CurRcnt << std::endl;

  if (PARSE_TREE)
    std::cout << "-->found property " << yytext << std::endl;
  std::string property = yytext;

  // We expect to see :
  iTok = yylex();
  if (iTok == TOK_COLON) {
    if (PARSE_TREE)
      std::cout << "-->found " << yytext << std::endl;
  } else {
    throw "did not find : after property";
  }

  // TODO: Set properties in some table to value

  // Get true or false value
  iTok = yylex();
  if (iTok == TOK_TRUE) {
    if (PARSE_TREE)
      std::cout << "-->found TRUE" << std::endl;

    // Set flags
    if (strcmp(property.c_str(), "directed") == 0)
      DIRECTED = 1;
  } else if (iTok == TOK_FALSE) {
    if (PARSE_TREE)
      std::cout << "-->found FALSE" << std::endl;

    // Set flags
    if (strcmp(property.c_str(), "directed") == 0)
      DIRECTED = 0;
  } else {
    throw "property not set to true or false";
  }

  // Read next token
  iTok = yylex();

  if (PARSE_TREE)
    std::cout << psp(CurRcnt) << "exit R " << CurRcnt << std::endl;
}

//*****************************************************************************
// IsFirstOfX Functions
//*****************************************************************************
bool Parser::IsFirstOfP()
{
  return iTok == TOK_VERTEX || iTok == TOK_COMMENT || iTok == TOK_COMMENT_START ||
    iTok == TOK_BEGIN;
}
//*****************************************************************************
bool Parser::IsFirstOfS()
{
  return iTok == TOK_BEGIN || iTok == TOK_IDENTIFIER || iTok == TOK_VERTEX ||
    iTok == TOK_COMMENT || iTok == TOK_COMMENT_START || strcmp(yytext, "_") == 0;
}
//*****************************************************************************
bool Parser::IsFirstOfA()
{
  return iTok == TOK_VERTEX;
}
//*****************************************************************************
bool Parser::IsFirstOfE()
{
  return iTok == TOK_IDENTIFIER || iTok == TOK_VERTEX;
}
//*****************************************************************************
bool Parser::IsFirstOfC()
{
  return iTok == TOK_COMMENT;
}
//*****************************************************************************
bool Parser::IsFirstOfM()
{
  return iTok == TOK_COMMENT_START;
}
//*****************************************************************************
bool Parser::IsFirstOfG()
{
  return iTok == TOK_BEGIN;
}
//*****************************************************************************
bool Parser::IsFirstOfR()
{
  // Assuming all properties are in range 3000-3999
  return (iTok >= 3000 && iTok < 4000);
}
//*****************************************************************************
// Helper Functions
//*****************************************************************************
bool doesNodeExist(std::string vertex)
{
  // Lookup vertex from symbol table
  SymbolTableT::iterator it;
  for (it = SymbolTable.begin(); it != SymbolTable.end(); it++) {
    if (strcmp(it->first.c_str(), vertex.c_str()) == 0)
      return true;
  }
  return false;
}
//*****************************************************************************
void Parser::addNodeToSet(std::string vertex, std::string label, bool update)
{
  // Create node string
  std::string node = "\n  var "
    + vertex
    + " = graph.createNode({renderData: {name: '"
    + label
    + "'}});";

  if (doesNodeExist(vertex)) {
    // If allowed, update
    if (update) {
      std::string prefix = "\n  var " + vertex + " = ";
      for (std::vector<std::string>::iterator linesit = nodeLines.begin(); linesit != nodeLines.end(); linesit++) {
        std::string s = *linesit;
        if (s.find(prefix) == 0) {
          *linesit = node;
        }
      }
    }
  } else {
    // If not found, write to node lines
    nodeLines.push_back(node);
    SymbolTable.insert(std::pair<std::string, std::string>(vertex, label));
  }
}
