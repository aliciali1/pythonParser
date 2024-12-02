/*ram.c*/

//
// ram.c implements the functions needed for the reading and writing of variables
//
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // true, false
#include <string.h>
#include <assert.h>

#include "ram.h"


//
// Public functions:
//

//
// ram_init
//
// Returns a pointer to a dynamically-allocated memory
// for storing nuPython variables and their values. All
// memory cells are initialized to the value None.
//
struct RAM* ram_init(void)
{
  struct RAM* memory = (struct RAM*)malloc(sizeof(struct RAM));
  memory->capacity = 4;
  memory->num_values = 0;
  memory->cells = (struct RAM_CELL*)malloc(memory->capacity * sizeof(struct RAM_CELL));

  for (int i = 0; i < memory->capacity; i++) {
    memory->cells[i].identifier = NULL;
    memory->cells[i].value.value_type = RAM_TYPE_NONE;
  }
  return memory;
}


//
// ram_destroy
//
// Frees the dynamically-allocated memory associated with
// the given memory. After the call returns, you cannot
// use the memory.
//
void ram_destroy(struct RAM* memory)
{
  for (int i = 0; i < memory->num_values; i++) {
    free(memory->cells[i].identifier);
    if (memory->cells[i].value.value_type==RAM_TYPE_STR){
      free(memory->cells[i].value.types.s);
    }
  }
  free(memory->cells);
  free(memory);
  return;
}


//
// ram_get_addr
// 
// If the given identifier (e.g. "x") has been written to 
// memory, returns the address of this value --- an integer
// in the range 0..N-1 where N is the number of values currently 
// stored in memory. Returns -1 if no such identifier exists 
// in memory. 
// 
// NOTE: a variable has to be written to memory before you can
// get its address. Once a variable is written to memory, its
// address never changes.
//
int ram_get_addr(struct RAM* memory, char* identifier)
{
  for (int i = 0; i < memory->num_values; i++) {
    if (strcmp(identifier, memory->cells[i].identifier) == 0) {
      return i;
    }
  }
  return -1;
}


//
// ram_read_cell_by_addr
//
// Given a memory address (an integer in the range 0..N-1), 
// returns a COPY of the value contained in that memory cell.
// Returns NULL if the address is not valid.
//
// NOTE: this function allocates memory for the value that
// is returned. The caller takes ownership of the copy and 
// must eventually free this memory via ram_free_value().
//
// NOTE: a variable has to be written to memory before its
// address becomes valid. Once a variable is written to memory,
// its address never changes.
//
struct RAM_VALUE* ram_read_cell_by_addr(struct RAM* memory, int address)
{
  if (address < memory->num_values && address >= 0) {
    struct RAM_VALUE* to_return = (struct RAM_VALUE*)malloc(sizeof(struct RAM_VALUE));
    if (memory->cells[address].value.value_type== RAM_TYPE_STR) {
      to_return->value_type = RAM_TYPE_STR;
      char* strcopy = (char *)malloc((strlen(memory->cells[address].value.types.s) + 1) * sizeof(char));
      strcpy(strcopy, memory->cells[address].value.types.s);
      to_return->types.s = strcopy;
    }
    else {
      *to_return = memory->cells[address].value;
    }
    return to_return;
  }
  

  return NULL;
}


// 
// ram_read_cell_by_name
//
// If the given name (e.g. "x") has been written to 
// memory, returns a COPY of the value contained in memory.
// Returns NULL if no such name exists in memory.
//
// NOTE: this function allocates memory for the value that
// is returned. The caller takes ownership of the copy and 
// must eventually free this memory via ram_free_value().
//
struct RAM_VALUE* ram_read_cell_by_name(struct RAM* memory, char* name)
{
  //get adrress for name
  //run read_cell_by_addr
  int addr = ram_get_addr(memory, name);
  if (addr != -1) {
    return ram_read_cell_by_addr(memory, addr);
  }
  return NULL;
}


//
// ram_free_value
//
// Frees the memory value returned by ram_read_cell_by_name and
// ram_read_cell_by_addr.
//
void ram_free_value(struct RAM_VALUE* value)
{
  if (value->value_type==RAM_TYPE_STR){
    free(value->types.s);
  }
  free(value);
  return;
}

//
// put_str_in_value
//
// takes a RAM_VALUE that holds a string, makes a copy of the string, and sets the value of the given cell of memory to that string
// returns nothing

void put_str_in_value(struct RAM* memory, struct RAM_VALUE value, int i) {
  char* strcopy = (char *)malloc((strlen(value.types.s) + 1) * sizeof(char));
  strcpy(strcopy, value.types.s);
  memory->cells[i].value.value_type = RAM_TYPE_STR;
  memory->cells[i].value.types.s = strcopy;
  return;
}

//
// ram_write_cell_by_addr
//
// Writes the given value to the memory cell at the given 
// address. If a value already exists at this address, that
// value is overwritten by this new value. Returns true if 
// the value was successfully written, false if not (which 
// implies the memory address is invalid).
// 
// NOTE: if the value being written is a string, it will
// be duplicated and stored.
// 
// NOTE: a variable has to be written to memory before its
// address becomes valid. Once a variable is written to memory,
// its address never changes.
//
bool ram_write_cell_by_addr(struct RAM* memory, struct RAM_VALUE value, int address)
{
  //if the address exists

  //if it is a string
  if (address<memory->num_values && address>=0) {
    if(memory->cells[address].value.value_type == RAM_TYPE_STR) {
      free(memory->cells[address].value.types.s);
    }
    if (value.value_type == RAM_TYPE_STR) {
      put_str_in_value(memory, value, address);
    }
    else {
        memory->cells[address].value = value;
    }
    return true;
  }

  //if the address doesn't exist
  return false;
}


//
// ram_write_cell_by_name
//
// Writes the given value to a memory cell named by the given
// name. If a memory cell already exists with this name, the
// existing value is overwritten by the given value. Returns
// true since this operation always succeeds.
// 
// NOTE: if the value being written is a string, it will
// be duplicated and stored.
// 
// NOTE: a variable has to be written to memory before its
// address becomes valid. Once a variable is written to memory,
// its address never changes.
//



bool ram_write_cell_by_name(struct RAM* memory, struct RAM_VALUE value, char* name)
{
  bool existed = false;
  for (int i = 0; i < memory->num_values; i++) {
    if (strcmp(name, memory->cells[i].identifier) == 0) {
      // if orginal value was string, must free
      if(memory->cells[i].value.value_type == RAM_TYPE_STR) {
        free(memory->cells[i].value.types.s);
      }
      if (value.value_type == RAM_TYPE_STR) {
        put_str_in_value(memory, value, i);
      }
      else {
        memory->cells[i].value = value;
      }
      existed = true;
    }
  }
  if (!existed) {
    //resize if not big enough
    if (memory->capacity == memory->num_values) {
      int new_capacity = memory->capacity*2; 
      struct RAM_CELL* new_array = (struct RAM_CELL*)malloc(new_capacity * sizeof(struct RAM_CELL));
      for (int i = 0; i < memory->capacity; i++) {
        new_array[i] = memory->cells[i];
      }
      for (int i = memory->capacity; i < new_capacity; i++) {
        new_array[i].identifier = NULL;
        new_array[i].value.value_type = RAM_TYPE_NONE;
      }
      
      free(memory->cells);
      memory->cells = new_array;
      memory->capacity = new_capacity;
    }
    //write cell by name
    //memory->cells[memory->num_values].identifier = name;
    memory->cells[memory->num_values].identifier = (char*)malloc(sizeof(char)*(strlen(name)+1));
    strcpy(memory->cells[memory->num_values].identifier, name);
    if (value.value_type == RAM_TYPE_STR) {
      put_str_in_value(memory, value, memory->num_values);
    }
    else {
      memory->cells[memory->num_values].value = value;
    }
    memory->num_values++;
  }
  return true;
}


//
// ram_print
//
// Prints the contents of memory to the console.
//
void ram_print(struct RAM* memory)
{
  printf("**MEMORY PRINT**\n");

  printf("Capacity: %d\n", memory->capacity);
  printf("Num values: %d\n", memory->num_values);
  printf("Contents:\n");

  for (int i = 0; i < memory->capacity; i++)
    {
      printf(" %d: %s, ", i, memory->cells[i].identifier);

      switch(memory->cells[i].value.value_type) {
        case RAM_TYPE_INT:
          printf("int, %d", memory->cells[i].value.types.i);
          break;
        case RAM_TYPE_REAL:
          printf("real, %lf", memory->cells[i].value.types.d);
          break;
        case RAM_TYPE_STR:
          printf("str, '%s'", memory->cells[i].value.types.s);
          break;
        case RAM_TYPE_PTR:
          printf("ptr, %d", memory->cells[i].value.types.i);
          break;
        case RAM_TYPE_BOOLEAN: 
          if (memory->cells[i].value.types.i == 0) {
            printf("boolean, False");
          } else {
            printf("boolean, True");
          }
          break;
        case RAM_TYPE_NONE:
          printf("none, None");
          break;
      }

      printf("\n");
    }

  printf("**END PRINT**\n");
}
