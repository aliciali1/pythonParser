/*tests.c*/

//
// tests.c contains tests to test the functions in ram.h
//
// Alicia Li
//
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ram.h"
#include "gtest/gtest.h"

//
// private helper functions:
//

//
// Test case: writing one integer value
//
TEST(memory_module, write_one_int) 
{
  //
  // create a new memory:
  //
  struct RAM* memory = ram_init();

  ASSERT_TRUE(memory != NULL);
  ASSERT_TRUE(memory->cells != NULL);

  ASSERT_EQ(memory->num_values, 0);
  ASSERT_EQ(memory->capacity, 4);

  //
  // we want to store the integer 123:
  //
  struct RAM_VALUE i;

  i.value_type = RAM_TYPE_INT;
  i.types.i = 123;

  bool success = ram_write_cell_by_name(memory, i, "x");
  ASSERT_TRUE(success);

  //
  // check the memory, does it contain x = 123?
  //
  ASSERT_EQ(memory->num_values, 1);
  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].value.types.i, 123);
  ASSERT_STREQ(memory->cells[0].identifier, "x");

  //
  // done test, free memory
  //
  ram_destroy(memory);
}


TEST(memory_module, write_seven_var) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;
  struct RAM_VALUE b;
  struct RAM_VALUE c;
  struct RAM_VALUE d;
  struct RAM_VALUE e;
  struct RAM_VALUE f;
  struct RAM_VALUE g;


  a.value_type = RAM_TYPE_INT;
  a.types.i = 12;

  b.value_type = RAM_TYPE_INT;
  b.types.i = -18;

  c.value_type = RAM_TYPE_REAL;
  c.types.d = 5.2;

  d.value_type = RAM_TYPE_BOOLEAN;
  d.types.i = 1;

  e.value_type = RAM_TYPE_REAL;
  e.types.d = -3.8;

  f.value_type = RAM_TYPE_STR;
  f.types.s = "hello";

  g.value_type = RAM_TYPE_REAL;
  g.types.d = 7.21242;

  

  bool success = ram_write_cell_by_name(memory, a, "a");
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 1);
  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[0].value.types.i, 12);
  ASSERT_STREQ(memory->cells[0].identifier, "a");

  success = ram_write_cell_by_name(memory, b, "b");
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 2);
  ASSERT_EQ(memory->cells[1].value.value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[1].value.types.i, -18);

  success = ram_write_cell_by_name(memory, c, "c");
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 3);
  ASSERT_EQ(memory->cells[2].value.value_type, RAM_TYPE_REAL);
  ASSERT_EQ(memory->cells[2].value.types.d, 5.2);

  success = ram_write_cell_by_name(memory, d, "d");
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 4);
  ASSERT_EQ(memory->cells[3].value.value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(memory->cells[3].value.types.i, 1);

  success = ram_write_cell_by_name(memory, e, "e");
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 5);
  ASSERT_EQ(memory->capacity, 8);
  ASSERT_EQ(memory->cells[4].value.value_type, RAM_TYPE_REAL);
  ASSERT_EQ(memory->cells[4].value.types.d, -3.8);

  success = ram_write_cell_by_name(memory, f, "f");
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 6);
  ASSERT_EQ(memory->capacity, 8);
  ASSERT_EQ(memory->cells[5].value.value_type, RAM_TYPE_STR);
  //printf(memory->cells[5].value.types.s);
  ASSERT_EQ(strcmp(memory->cells[5].value.types.s, "hello"), 0);
  
  success = ram_write_cell_by_name(memory, a, "f");
  ASSERT_EQ(memory->num_values, 6);
  ASSERT_EQ(memory->capacity, 8);
  ASSERT_EQ(memory->cells[5].value.value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->cells[5].value.types.i, 12);
  ASSERT_STREQ(memory->cells[5].identifier, "f");

  success = ram_write_cell_by_name(memory, d, "c");
  ASSERT_EQ(memory->num_values, 6);
  ASSERT_EQ(memory->capacity, 8);
  ASSERT_EQ(memory->cells[2].value.value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(memory->cells[2].value.types.i, 1);
  ASSERT_STREQ(memory->cells[2].identifier, "c");

  success = ram_write_cell_by_addr(memory, d, 1);
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 6);
  ASSERT_EQ(memory->cells[1].value.value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(memory->cells[1].value.types.i, 1);

  success = ram_write_cell_by_addr(memory, g, 3);
  ASSERT_TRUE(success);
  ASSERT_EQ(memory->num_values, 6);
  ASSERT_EQ(memory->capacity, 8);
  ASSERT_EQ(memory->cells[3].value.value_type, RAM_TYPE_REAL);
  ASSERT_EQ(memory->cells[3].value.types.d, 7.21242);

  success = ram_write_cell_by_addr(memory, c, 6);
  ASSERT_FALSE(success);

  ASSERT_EQ(ram_get_addr(memory, "e"), 4);
  ASSERT_EQ(ram_get_addr(memory, "a"), 0);
  ASSERT_EQ(ram_get_addr(memory, "h"), -1);

  struct RAM_VALUE* x = ram_read_cell_by_addr(memory, 0);
  ASSERT_EQ(x->types.i, 12);
  ASSERT_EQ(x->value_type, RAM_TYPE_INT);

  struct RAM_VALUE* y = ram_read_cell_by_name(memory, "c");
  ASSERT_EQ(y->value_type, RAM_TYPE_BOOLEAN);
  ASSERT_EQ(y->types.i, 1);
  

  ram_destroy(memory);
  ram_free_value(x);
  ram_free_value(y);
}

TEST(memory_module, test_read_cell_by_addr) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;
  struct RAM_VALUE b;
  struct RAM_VALUE c;
  struct RAM_VALUE d;
  struct RAM_VALUE e;

  a.value_type = RAM_TYPE_INT;
  a.types.i = 12;

  b.value_type = RAM_TYPE_STR;
  b.types.s = "testing";

  c.value_type = RAM_TYPE_STR;
  c.types.s = "see";

  d.value_type = RAM_TYPE_REAL;
  d.types.d = 124089.123;

  e.value_type = RAM_TYPE_NONE;

  bool success = ram_write_cell_by_name(memory, a, "a");
  ASSERT_TRUE(success);

  success = ram_write_cell_by_name(memory, b, "b");
  ASSERT_TRUE(success);

  success = ram_write_cell_by_name(memory, c, "c");
  ASSERT_TRUE(success);

  success = ram_write_cell_by_addr(memory, a, 3);
  ASSERT_FALSE(success);
  ASSERT_EQ(memory->num_values, 3);

  success = ram_write_cell_by_name(memory, d, "d");
  ASSERT_TRUE(success);

  ASSERT_EQ(memory->num_values, 4);
  ASSERT_EQ(memory->capacity, 4);

  success = ram_write_cell_by_addr(memory, a, 3);
  ASSERT_EQ(memory->cells[3].value.types.i, 12);
  ASSERT_EQ(memory->cells[3].value.value_type, RAM_TYPE_INT);
  ASSERT_EQ(memory->num_values, 4);
  ASSERT_EQ(memory->capacity, 4);

  success = ram_write_cell_by_addr(memory, e, 1);
  ASSERT_EQ(memory->cells[1].value.value_type, RAM_TYPE_NONE);
  ASSERT_EQ(memory->num_values, 4);
  ASSERT_EQ(memory->capacity, 4);

  success = ram_write_cell_by_addr(memory, a, 1);
  ASSERT_EQ(memory->num_values, 4);



  struct RAM_VALUE* x = ram_read_cell_by_addr(memory, 3);
  ASSERT_EQ(x->types.i, 12);
  ASSERT_EQ(x->value_type, RAM_TYPE_INT); 

  ram_destroy(memory);
  ram_free_value(x);

}


TEST(memory_mode, string_tests) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;

  a.value_type = RAM_TYPE_STR;
  a.types.s = "testing";

  bool success = ram_write_cell_by_name(memory, a, "a");
  ASSERT_TRUE(success);
  ASSERT_EQ(strcmp(memory->cells[0].value.types.s, "testing"), 0);

  ASSERT_TRUE(a.types.s != memory->cells[0].value.types.s);

  ram_destroy(memory);

}

TEST(memory_mode, expanding_memory) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;
  struct RAM_VALUE b;

  a.value_type = RAM_TYPE_INT;
  a.types.i = 98123;

  b.value_type = RAM_TYPE_INT;
  b.types.i = 15;

  ram_write_cell_by_name(memory, a, "a");
  ram_write_cell_by_name(memory, a, "b");
  ram_write_cell_by_name(memory, a, "c");
  ram_write_cell_by_name(memory, a, "d");
  ASSERT_EQ(memory->capacity, 4);
  ASSERT_EQ(memory->num_values, 4);
  ram_write_cell_by_name(memory, a, "f");
  ram_write_cell_by_name(memory, a, "g");
  ram_write_cell_by_name(memory, a, "h");
  ram_write_cell_by_name(memory, a, "i");
  ASSERT_EQ(memory->capacity, 8);
  ASSERT_EQ(memory->num_values, 8);
  ram_write_cell_by_name(memory, b, "j");
  ASSERT_EQ(memory->capacity, 16);
  ASSERT_EQ(memory->num_values, 9);

  struct RAM_VALUE* x = ram_read_cell_by_addr(memory, 8);
  ASSERT_EQ(x->types.i, 15);
  ASSERT_EQ(x->value_type, RAM_TYPE_INT); 

  struct RAM_VALUE* y = ram_read_cell_by_name(memory, "j");
  ASSERT_EQ(y->types.i, 15);
  ASSERT_EQ(y->value_type, RAM_TYPE_INT); 


  ram_destroy(memory);
  ram_free_value(x);
  ram_free_value(y);
}

TEST(memory_module, read_invalid_addr) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;

  a.value_type = RAM_TYPE_INT;
  a.types.i = 98123;

  ram_write_cell_by_name(memory, a, "a");
  struct RAM_VALUE* x = ram_read_cell_by_addr(memory, 1);
  ASSERT_TRUE(x == NULL);

  ram_destroy(memory);
}

TEST(memory_module, read_invalid_mame) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;

  a.value_type = RAM_TYPE_INT;
  a.types.i = 98123;

  ram_write_cell_by_name(memory, a, "a");
  struct RAM_VALUE* x = ram_read_cell_by_name(memory, "b");
  ASSERT_TRUE(x == NULL);

  ram_destroy(memory);

}

TEST(memory_module, write_by_address) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;
  struct RAM_VALUE b;
  //struct RAM_VALUE c;

  a.value_type = RAM_TYPE_BOOLEAN;
  a.types.i = 0;

  b.value_type = RAM_TYPE_REAL;
  b.types.d = 7.21242;

  //c.value_type = RAM_TYPE_STR;
  //c.types.s = "testing testing";

  ram_write_cell_by_name(memory, a, "one");
  ASSERT_EQ(memory->cells[0].value.types.i, 0);
  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_BOOLEAN);

  ram_write_cell_by_addr(memory, b, 0);
  ASSERT_EQ(memory->cells[0].value.types.d, 7.21242);
  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_REAL);

  ram_destroy(memory);
}

TEST(memory_module, get_invalid_address) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;

  a.value_type = RAM_TYPE_INT;
  a.types.i = 98123;

  ram_write_cell_by_name(memory, a, "a");
  ASSERT_EQ(ram_get_addr(memory, "b"), -1);

  ram_destroy(memory);
}

TEST(memory_module, overwrite_string) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;
  struct RAM_VALUE b;

  a.value_type = RAM_TYPE_STR;
  a.types.s = "testing";

  b.value_type = RAM_TYPE_STR;
  b.types.s = "see";

  ram_write_cell_by_name(memory, a, "a");
  ram_write_cell_by_addr(memory, b, 0);

  ASSERT_EQ(strcmp("see", memory->cells[0].value.types.s), 0);
  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_STR);

  ram_destroy(memory);
}

TEST(memory_module, overwrite_string_w_int) {
  struct RAM* memory = ram_init();
  struct RAM_VALUE a;
  struct RAM_VALUE b;

  a.value_type = RAM_TYPE_STR;
  a.types.s = "testing";

  b.value_type = RAM_TYPE_INT;
  b.types.i = 0;

  ram_write_cell_by_name(memory, a, "a");
  ram_write_cell_by_addr(memory, b, 0);

  ASSERT_EQ(memory->cells[0].value.types.i, 0);
  ASSERT_EQ(memory->cells[0].value.value_type, RAM_TYPE_INT);

  ram_destroy(memory);
}

