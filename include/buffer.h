#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

#define BUFFER_SIZE_MAX 8


#define BUFFER_SUCCESS (0)
#define BUFFER_FULL    (1)
#define BUFFER_EMPTY   (2)
#define BUFFER_LOCKED  (4)

#define BUFFER_UNLOCKED (0)

typedef struct buffer_u8_t_struct {
  uint8_t buffer_vals[BUFFER_SIZE_MAX];
  volatile uint8_t index_w;
  volatile uint8_t index_r;
  volatile uint8_t free;
  volatile uint8_t lock;
} buffer_u8_t;

typedef uint8_t buffer_result_t;

void buffer_u8_init(buffer_u8_t *buf)
{
  buf->lock = BUFFER_LOCKED;
  buf->index_w = 0;
  buf->index_r = 0;
  buf->free = BUFFER_SIZE_MAX;
  for(uint8_t i = 0;i<BUFFER_SIZE_MAX;i++)
  {
    buf->buffer_vals[i] = 0;
  }
  buf->lock = BUFFER_UNLOCKED;
}

buffer_result_t buffer_u8_usage(buffer_u8_t *buf, uint8_t *usage)
{
  if(buf->lock)
    return BUFFER_LOCKED;
  buf->lock = BUFFER_LOCKED;
  *usage = BUFFER_SIZE_MAX - buf->free;
  buf->lock = BUFFER_UNLOCKED;
  return BUFFER_SUCCESS;
}

buffer_result_t buffer_u8_write(buffer_u8_t *buf, uint8_t val)
{
  /* At first the buffer needs to be checked if it is locked */
  if(buf->lock == BUFFER_LOCKED)
    return BUFFER_LOCKED;
  buf->lock = BUFFER_LOCKED;

  /* Check if buffer is full */
  uint8_t free = buf->free;
  if(free == 0)
  {
    buf->lock = BUFFER_UNLOCKED;
    return BUFFER_FULL;
  }

  /* Increase the write index, check for overflow and write to the buffer */
  uint8_t index_w = buf->index_w;
  index_w += 1;
  if(index_w >= BUFFER_SIZE_MAX)
    index_w = 0;
  buf->buffer_vals[index_w] = val;
  free -= 1;

  /* Write the temporary values back to the buffer */
  buf->index_w = index_w;
  buf->free = free;

  /* Finally, unlock the buffer and return success */
  buf->lock = BUFFER_UNLOCKED;
  return BUFFER_SUCCESS;
}

buffer_result_t buffer_u8_read(buffer_u8_t *buf, uint8_t *val)
{
  /* At first the buffer needs to be checked if it is locked */
  if(buf->lock == BUFFER_LOCKED)
    return BUFFER_LOCKED;
  buf->lock = BUFFER_LOCKED;

  /* Check if there is data in the buffer */
  uint8_t free = buf->free;
  if(free >= BUFFER_SIZE_MAX)
  {
    buf->lock = BUFFER_UNLOCKED;
    return BUFFER_EMPTY;
  }

  /* Increase the read index, check for overflow and read from the buffer */
  uint8_t index_r = buf->index_r;
  index_r += 1;
  if(index_r >= BUFFER_SIZE_MAX)
    index_r = 0;
  *val = buf->buffer_vals[index_r];
  free += 1;

  /* Write the temporary values back to the buffer */
  buf->index_r = index_r;
  buf->free = free;

  /* Finally, unlock the buffer and return success */
  buf->lock = BUFFER_UNLOCKED;
  return BUFFER_SUCCESS;
}

#endif //BUFFER_H
