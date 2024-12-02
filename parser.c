/*parser.c*/

//
// Recursive-descent parsing functions for nuPython programming language.
// The parser is responsible for checking if the input follows the syntax
// ("grammar") rules of nuPython. If successful, a copy of the tokens is
// returned so the program can be analyzed and executed.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // true, false
#include <assert.h>

#include "token.h"
#include "tokenqueue.h"
#include "scanner.h"
#include "parser.h"


//
// declarations of private functions:
//
static void errorMsg(char* expecting, char* value, struct Token found);
static bool match(struct TokenQueue* tokens, int expectedID, char* expectedValue);

static bool parser_expr(struct TokenQueue* tokens);
static bool parser_body(struct TokenQueue* tokens);
static bool parser_else(struct TokenQueue* tokens);

static bool parser_if_then_else(struct TokenQueue* tokens);
static bool parser_pass_stmt(struct TokenQueue* tokens);
static bool parser_empty_stmt(struct TokenQueue* tokens);
static bool startOfStmt(struct TokenQueue* tokens);
static bool parser_stmt(struct TokenQueue* tokens);
static bool parser_stmts(struct TokenQueue* tokens);
static bool parser_program(struct TokenQueue* tokens);

// declarations of more that I create 

static bool parser_op(struct TokenQueue* tokens);
static bool parser_unary_expr(struct TokenQueue* tokens);
static bool parser_element(struct TokenQueue* tokens);
static bool parser_function_call(struct TokenQueue* tokens);
static bool parser_value(struct TokenQueue* tokens);
static bool parser_while_loop(struct TokenQueue* tokens);
static bool parser_assignment(struct TokenQueue* tokens);
static bool parser_call_stmt(struct TokenQueue* tokens);


//
// errorMsg:
//
// Outputs a properly-formatted syntax error message of the form
// "expecting X, found Y".
//
static void errorMsg(char* expecting, char* value, struct Token found)
{
  printf("**SYNTAX ERROR @ (%d,%d): expecting %s, found '%s'\n",
    found.line, found.col, expecting, value);
}


//
// match
//
// Checks to see if the token at the front of the queue matches
// the exectedID. If so, the token is removed from the queue and
// true is returned. If not, an error message is output and false
// is returned.
// 
// If false is returned, the error message output is of the form
// "expecting X, found Y" where X is the value of the expected 
// token and Y is the expectedValue passed in. 
//
static bool match(struct TokenQueue* tokens, int expectedID, char* expectedValue)
{
  //
  // does the token match the expected token?
  //
  struct Token curToken = tokenqueue_peekToken(tokens);
  char* curValue = tokenqueue_peekValue(tokens);

  if (curToken.id != expectedID)  // no, => error
  {
    errorMsg(expectedValue, curValue, curToken);
    return false;
  }

  //
  // yes, it matched, so discard and return true:
  //
  tokenqueue_dequeue(tokens);

  return true;
}


// 
// <element> ::= INDENTIFIER, INT_LITERAL, REAL_LITERAL, STR_LITERAL, True, False, None
// 

static bool parser_element(struct TokenQueue* tokens)
{
  struct Token curToken = tokenqueue_peekToken(tokens);

  if (curToken.id == nuPy_IDENTIFIER)
  {
    return match(tokens, nuPy_IDENTIFIER, "IDENTIFIER");
  }

  else if (curToken.id == nuPy_INT_LITERAL)
  {
    return match(tokens, nuPy_INT_LITERAL, "INT_LITERAL");
  }

  else if (curToken.id == nuPy_REAL_LITERAL)
  {
    return match(tokens, nuPy_REAL_LITERAL, "REAL_LITERAL");
  }

  else if (curToken.id == nuPy_STR_LITERAL)
  {
    return match(tokens, nuPy_STR_LITERAL, "STR_LITERAL");
  }

  else if (curToken.id == nuPy_KEYW_TRUE)
  {
    return match(tokens, nuPy_KEYW_TRUE, "true");
  }

  else if (curToken.id == nuPy_KEYW_FALSE)
  {
    return match(tokens, nuPy_KEYW_FALSE, "false");
  }
  
  else if (curToken.id == nuPy_KEYW_NONE)
  {
    return match(tokens, nuPy_KEYW_NONE, "None");
  }
  errorMsg("element", "not an element", curToken);
  return false;


}

/*
<unary_expr> ::= '*' IDENTIFIER 
                | '&' IDENTIFIER 
                | '+' [IDENTIFIER | INT_LITERAL | REAL_LITERAL]
                | '-' [IDENTIFIER | INT_LITERAL | REAL_LITERAL]
                |<element>
*/


static bool parser_unary_expr(struct TokenQueue* tokens)
{

  struct Token curToken = tokenqueue_peekToken(tokens);
  char* curValue = tokenqueue_peekValue(tokens);
  //'*' IDENTIFIER 
  if (curToken.id == nuPy_ASTERISK)
  {
    if (!match(tokens, nuPy_ASTERISK, "*"))
      return false;
    if (!match(tokens, nuPy_IDENTIFIER, "IDENTIFIER"))
      return false;
    return true;
  }
  //'&' IDENTIFIER 
  else if (curToken.id == nuPy_AMPERSAND)
  {
    if (!match(tokens, nuPy_AMPERSAND, "&"))
      return false;
    if (!match(tokens, nuPy_IDENTIFIER, "IDENTIFIER"))
      return false;
    return true;
  }
  //'+' [IDENTIFIER | INT_LITERAL | REAL_LITERAL]
  else if (curToken.id == nuPy_PLUS)
  {
    if (!match(tokens, nuPy_PLUS, "+"))
      return false;
    struct Token nextToken = tokenqueue_peekToken(tokens);
    char* nextValue = tokenqueue_peekValue(tokens);

    if (nextToken.id == nuPy_IDENTIFIER)
    {
      return match(tokens, nuPy_IDENTIFIER, "IDENTIFIER");
    }
    else if (nextToken.id == nuPy_INT_LITERAL)
    {
      return match(tokens, nuPy_INT_LITERAL, "INT_LITERAL");

    } 
    else if (nextToken.id == nuPy_REAL_LITERAL)
    {
      return match(tokens, nuPy_REAL_LITERAL, "REAL_LITERAL");
    }
    else 
    {
      errorMsg("identifier or numberic literal", nextValue, nextToken);
      return false;
    }
  }
  //'-' [IDENTIFIER | INT_LITERAL | REAL_LITERAL]
  else if (curToken.id == nuPy_MINUS)
  {
    if (!match(tokens, nuPy_MINUS, "-"))
      return false;
    struct Token nextToken = tokenqueue_peekToken(tokens);
    char* nextValue = tokenqueue_peekValue(tokens);


     if (nextToken.id == nuPy_IDENTIFIER)
    {
      return match(tokens, nuPy_IDENTIFIER, "IDENTIFIER");
    }
    else if (nextToken.id == nuPy_INT_LITERAL)
    {
      return match(tokens, nuPy_INT_LITERAL, "INT_LITERAL");
    } 
    else if (nextToken.id == nuPy_REAL_LITERAL)
    {
      return match(tokens, nuPy_REAL_LITERAL, "REAL_LITERAL");
    }
    else 
    {
      errorMsg("identifier or numberic literal", nextValue, nextToken);
      return false;
    }
  }
  //<element>
  else if (curToken.id == nuPy_IDENTIFIER 
      || curToken.id == nuPy_INT_LITERAL 
      || curToken.id == nuPy_REAL_LITERAL 
      || curToken.id == nuPy_STR_LITERAL 
      || curToken.id == nuPy_KEYW_TRUE 
      || curToken.id == nuPy_KEYW_FALSE 
      || curToken.id == nuPy_KEYW_NONE)
  {
    return parser_element(tokens);
  }
  errorMsg("unary expression", "nothing", curToken);
  return false;
}

//create parser_element - done


// 
// <op> ::= + | - | * | ** | % | / | == | != | < | <= | > | >= | is | in
//

static bool parser_op(struct TokenQueue* tokens)
{
  struct Token curToken = tokenqueue_peekToken(tokens);

  if (curToken.id == nuPy_PLUS)
  {
    return match(tokens, nuPy_PLUS, "+");
  }

  else if (curToken.id == nuPy_MINUS)
  {
    return match(tokens, nuPy_MINUS, "-");
  }

  else if (curToken.id == nuPy_ASTERISK)
  {
    return match(tokens, nuPy_ASTERISK, "*");
  }

  else if (curToken.id == nuPy_POWER)
  {
    return match(tokens, nuPy_POWER, "**");
  }

  else if (curToken.id == nuPy_PERCENT)
  {
    return match(tokens, nuPy_PERCENT, "%%");
  }

  else if (curToken.id == nuPy_SLASH)
  {
    return match(tokens, nuPy_SLASH, "/");
  }

  else if (curToken.id == nuPy_EQUALEQUAL)
  {
    return match(tokens, nuPy_EQUALEQUAL, "==");
  }

  else if (curToken.id == nuPy_NOTEQUAL)
  {
    return match(tokens, nuPy_NOTEQUAL, "!=");
  }

  else if (curToken.id == nuPy_LT)
  {
    return match(tokens, nuPy_LT, "<");
  }
  else if (curToken.id == nuPy_LTE)
  {
    return match(tokens, nuPy_LTE, "<=");
  }
  else if (curToken.id == nuPy_GT)
  {
    return match(tokens, nuPy_GT, ">");
  }
  else if (curToken.id == nuPy_GTE)
  {
    return match(tokens, nuPy_GTE, ">=");
  }
  else if (curToken.id == nuPy_KEYW_IS)
  {
    return match(tokens, nuPy_KEYW_IS, "is");
  }
  else if (curToken.id == nuPy_KEYW_IN)
  {
    return match(tokens, nuPy_KEYW_IN, "in");
  }
  errorMsg("op", "other", curToken);
  return false;
}




//
// <expr> ::= <unary_expr> [<op> <unary_expr>]
//

static bool parser_expr(struct TokenQueue* tokens)
{
  //
  // TODO: done?
  //
  // does the unary_expr match?
  if (!parser_unary_expr(tokens)) 
    return false;

  //make a struct to peek at the next token; does it have an op? 
  struct Token curToken = tokenqueue_peekToken(tokens);

  if (curToken.id == nuPy_PLUS || 
      curToken.id == nuPy_MINUS || 
      curToken.id == nuPy_ASTERISK || 
      curToken.id == nuPy_POWER || 
      curToken.id == nuPy_PERCENT || 
      curToken.id == nuPy_SLASH || 
      curToken.id == nuPy_EQUALEQUAL || 
      curToken.id == nuPy_NOTEQUAL || 
      curToken.id == nuPy_LT || 
      curToken.id == nuPy_LTE || 
      curToken.id == nuPy_GT || 
      curToken.id == nuPy_GTE || 
      curToken.id == nuPy_KEYW_IS || 
      curToken.id == nuPy_KEYW_IN )
  //if op it should also have unary expr
  {
    if (!parser_op(tokens))
      return false;
    if (!parser_unary_expr(tokens))
      return false;
    return true;
  }
  //else just return true

  return true;

}
  

// create parser_unary_expr - yes
// create parser_op - yes





//
// <body> ::= '{' EOLN <stmts> '}' EOLN
//

static bool parser_body(struct TokenQueue* tokens)
{
  //
  // TODO: done?
  //
  if (!match(tokens, nuPy_LEFT_BRACE, "{"))
    return false;
  if (!match(tokens, nuPy_EOLN, "EOLN"))
    return false;
  if (!parser_stmts(tokens))
    return false;
  if (!match(tokens, nuPy_RIGHT_BRACE, "}"))
    return false;
  if (!match(tokens, nuPy_EOLN, "EOLN"))
    return false;
  return true;
}


//
// <else> ::= elif <expr> ':' EOLN <body> [<else>]
//          | else ':' EOLN <body>
//
static bool parser_else(struct TokenQueue* tokens)
{
  //
  // TODO: done
  //

  struct Token curToken = tokenqueue_peekToken(tokens);

  if (curToken.id == nuPy_KEYW_ELIF)
  {
    if (!match(tokens, nuPy_KEYW_ELIF, "ELIF"))
      return false;
    if (!parser_expr(tokens))
      return false;
    if (!match(tokens, nuPy_COLON, ":"))
      return false;
    if (!match(tokens, nuPy_EOLN, "EOLN"))
      return false;
    if(!parser_body(tokens))
      return false;
    struct Token nextToken = tokenqueue_peekToken(tokens);
    if (nextToken.id == nuPy_KEYW_ELSE || nextToken.id == nuPy_KEYW_ELIF)
    {
      if (!parser_else(tokens))
        return false;
    }
  }
  if (curToken.id == nuPy_KEYW_ELSE)
  {
    if (!match(tokens, nuPy_KEYW_ELSE, "ELSE"))
      return false;
    if (!match(tokens, nuPy_COLON, ":"))
      return false;
    if (!match(tokens, nuPy_EOLN, "EOLN"))
      return false;
    if (!parser_body(tokens))
      return false; 
  }
  return true;
}


//
// <if_then_else> ::= if <expr> ':' EOLN <body> [<else>]
//
static bool parser_if_then_else(struct TokenQueue* tokens)
{
  if (!match(tokens, nuPy_KEYW_IF, "if"))
    return false;

  if (!parser_expr(tokens))
    return false;

  if (!match(tokens, nuPy_COLON, ":"))
    return false;

  if (!match(tokens, nuPy_EOLN, "EOLN"))
    return false;

  if (!parser_body(tokens))
    return false;

  //
  // is the optional <else> present?
  //
  struct Token curToken = tokenqueue_peekToken(tokens);

  if (curToken.id == nuPy_KEYW_ELIF || curToken.id == nuPy_KEYW_ELSE)
  {
    bool result = parser_else(tokens);
    return result;
  }
  else
  {
    // <else> is optional, missing => do nothing and return success:
    return true;
  }
}


// 
// <pass_stmt> ::= pass EOLN
//
static bool parser_pass_stmt(struct TokenQueue* tokens)
{
  if (!match(tokens, nuPy_KEYW_PASS, "pass"))
    return false;

  if (!match(tokens, nuPy_EOLN, "EOLN"))
    return false;

  return true;
}


// 
// <empty_stmt> ::= EOLN
//
static bool parser_empty_stmt(struct TokenQueue* tokens)
{
  if (!match(tokens, nuPy_EOLN, "EOLN"))
    return false;

  return true;
}


//
// startOfStmt
//
// Returns true if the next token denotes the start of a stmt,
// and false if not.
//
static bool startOfStmt(struct TokenQueue* tokens)
{
  struct Token curToken = tokenqueue_peekToken(tokens);

  //
  // TODO: this is not complete.
  //

  if (curToken.id == nuPy_KEYW_PASS ||
      curToken.id == nuPy_EOLN ||
      curToken.id == nuPy_KEYW_IF || 
      curToken.id == nuPy_KEYW_WHILE ||
      curToken.id == nuPy_ASTERISK ||
      curToken.id == nuPy_IDENTIFIER || 
      curToken.id == nuPy_INT_LITERAL || 
      curToken.id == nuPy_REAL_LITERAL || 
      curToken.id == nuPy_STR_LITERAL || 
      curToken.id == nuPy_KEYW_TRUE || 
      curToken.id == nuPy_KEYW_FALSE || 
      curToken.id == nuPy_KEYW_NONE)
      {
    return true;
  }
  else {
    return false;
  }
}


//
// <stmt> ::= <assignment>
//          | <if_then_else>
//          | <while_loop>
//          | <call_stmt>
//          | <pass_stmt>
//          | <empty_stmt>
//
static bool parser_stmt(struct TokenQueue* tokens)
{
  //
  // TODO: for now we just accept a program consisting of a
  // single "pass" or "empty" statement.
  //
  if (!startOfStmt(tokens)) {
    struct Token curToken = tokenqueue_peekToken(tokens);
    char* curValue = tokenqueue_peekValue(tokens);

    errorMsg("start of a statement", curValue, curToken);
    return false;
  }

  //
  // we have the start of a stmt, but which one?
  //
  struct Token curToken = tokenqueue_peekToken(tokens);
  struct Token peekNext = tokenqueue_peek2Token(tokens);
  char* curValue = tokenqueue_peekValue(tokens);

  if (curToken.id == nuPy_KEYW_PASS) {
    bool result = parser_pass_stmt(tokens);
    return result;
  }
  else if (curToken.id == nuPy_EOLN) {
    bool result = parser_empty_stmt(tokens);
    return result;
  }
  else if (curToken.id == nuPy_KEYW_IF) {
    bool result = parser_if_then_else(tokens);
    return result;
  }
  else if (curToken.id == nuPy_KEYW_WHILE) {
    bool result = parser_while_loop(tokens);
    return result;
  }
  else if (curToken.id == nuPy_ASTERISK && peekNext.id == nuPy_IDENTIFIER){
    bool result = parser_assignment(tokens);
    return result;
  }
   else if (curToken.id == nuPy_IDENTIFIER && peekNext.id == nuPy_EQUAL){
    bool result = parser_assignment(tokens);
    return result;
  }
  else if (curToken.id == nuPy_IDENTIFIER && peekNext.id == nuPy_LEFT_PAREN){
    bool result = parser_call_stmt(tokens);
    return result;
  }
  else if (curToken.id == nuPy_IDENTIFIER){
    errorMsg("assignment or function call", curValue, curToken);
    return false;
  }
  
  else {
    printf("**INTERNAL ERROR: unknown stmt (parser_stmt)\n"); 
    //errorMsg("assignment or function call", curValue, curToken);
    return false;
  }
}



//
// <stmts> ::= <stmt> [<stmts>]
//
static bool parser_stmts(struct TokenQueue* tokens)
{
  //
  // TODO: for now we just accept a program consisting of a
  // single statement.
  //
  if (!parser_stmt(tokens))
    return false;

  struct Token curToken = tokenqueue_peekToken(tokens);

  if (curToken.id == nuPy_ASTERISK 
      || curToken.id == nuPy_IDENTIFIER 
      || curToken.id == nuPy_KEYW_IF 
      || curToken.id == nuPy_KEYW_WHILE 
      || curToken.id == nuPy_KEYW_PASS 
      || curToken.id == nuPy_EOLN)
  {
    bool result = parser_stmts(tokens);
    return result;
  }

  return true;
}


//
// <program> ::= <stmts> EOS
//
static bool parser_program(struct TokenQueue* tokens)
{
  if (!parser_stmts(tokens))
    return false;

  if (!match(tokens, nuPy_EOS, "$"))
    return false;

  return true;
}


//
//<call_stmt> ::= <function_call> EOLN
//
static bool parser_call_stmt(struct TokenQueue* tokens)
{
  if(!parser_function_call(tokens))
    return false;
  if(!match(tokens, nuPy_EOLN, "EOLN"))
    return false;
  return true;
}


//
//<function_call> ::= IDENTIFIER '(' [<element>] ')'
//

static bool parser_function_call(struct TokenQueue* tokens)
{
  if (!match(tokens, nuPy_IDENTIFIER, "IDENTIFIER"))
    return false;
  if (!match(tokens, nuPy_LEFT_PAREN, "("))
    return false;
  
  struct Token curToken = tokenqueue_peekToken(tokens);
  if (curToken.id == nuPy_IDENTIFIER 
      || curToken.id == nuPy_INT_LITERAL 
      || curToken.id == nuPy_REAL_LITERAL 
      || curToken.id == nuPy_STR_LITERAL 
      || curToken.id == nuPy_KEYW_TRUE 
      || curToken.id == nuPy_KEYW_FALSE 
      || curToken.id == nuPy_KEYW_NONE)
  {
    if (!parser_element(tokens))
      return false;
  }
  if (!match(tokens, nuPy_RIGHT_PAREN, ")"))
    return false;
  return true;
}

//
//<value> ::= <expr> | <function_call>
//

static bool parser_value(struct TokenQueue* tokens)
{
  struct Token curToken = tokenqueue_peekToken(tokens);
  struct Token peekNext = tokenqueue_peek2Token(tokens);
  //if identifier and left paren, then must be function call. otherwise expression
  if (curToken.id == nuPy_IDENTIFIER && peekNext.id == nuPy_LEFT_PAREN)
  {
    return parser_function_call(tokens);
  }
    
  //now must be expr
  return parser_expr(tokens);
}

//
// <while_loop> ::= while <expr> ':' EOLN <body>
//

static bool parser_while_loop(struct TokenQueue* tokens)
{
  if (!match(tokens, nuPy_KEYW_WHILE, "while"))
    return false;

  if (!parser_expr(tokens))
    return false;

  if (!match(tokens, nuPy_COLON, ":"))
    return false;

  if (!match(tokens, nuPy_EOLN, "EOLN"))
    return false;

  if (!parser_body(tokens))
    return false;
  return true;
}

//
// <assignment> ::= ['*'] IDENTIFIER '=' <value> EOLN
//

static bool parser_assignment(struct TokenQueue* tokens)
{
  struct Token curToken = tokenqueue_peekToken(tokens);
  //first check if the assignment starts with an asterisk
  if (curToken.id == nuPy_ASTERISK)
  {
    if (!match(tokens, nuPy_ASTERISK, "*"))
      return false;
  }

  if(!match(tokens, nuPy_IDENTIFIER, "IDENTIFIER"))
    return false;
  if(!match(tokens, nuPy_EQUAL, "="))
    return false;
  if(!parser_value(tokens))
    return false;
  if(!match(tokens, nuPy_EOLN, "EOLN"))
    return false;
  return true;
}
//
// public functions:
//

//
// parser_parse
//
// Given an input stream, uses the scanner to obtain the tokens
// and then checks the syntax of the input against the BNF rules
// for the subset of Python we are supporting. 
//
// Returns NULL if a syntax error was found; in this case 
// an error message was output. Returns a pointer to a list
// of tokens -- a Token Queue -- if no syntax errors were 
// detected. This queue contains the complete input in token
// form for further analysis.
//
// NOTE: it is the callers responsibility to free the resources
// used by the Token Queue.
//
struct TokenQueue* parser_parse(FILE* input)
{
  if (input == NULL) {
    printf("**INTERNAL ERROR: input stream is NULL (parser_parse)\n");
    return NULL;
  }

  //
  // First, let's get all the tokens and store them
  // into a queue:
  //
  int lineNumber, colNumber;
  char value[256];
  struct Token token;
  struct TokenQueue* tokens;

  scanner_init(&lineNumber, &colNumber, value);

  token = scanner_nextToken(input, &lineNumber, &colNumber, value);
  tokens = tokenqueue_create();

  while (token.id != nuPy_EOS)
  {
    tokenqueue_enqueue(tokens, token, value);

    token = scanner_nextToken(input, &lineNumber, &colNumber, value);
  }

  // enqueue the final token:
  tokenqueue_enqueue(tokens, token, value);

  //
  // now duplicate the tokens so that we have a copy after the
  // parsing process is over --- we need a copy so we can return
  // in case the parsing is successful. The tokens are then used
  // for analysis and execution.
  //
  struct TokenQueue* duplicate;

  duplicate = tokenqueue_duplicate(tokens);

  //
  // okay, now let's parse the input tokens:
  //
  bool result = parser_program(tokens);

  //
  // When we are done parsing, we are going to 
  // execute (assuming the parse was successful).
  // If the input is coming from the keyboard, 
  // consume the rest of the input after the $ 
  // before we start executing the python which
  // may do it's own input from the keyboard:
  //
  if (result && input == stdin) {
    int c = fgetc(stdin);
    while (c != '\n' && c != EOF)
      c = fgetc(stdin);
  }

  //
  // done, free memory and return tokens or NULL:
  //
  tokenqueue_destroy(tokens);

  if (result) // parse was successful
  {
    return duplicate;
  }
  else  // syntax error, nothing to execute:
  {
    tokenqueue_destroy(duplicate);

    return NULL;
  }
}
