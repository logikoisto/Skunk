#include "skunk/buffer.h"

#include <stdlib.h>

/**
 * the Buffer stack alloc limit
 */
const static size_t kMaxBufferStackAllocSize = 32;

const static size_t kShrinkBufferAllocSize = 16;

// move read pointer
#define COMPLETE_READ(size)                    \
  do {                                         \
    read_ptr_ = (read_ptr_ + size) % capcity_; \
    length_ -= size;                           \
  } while (0)

// move write pointer
#define COMPLETE_WRITE(size)                     \
  do {                                           \
    write_ptr_ = (write_ptr_ + size) % capcity_; \
    length_ += size;                             \
  } while (0)

zoo::skunk::Buffer::Buffer(size_t capcity)
    : capcity_(capcity), length_(0), read_ptr_(0), write_ptr_(0) {
  // TODO: the buffer in the next version of skunk will be alloced from the
  // arena module to make effective allocation
  if (capcity <= kMaxBufferStackAllocSize) {
    // alloc on stack need test maybe free at the end of this scope
    buf_ = (unsigned char *)alloca(capcity);
  } else {
    // alloc on heap
    buf_ = (unsigned char *)malloc(capcity);
  }
}

zoo::skunk::Buffer::~Buffer() {
  if (capcity_ > kMaxBufferStackAllocSize) {
    free(buf_);
  }
}

void zoo::skunk::Buffer::Reset() {
  if (capcity_ >= kMaxBufferStackAllocSize) {
    free(buf_);
  }
  buf_ = (unsigned char *)alloca(kShrinkBufferAllocSize);
  read_ptr_ = 0;
  write_ptr_ = 0;
  capcity_ = kShrinkBufferAllocSize;
  length_ = 0;
}

size_t zoo::skunk::Buffer::Readable() const { return length_; }

size_t zoo::skunk::Buffer::Writeable() const { return capcity_ - length_; }

size_t zoo::skunk::Buffer::Capcity() const { return capcity_; }

void zoo::skunk::Buffer::Seek(off_t offset, Whence whence) {
  switch (whence) {
    case Whence::Start: {
    }
    case Whence::Current: {
    }
    case Whence::End: {
    }
  }
}

size_t zoo::skunk::Buffer::Read(void *dest, size_t len) {
  COMPLETE_READ(1);
  return 0;
}

size_t zoo::skunk::Buffer::Read(zoo::skunk::Buffer &rhs, size_t len) {
  COMPLETE_READ(1);
  return 0;
}

size_t zoo::skunk::Buffer::Read(off_t offset, void *dest, size_t len) {
  COMPLETE_READ(1);
  return 0;
}

size_t zoo::skunk::Buffer::Read(off_t offset, zoo::skunk::Buffer &rhs,
                                size_t len) {
  COMPLETE_READ(1);
  return 0;
}

size_t zoo::skunk::Buffer::Read(size_t len, iovec *vectors,
                                size_t &vector_size) {
  COMPLETE_READ(1);
  return 0;
}

void zoo::skunk::Buffer::Write(zoo::skunk::Buffer &buffer) {}

void zoo::skunk::Buffer::Write(const void *src, size_t len) {}

void zoo::skunk::Buffer::Write(off_t offset, zoo::skunk::Buffer &buffer) {}

void zoo::skunk::Buffer::Write(off_t offset, const void *src, size_t len) {}

void zoo::skunk::Buffer::Write(Whence whence, const void *src, size_t len) {}

void zoo::skunk::Buffer::Write(iovec *vectors, size_t &vector_size) {}

// FIXME: should use the memmove to handle the logic ?
void zoo::skunk::Buffer::fit(size_t len) {}