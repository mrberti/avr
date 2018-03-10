#ifndef BUFFER_H
#define BUFFER_H

#include "global.h"

#include <stdint.h>

#define BUFFER_SIZE_MAX 8
#if BUFFER_SIZE_MAX > 255
# error "Currently maximum 255 buffer values supported."
#endif

typedef uint8_t buffer_result_t;

/* Return values > 0 are errors */
#define BUFFER_SUCCESS  (0)
#define BUFFER_UNLOCKED (0)
#define BUFFER_FULL     (1)
#define BUFFER_EMPTY    (2)
#define BUFFER_LOCKED   (4)

typedef struct buffer_u8_t_struct {
  uint8_t buffer_vals[BUFFER_SIZE_MAX];
  volatile uint8_t index_w;
  volatile uint8_t index_r;
  volatile uint8_t free;
  volatile uint8_t lock;
} buffer_u8_t;

typedef struct buffer_u16_t_struct {
  uint16_t buffer_vals[BUFFER_SIZE_MAX];
  volatile uint8_t index_w;
  volatile uint8_t index_r;
  volatile uint8_t free;
  volatile uint8_t lock;
} buffer_u16_t;

#ifdef __cplusplus
extern "C" {
#endif

/* wrapper functions for uint8_t buffer */
void buffer_u8_init(buffer_u8_t *);
buffer_result_t buffer_u8_usage(buffer_u8_t *buf, uint8_t *usage);
buffer_result_t buffer_u8_write(buffer_u8_t *buf, uint8_t val);
buffer_result_t buffer_u8_read(buffer_u8_t *buf, uint8_t *val);

/* wrapper functions for uint16_t buffer */
void buffer_u16_init(buffer_u16_t *);
buffer_result_t buffer_u16_usage(buffer_u16_t *buf, uint8_t *usage);
buffer_result_t buffer_u16_write(buffer_u16_t *buf, uint16_t val);
buffer_result_t buffer_u16_read(buffer_u16_t *buf, uint16_t *val);

#ifdef __cplusplus
}
#endif

#endif //BUFFER_H
