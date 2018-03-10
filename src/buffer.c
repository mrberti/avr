#include "buffer.h"
#include "uart.h"

write_type(char)
{
  *(char*)(data+index*sizeof(char)) = *(char*)val;
}

write_type(int)
{
  *(int*)(data+index*sizeof(int)) = *(int*)val;
}

write_type(long)
{
  *(long*)(data+index*sizeof(long)) = *(long*)val;
}

read_type(char)
{
  *(char*)val = *(char*)(data+index*sizeof(char));
}

read_type(int)
{
  *(int*)val = *(int*)(data+index*sizeof(int));
}

read_type(long)
{
  *(long*)val = *(long*)(data+index*sizeof(long));
}

void buffer_write(buffer_t *buf, void *val)
{
  buf->write_func(buf->data, buf->index_w, val);

  if(buf->index_w < buf->index_max)
    buf->index_w += 1;
  else
    buf->index_w = 0;
}

void buffer_read(buffer_t *buf, void *val)
{
  buf->read_func(buf->data, buf->index_r, val);

  if(buf->index_r < buf->index_max)
    buf->index_r += 1;
  else
    buf->index_r = 0;
}
