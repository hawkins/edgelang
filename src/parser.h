//*****************************************************************************
// Edge language recursive descent parser
// written by Josh Hawkins
//*****************************************************************************
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include "lexer.h"

class Parser {
public:
  Parser();

private:
  // Parse production rules
  void P();
  void S();
  void A(const std::string&);
  void E(const std::string& identifier, int lTok);
  void C();
  void M();
  void G();
  void R();

  // Determine next production rule
  bool IsFirstOfP();
  bool IsFirstOfS();
  bool IsFirstOfA();
  bool IsFirstOfE();
  bool IsFirstOfC();
  bool IsFirstOfM();
  bool IsFirstOfG();
  bool IsFirstOfR();

  // Flags for program behavior
  int JS;
  int INPUT;
  int PARSE_TREE;
  int DIRECTED;

public:
  void set_opt(int FLAG, int value);
  int get_opt(int FLAG);

  std::vector<std::string> get_node_lines();
  std::vector<std::string> get_edge_lines();

private:
  // Output variables
  std::vector<std::string> nodeLines;
  std::vector<std::string> edgeLines;
  std::string defaultLabel;

public:
  void addNodeToSet(std::string vertex, std::string label, bool update);

  // TODO: Return an AST
  bool parse(); 
};
