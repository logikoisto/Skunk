#include <stdlib.h>

#include "buffer.h"

/**
 * the Buffer stack alloc limit 
 */
const size_t kMaxBufferStackAllocLen = 32;

zoo::skunk::Buffer::Buffer(size_t capcity): _capcity(capcity){
  if(capcity <= kMaxBufferStackAllocLen){
    // alloc on stack need test maybe free at the end of this scope 
    _buf = (unsigned char *)alloca(capcity);
  }else{
    // alloc on heap 
    _buf = (unsigned char *)malloc(capcity);
  }
}

zoo::skunk::Buffer::~Buffer(){
  if(_capcity > kMaxBufferStackAllocLen){
    free(_buf);
  }
}