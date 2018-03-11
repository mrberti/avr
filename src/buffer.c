#include "buffer.h"

#if UART_DEBUG_LEVEL > UART_DEBUG_LEVEL_NONE
# include "uart.h"
# include "timer.h"
#endif

inline write_type(char)
{
  *(char*)(data+index*sizeof(char)) = *(char*)val;
}

inline write_type(int)
{
  *(int*)(data+index*sizeof(int)) = *(int*)val;
}

inline write_type(long)
{
  *(long*)(data+index*sizeof(long)) = *(long*)val;
}

inline read_type(char)
{
  *(char*)val = *(char*)(data+index*sizeof(char));
}

inline read_type(int)
{
  *(int*)val = *(int*)(data+index*sizeof(int));
}

inline read_type(long)
{
  *(long*)val = *(long*)(data+index*sizeof(long));
}

buffer_result_t buffer_write(buffer_t *buf, void *val)
{
  /* At first it must be checked if the buffer is locked */
  if(buf->flags & BUFFER_FLAG_LOCK)
  {
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_WARNING
    UART_puts("\n\r");
    UART_putd_32(timer0_us_since_start);
    UART_puts(" W: BUFFER_LOCKED");
#endif
    return BUFFER_LOCKED;
  }
  buf->flags |= BUFFER_FLAG_LOCK;

  /* Check if there is enough space in the buffer. Otherwise release the lock and
  return */
  buffer_index_t index_max = buf->index_max;
  buffer_index_t used = buf->used;
  if(used > index_max)
  {
    buf->flags &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_WARNING
    UART_puts("\n\r");
    UART_putd_32(timer0_us_since_start);
    UART_puts(" W: BUFFER_FULL");
#endif
    return BUFFER_FULL;
  }

  /* Increase the write index, check for overflow and write to the buffer */
  buffer_index_t index_w = buf->index_w;
  if(index_w < index_max)
    index_w += 1;
  else
    index_w = 0;
  buf->write_func(buf->data, index_w, val);
  used += 1;

  /* Finally, write the temporary values back to the buffer, unlock the buffer
  and return success */
  buf->index_w = index_w;
  buf->used    = used;
  buf->flags  &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_NOTICE
    UART_puts("\n\r");
    UART_putd_32(timer0_us_since_start);
    UART_puts(" W: OK. used: ");
    UART_putd(used);
    UART_puts("/");
    UART_putd(index_max+1);
    UART_puts(" index_w: ");
    UART_putd(index_w);
#endif
  return BUFFER_SUCCESS;
}

buffer_result_t buffer_read(buffer_t *buf, void *val)
{
  /* At first it must be checked if the buffer is locked */
  if(buf->flags & BUFFER_FLAG_LOCK)
  {
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_WARNING
    UART_puts("\n\r");
    UART_putd_32(timer0_us_since_start);
    UART_puts(" R: BUFFER_LOCKED");
#endif
    return BUFFER_LOCKED;
  }
  buf->flags |= BUFFER_FLAG_LOCK;

  /* Check if there is data in the buffer. Otherwise release the lock and
  return */
  buffer_index_t used = buf->used;
  if(used == 0)
  {
    buf->flags &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_WARNING
    UART_puts("\n\r");
    UART_putd_32(timer0_us_since_start);
    UART_puts(" R: BUFFER_EMPTY");
#endif
    return BUFFER_EMPTY;
  }

  /* Increase the read index, check for overflow and read from the buffer */
  buffer_index_t index_r = buf->index_r;
  buffer_index_t index_max = buf->index_max;
  if(index_r <  index_max)
    index_r += 1;
  else
    index_r = 0;
  buf->read_func(buf->data, index_r, val);
  used -= 1;

  /* Finally, write the temporary values back to the bufferunlock the buffer
  and return success */
  buf->index_r = index_r;
  buf->used    = used;
  buf->flags  &= ~BUFFER_FLAG_LOCK;
#if UART_DEBUG_LEVEL >= UART_DEBUG_LEVEL_NOTICE
    UART_puts("\n\r");
    UART_putd_32(timer0_us_since_start);
    UART_puts(" R: OK. used: ");
    UART_putd(used);
    UART_puts("/");
    UART_putd(index_max+1);
    UART_puts(" index_r: ");
    UART_putd(index_r);
#endif
  return BUFFER_SUCCESS;
}
