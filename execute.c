/*execute.c*/

//
// Executes nuPython program, given as a Program Graph.
// 



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>  // true, false
#include <string.h>
#include <assert.h>
#include <math.h>

#include "programgraph.h"
#include "ram.h"
#include "execute.h"


//
// Private functions:
//
bool execute_function_call(struct STMT* stmt, struct RAM* memory);
static struct RAM_VALUE* execute_binary_expression(struct RAM_VALUE* lhs, int operator, struct RAM_VALUE* rhs, bool* success, struct STMT* stmt);
static bool execute_assignment(struct STMT* stmt, struct RAM* memory);


// retrieve value
// helper function, takes in an element and return it as a ram_value struct regardless of whether it came in as a string, int, bool, double, 
// or an identifier of one of these types

//if element is int, set the ram_to_return.types.i =  int_val. also set ram_to_return-> value_type to int
//if element is real_literal, set the ram_to_return.types.d = real_val. ram_to_return->value_type to double
//if element is string, set the ram_to_return.types.s = string
//if element is bool, set the ram_to_return.value_type = RAM_TYPE_BOOLEAN and ram_to_return->types.i = 0 or 1 
//if element is identifier, just set ram_to_return as ram_read_cell_by_name(memory, var_name)

struct RAM_VALUE* retrieve_value(struct ELEMENT* element, struct STMT* stmt, struct RAM* memory, bool* success) {
  *success = false;
  struct RAM_VALUE* ram_to_return = malloc(sizeof(struct RAM_VALUE));

  if (element->element_type == ELEMENT_IDENTIFIER) {
    char* var_name = element->element_value;
    ram_to_return = ram_read_cell_by_name(memory, var_name);
    *success = true;
    if (ram_to_return == NULL) {
      *success = false;
      printf("**SEMANTIC ERROR: name '%s' is not defined (line %d)\n", var_name, stmt->line);
    }
  }
  else if(element->element_type == ELEMENT_INT_LITERAL) {
    ram_to_return->value_type = RAM_TYPE_INT;
    ram_to_return->types.i = atoi(element->element_value);
    *success = true;
  }
  else if(element-> element_type == ELEMENT_REAL_LITERAL) {
    ram_to_return->value_type = RAM_TYPE_REAL;
    ram_to_return->types.d = atof(element->element_value);
    *success = true;
  }
  else if(element-> element_type == ELEMENT_TRUE) {
    ram_to_return->value_type = RAM_TYPE_BOOLEAN;
    ram_to_return->types.i = 1;
    *success = true;
  }
  else if(element-> element_type == ELEMENT_FALSE) {
    ram_to_return->value_type = RAM_TYPE_BOOLEAN;
    ram_to_return->types.i = 0;
    *success = true;
  }
  else if(element-> element_type == ELEMENT_STR_LITERAL) {
    ram_to_return->value_type = RAM_TYPE_STR;
    ram_to_return->types.s = element->element_value;
    *success = true;
  }
  return ram_to_return;
}



//
// execute_function_call
//
// Executes a function call statement, returning true if 
// successful and false if not (an error message will be
// output before false is returned, so the caller doesn't
// need to output anything).
//
// Function call only evaluates print
// 
// Examples: print()
//           print(x)
//           print(123)
//
bool execute_function_call(struct STMT* stmt, struct RAM* memory)
{
  struct STMT_FUNCTION_CALL* call = stmt->types.function_call;

  //
  // for now we are assuming it's a call to print:
  //
  char* function_name = call->function_name;
  //bool success;


    if (call->parameter == NULL)
      printf("\n");
    else {
      bool success;
      struct RAM_VALUE* to_print = retrieve_value(call->parameter, stmt, memory, &success);

      if (!success) {
        return false;
      }

      switch (to_print->value_type) {
        case RAM_TYPE_INT:
          printf("%d\n", to_print->types.i);
          break;
        case RAM_TYPE_REAL:
          printf("%lf\n", to_print->types.d);
          break;
        case RAM_TYPE_STR:
          printf("%s\n", to_print->types.s);
          break;
        case RAM_TYPE_BOOLEAN: 
          if (to_print->types.i == 0) {
            printf("False\n");
          } else if (to_print->types.i == 1){
            printf("True\n");
          } else {
            printf("Neither false nor true?\n");
            return false;
          }
          break;
        default:
          printf("Not int, real, string, or boolean\n");
          return false;
      }
    }
    return true;
}
  
// execute_int_int_binary
// takes in two ints and an operator, returns RAM_VALUE
// will return as RAM_TYPE_BOOLEAN for relational operators and RAM_TYPE_INT for other operators
// returns error for unrecognizable operators

struct RAM_VALUE* execute_int_int_binary(int lhs, int operator, int rhs) {
    struct RAM_VALUE* result = malloc(sizeof(struct RAM_VALUE));
    switch (operator)
  {
  case OPERATOR_PLUS:
    result->value_type = RAM_TYPE_INT;
    result->types.i = lhs + rhs;
    break;

  case OPERATOR_MINUS:
    result->value_type = RAM_TYPE_INT;
    result->types.i = lhs - rhs;
    break;

  case OPERATOR_ASTERISK:
    result->value_type = RAM_TYPE_INT;
    result->types.i = lhs * rhs;
    break;

  case OPERATOR_POWER:
    result->value_type = RAM_TYPE_INT;
    result->types.i = (int)pow(lhs, rhs);
    break;

  case OPERATOR_MOD:
    result->value_type = RAM_TYPE_INT;
    result->types.i = lhs % rhs;
    break;

  case OPERATOR_DIV:
    result->value_type = RAM_TYPE_INT;
    result->types.i = lhs / rhs;
    break;
  case OPERATOR_EQUAL:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs == rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_NOT_EQUAL:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs != rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_LT:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs < rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_LTE:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs <= rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_GT:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs > rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_GTE:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs >= rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  default:
    //
    // did we miss something?
    //
    printf("**INTERNAL ERROR: unexpected operator (%d) in execute_binary_expr\n", operator);
    assert(false);
  }
  return result;
}

// execute_real_real_binary
// takes in two doubles and an operator, returns RAM_VALUE
// will return as RAM_TYPE_BOOLEAN for relational operators and RAM_TYPE_REAL for other operators
// returns error for unrecognizable operators

struct RAM_VALUE* execute_real_real_binary(double lhs, int operator, double rhs) {
    struct RAM_VALUE* result = malloc(sizeof(struct RAM_VALUE));
    //printf("executing real real: %lf, %lf\n", lhs, rhs); DELETE LATER
    switch (operator)
  {
  case OPERATOR_PLUS:
    result->value_type = RAM_TYPE_REAL;
    result->types.d = lhs + rhs;
    //printf("computed result: %lf\n", result);  DELETE LATER
    break;

  case OPERATOR_MINUS:
    result->value_type = RAM_TYPE_REAL;
    result->types.d = lhs - rhs;
    break;

  case OPERATOR_ASTERISK:
    result->value_type = RAM_TYPE_REAL;
    result->types.d = lhs * rhs;
    break;

  case OPERATOR_POWER:
    result->value_type = RAM_TYPE_REAL;
    result->types.d = pow(lhs, rhs);
    break;

  case OPERATOR_MOD:
    result->value_type = RAM_TYPE_REAL;
    result->types.d = fmod(lhs, rhs);
    break;

  case OPERATOR_DIV:
    result->value_type = RAM_TYPE_REAL;
    result->types.d = lhs / rhs;
    break;

  case OPERATOR_EQUAL:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs == rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;

  case OPERATOR_NOT_EQUAL:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs != rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_LT:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs < rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_LTE:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs <= rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_GT:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs > rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;
  case OPERATOR_GTE:
    result->value_type = RAM_TYPE_BOOLEAN;
    if (lhs >= rhs) {
      result->types.i = 1;
    } else {
      result->types.i = 0;
    }
    break;

  default:
    //
    // did we miss something?
    //
    printf("**INTERNAL ERROR: unexpected operator (%d) in execute_binary_expr\n", operator);
    assert(false);
  }
  return result;
}


// execute_str_str_binary
// takes in two strings and an operator, returns RAM_VALUE of type RAM_TYPE_BOOLEAN
// value will be true if the binary expression using a relational operator and 2 strings is true, false if not
// returns error for unrecognizable operators

struct RAM_VALUE* execute_str_str_binary (char* s1, int operator, char* s2){
  struct RAM_VALUE* result = malloc(sizeof(struct RAM_VALUE));
  result->value_type = RAM_TYPE_BOOLEAN;

  switch(operator) {
    case OPERATOR_EQUAL:
      if (strcmp(s1,s2) == 0) {
        result->types.i = 1;
      } else {
        result->types.i = 0;
      }
      break;
    case OPERATOR_NOT_EQUAL:
      if (strcmp(s1,s2) != 0) {
        result->types.i = 1;
      } else {
        result->types.i = 0;
      }
      break;
    case OPERATOR_LT:
      if (strcmp(s1,s2) < 0) {
        result->types.i = 1;
      } else {
        result->types.i = 0;
      }
      break;
    case OPERATOR_LTE:
      if (strcmp(s1,s2) <= 0) {
        result->types.i = 1;
      } else {
        result->types.i = 0;
      }
      break;
    case OPERATOR_GT:
      if (strcmp(s1,s2) > 0) {
        result->types.i = 1;
      } else {
        result->types.i = 0;
      }
      break;
    case OPERATOR_GTE:
      if (strcmp(s1,s2) >= 0) {
        result->types.i = 1;
      } else {
        result->types.i = 0;
      }
      break;

    default:
      printf("**INTERNAL ERROR: unexpected operator (%d) in execute_binary_expr with 2 strings\n", operator);
      assert(false);
  }
  return result;

}

//is rel_op
// is rel_op takes operator and returns boolean: true if it is a relational operator, false otherwise
bool is_rel_op (int operator) {
  switch (operator) {
    case OPERATOR_EQUAL:
      return true;
      break;
    case OPERATOR_NOT_EQUAL:
      return true;
      break;
    case OPERATOR_LT:
      return true;
      break;
    case OPERATOR_LTE:
      return true;
      break;
    case OPERATOR_GT:
      return true;
      break;
    case OPERATOR_GTE:
      return true;
      break;
    default:
      return false; 
  }
}



//
// execute_binary_expression
//
// Given two values (both RAM_VALUE) and an operator (int value), performs the operation
// if working with one real and one int, convert the int to real so both are real
// run helper functions str_str, int_int_ and real_real for the different scenarios
// returns the result as a struct RAM_VALUE*
//

struct RAM_VALUE* execute_binary_expression(struct RAM_VALUE* lhs, int operator, struct RAM_VALUE* rhs, bool* success, struct STMT* stmt)
{
  assert(operator != OPERATOR_NO_OP);
  struct RAM_VALUE* result = malloc(sizeof(struct RAM_VALUE));
  
  if (lhs->value_type == RAM_TYPE_INT && rhs->value_type == RAM_TYPE_INT) {
    if (rhs->types.i == 0 && operator == OPERATOR_DIV) {
      printf("ZeroDivisionError: division by zero\n");
      *success = false;
    } else {
      result = execute_int_int_binary(lhs->types.i, operator, rhs->types.i);
      *success = true;
    }
  }

  else if ((lhs->value_type == RAM_TYPE_REAL && rhs->value_type == RAM_TYPE_REAL) || (lhs->value_type == RAM_TYPE_REAL && rhs->value_type == RAM_TYPE_INT) || (lhs->value_type == RAM_TYPE_INT && rhs->value_type == RAM_TYPE_REAL)) {
    double new_left;
    double new_right;
    if (lhs->value_type == RAM_TYPE_INT) {
      new_left = (double)lhs->types.i;
    } else {
      new_left = lhs->types.d;
    }
    if (rhs->value_type == RAM_TYPE_INT) {
      new_right = (double)rhs->types.i;
    } else {
      new_right = rhs->types.d;
    }
    if (new_right == 0.0 && operator == OPERATOR_DIV) {
      printf("ZeroDivisionError: division by zero\n");
      *success = false;
    } else {
      result = execute_real_real_binary(new_left, operator, new_right);
      *success = true;
    }
    //float ex_gets = execute_real_real_binary(new_left, operator, new_right); //DELETE LATER
    //printf("ex gets: %lf\n", ex_gets);  DELETE LATER
  }

  else if (lhs->value_type == RAM_TYPE_STR && rhs->value_type == RAM_TYPE_STR && operator == OPERATOR_PLUS) {
    result->value_type = RAM_TYPE_STR;
    size_t len = strlen(lhs->types.s) + strlen(rhs->types.s) + 1;
    result->types.s = malloc(len);
    strcpy(result->types.s,lhs->types.s);
    strcat(result->types.s,rhs->types.s);
    *success = true;
  }

  else if (lhs->value_type == RAM_TYPE_STR && rhs->value_type == RAM_TYPE_STR && is_rel_op(operator)) {
    result = execute_str_str_binary(lhs->types.s, operator, rhs->types.s);
    *success = true;
  }

  else {
    *success = false;
    printf("**SEMANTIC ERROR: invalid operand types (line %d)\n", stmt->line);
    return result;
  }

  

  

  //
  // perform the operation:
  //


  return result;
}

//is_zero
//takes in a string and returns true if the value is equal to 0 (comprised completely of 0s and up to one decimal point)
//returns false if not

bool is_zero(char* input_num) {
  int num_dec = 0;
  for (int i = 0; i<strlen(input_num); i++) {
    if (input_num[i] != '0' && input_num[i] != '.'){
      return false;
    }
    if (input_num[i] == '.') {
      num_dec++;
    }
    if (num_dec > 1) {
      printf("More than one decimal point");
      return false;
    }
  }
  return true;
}


//
// execute_assignment
//
// Executes an assignment statement, returning true if 
// successful and false if not (an error message will be
// output before false is returned, so the caller doesn't
// need to output anything).
// 
// Examples: x = 123
//           y = x ** 2
//

static bool execute_assignment(struct STMT* stmt, struct RAM* memory)
{
  struct STMT_ASSIGNMENT* assign = stmt->types.assignment;
  struct RAM_VALUE* result = malloc(sizeof(struct RAM_VALUE));
  bool success;

  char* var_name = assign->var_name;

  //
  // no pointers yet:
  //
  assert(assign->isPtrDeref == false);

  //
  // we only have expressions on the RHS, no function calls:
  //
  if (assign->rhs->value_type == VALUE_EXPR) {
    struct EXPR* expr = assign->rhs->types.expr;

    //
    // we always have a LHS:
    //
    assert(expr->lhs != NULL);


    struct RAM_VALUE* lhs_value = retrieve_value(expr->lhs->element, stmt, memory, &success);

    if (!success)  // semantic error? If so, return now:
      return false;

    //
    // do we have a binary expression?
    //
    if (!expr->isBinaryExpr) {  // no
      result = lhs_value;
    }
    else {
      //
      // binary expression such as x + y
      //
      assert(expr->operator != OPERATOR_NO_OP);  // we must have an operator

      struct RAM_VALUE* rhs_value = retrieve_value(expr->rhs->element, stmt, memory, &success);

      if (!success)  // semantic error? If so, return now:
        return false;

      //
      // perform the operation:
      //
      bool bin_success;
      result = execute_binary_expression(lhs_value, expr->operator, rhs_value, &bin_success, stmt);

      if (!bin_success)
        return false;
    }
    
  }

  else if (assign->rhs->value_type == VALUE_FUNCTION_CALL) {
    char* func_name = assign->rhs->types.function_call->function_name;
    char* param = assign->rhs->types.function_call->parameter->element_value;

    if (strcmp(func_name,"input") == 0) {
      printf("%s", param);
      char line[256];
      fgets(line, sizeof(line), stdin);

      //delete EOL chars from input:
      line[strcspn(line, "\r\n")] = '\0';

      result->value_type = RAM_TYPE_STR;
      result->types.s = line;
    } else if (strcmp(func_name, "int") == 0) {
      result->value_type = RAM_TYPE_INT;
      struct RAM_VALUE* var_value = ram_read_cell_by_name(memory, param);
      result->types.i = atoi(var_value->types.s); //didn't check if this works
      //if result->types.i == 0, check if the string is equal to 0, if not, return false
      if (result->types.i == 0 && !(is_zero(var_value->types.s))) {
        printf("**SEMANTIC ERROR: invalid string for int() (line %d)\n", stmt->line);
        return false;
      }
    } else if (strcmp(func_name, "float") == 0) {
      result->value_type = RAM_TYPE_REAL;
      struct RAM_VALUE* var_value = ram_read_cell_by_name(memory, param);
      result->types.d = atof(var_value->types.s); //didn't check if this works
      if (result->types.d == 0.0 && !(is_zero(var_value->types.s))) {
        printf("**SEMANTIC ERROR: invalid string for float() (line %d)\n", stmt->line);
        return false;
      }
    } else {
      printf("ERROR: invalid function call (line %d\n)", stmt->line);
      return false;
    }
  }

  

  //
  // write result to memory:
  //
  struct RAM_VALUE ram_value;

  ram_value = *result;

  success = ram_write_cell_by_name(memory, ram_value, var_name);

  return success;
}


//
// Public functions:
//

//
// execute
//
// Given a nuPython program graph and a memory, 
// executes the statements in the program graph
// If a semantic error occurs (e.g. type error),
// an error message is output, execution stops,
// and the function returns
//
void execute(struct STMT* program, struct RAM* memory)
{
  struct STMT* stmt = program;

  //
  // traverse through the program statements:
  //
  while (stmt != NULL) {

    if (stmt->stmt_type == STMT_ASSIGNMENT) {

      bool success = execute_assignment(stmt, memory);

      if (!success)
        return;

      stmt = stmt->types.assignment->next_stmt;  // advance
    }
    else if (stmt->stmt_type == STMT_FUNCTION_CALL) {


      if (!execute_function_call(stmt, memory)) {
        return;
      }

      stmt = stmt->types.function_call->next_stmt;
    } 
    else if (stmt->stmt_type == STMT_WHILE_LOOP) {
      struct EXPR* expr = stmt->types.while_loop->condition;
      struct RAM_VALUE* result = malloc(sizeof(struct RAM_VALUE));
      bool success;

      struct RAM_VALUE* lhs_value = retrieve_value(expr->lhs->element, stmt, memory, &success);
      if (!success)
        return;

      if (expr->isBinaryExpr) {  
        struct RAM_VALUE* rhs_value = retrieve_value(expr->rhs->element, stmt, memory, &success);
        if (!success)
          return;
        result = execute_binary_expression(lhs_value, expr->operator, rhs_value, &success, stmt);
        if (!success)
          return;
      }
      else {
        result = lhs_value;
      }

      if (result->types.i == 1) {
        stmt = stmt->types.while_loop->loop_body;
      } else {
        stmt = stmt->types.while_loop->next_stmt;
      }

    }
    else {
      assert(stmt->stmt_type == STMT_PASS);

      //
      // nothing to do!
      //

      stmt = stmt->types.pass->next_stmt;
    }
  }//while

  //
  // done:
  //
  return;
}
