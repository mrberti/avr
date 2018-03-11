#ifndef _BUFFER_H
#define _BUFFER_H

#include "global.h"

#include <stdint.h>

typedef uint8_t buffer_result_t;

/* Return values > 0 are errors */
#define BUFFER_SUCCESS  (0)
#define BUFFER_LOCKED   (1<<1)
#define BUFFER_FULL     (1<<2)
#define BUFFER_EMPTY    (1<<3)

#define BUFFER_FLAG_LOCK   (1<<0)

typedef uint8_t buffer_index_t;

typedef struct buffer_s {
  void *data;
  volatile buffer_index_t index_r;
  volatile buffer_index_t index_w;
  volatile buffer_index_t index_max;
  volatile buffer_index_t used;
  volatile uint8_t flags;
  void (*write_func)(void *data, buffer_index_t index, void *val);
  void (*read_func)(void *data, buffer_index_t index, void *val);
} buffer_t;

/*
 * FUNCTION DECLARATIONS
 */
#define write_type(type) void write_##type(void *data, buffer_index_t index, void *val)
#define read_type(type) void read_##type(void *data, buffer_index_t index, void *val)
buffer_result_t buffer_write(buffer_t *buf, void *val);
buffer_result_t buffer_read(buffer_t *buf, void *val);
/* The following functions define type specific read and write access */
write_type(char);
write_type(int);
write_type(long);
read_type(char);
read_type(int);
read_type(long);

#define BUFFER_DECLARE(buf_name,data_type,buf_size) \
	typedef data_type buf_name##_type_t; \
	buf_name##_type_t buf_name##_data[buf_size]; \
	buffer_t buf_name

#define BUFFER_DECLARE_EXTERN(buf_name,data_type,buf_size) \
	extern typedef data_type buf_name##_type_t; \
	extern buf_name##_type_t buf_name##_data[buf_size]; \
	extern buffer_t buf_name

#define BUFFER_INIT(buf_name,data_type,buf_size) \
  buf_name.data = buf_name##_data; \
  buf_name.index_r = 0; \
  buf_name.index_w = 0; \
  buf_name.index_max = buf_size-1; \
  buf_name.used = 0; \
  buf_name.flags = 0; \
  buf_name.write_func = write_##data_type; \
  buf_name.read_func = read_##data_type

/* The all in macro. works only within function scope */
#define BUFFER_DECLARE_AND_INIT(buf_name,data_type,buf_size) \
  BUFFER_DECLARE(buf_name,data_type,buf_size); \
  BUFFER_INIT(buf_name,data_type,buf_size)

#endif
