#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Aumentar capacidad inicial si se esta relentizando mucho por andar haciendo
// malloc a cada rato
#define ARRAY_CAPACITY 512
#define array(T) array_init(sizeof(T), ARRAY_CAPACITY)
#define head(array) (((Array_Header *)array) - 1)
#define len(array) ((head(array))->length)
#define cap(array) ((Array_Header *)(array)->capacity)

#define append(a, v)                                                           \
  ((a) = array_ensure_capacity(a, 1, sizeof(*a)), (a)[head(a)->length] = (v),  \
   &(a)[head(a)->length++])

#define remove_at(array, idx)                                                  \
  do {                                                                         \
    Array_Header *header = head(array);                                        \
    int last_idx = header->length - 1;                                         \
    if (last_idx == idx) {                                                     \
      header->length--;                                                        \
    } else if (header->length > 1) {                                           \
      void *free_space = &array[idx];                                          \
      void *last = &array[last_idx];                                           \
      memcpy(free_space, last, sizeof(*array));                                \
      header->length--;                                                        \
    }                                                                          \
  } while (0);

#define pop_back(array) ((array)[head(array)->length])

typedef struct {
  size_t length;
  size_t capacity;
  size_t padding;
} Array_Header;

void *array_init(size_t item_size, size_t capacity) {
  void *ptr = 0;
  size_t size = sizeof(Array_Header) + item_size * capacity;
  Array_Header *header = malloc(size);
  if (!header)
    return NULL;
  header->capacity = capacity;
  header->length = 0;
  ptr = header + 1;
  return ptr;
}

void *array_ensure_capacity(void *array, size_t item_count, size_t item_size) {
  Array_Header *header = head(array);

  if (!header)
    return NULL;

  size_t desired_capacity = header->length + item_count;

  if (header->capacity > desired_capacity) {
    return header + 1;
  }

  size_t new_capacity = header->capacity * 2;
  while (new_capacity < header->capacity)
    new_capacity = header->capacity * 2;
  size_t old_size = sizeof(*header) + header->length * item_size;
  size_t new_size = old_size + item_count * item_size;

  Array_Header *reallocated_header = malloc(new_size);

  if (!reallocated_header)
    return NULL;

  memcpy(reallocated_header, header, old_size);
  free(header);
  return reallocated_header + 1;
}
