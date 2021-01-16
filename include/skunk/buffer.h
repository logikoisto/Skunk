#ifndef __SKUNK_BUFFER_H__
#define __SKUNK_BUFFER_H__

#include <sys/types.h>
#include <sys/uio.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace zoo {
namespace skunk {
/**
 * Buffer implement the I/O buffer primitives
 * use the simple ring buffer struct
 */
class Buffer {
 public:
  /**
   * Whence move behaviors used by Seek
   */
  enum Whence {
    Start,
    Current,
    End,
  };

 public:
  /**
   * Buffer common constructor
   *
   * if the capcity less than 32, the buffer will allocate the space on stack
   * else alloc on heap, when destruct the Buffer the Buffer should know how
   * to free the memeory space
   */
  explicit Buffer(size_t capcity = 16);

  ~Buffer();

 private:
  Buffer(const Buffer& rhs);
  Buffer& operator=(const Buffer& rhs);

 public:
  /**
   * Seek the read pointer to the destination with offset from the position
   * at the protocol layer the protocol could use Read and Write seilize
   * methods if the protocol not
   */
  void Seek(off_t offset, Whence whence = Current);

  /**
   * Readable return the readable size in this buffer this method maybe
   * more useful in the protocol design and implementation
   */
  size_t Readable() const;

  /**
   * Writeable return the writeable size in this buffer, same usage as
   * Readable
   */
  size_t Writeable() const;

  /**
   * Reset the Buffer move the read and write pointer to start position
   * but the undelying buffer is not freed. In network application the
   * Buffer may get new content with the same size or large size than
   * last use.
   */
  void Reset();

  /**
   * Capcity return the undelying buffer size
   */
  size_t Capcity() const;

 public:
  /**
   * Buffer read overload functors
   */
  size_t Read(void* dest, size_t len);
  size_t Read(Buffer& rhs, size_t len);
  size_t Read(off_t offset, void* dest, size_t len);
  size_t Read(off_t offset, Buffer& rhs, size_t len);

  /**
   * Read template functions
   * only use for read the primitive type such as int unsigned int or more
   * the user define structs could not use this logic
   */
  template <class T>
  size_t Read(T& val) {
    return Read(&val, sizeof(val));
  }

  template <class T>
  size_t Read(off_t offset, T& val) {
    return Read(offset, &val, sizeof(val));
  }

  /**
   * Extract buffer from the undelying buffer in io vector
   * input vectors and will create many vectors by this function
   * also return the vector size
   */
  size_t Read(size_t len, iovec* vectors, size_t& vector_size);

  /**
   * Buffer write overload functors
   */
  void Write(Buffer& buffer);
  void Write(const void* src, size_t len);
  void Write(off_t offset, Buffer& buffer);
  void Write(off_t offset, const void* src, size_t len);
  void Write(Whence whence, const void* src, size_t len);

  /**
   * Write template functions
   * only use for write the primitive types such as int unsigned int
   * see also read template functions
   */
  template <class T>
  void Write(const T& val) {
    Write(&val, sizeof(val));
  }

  template <class T>
  void Write(off_t offset, const T& val) {
    Write(offset, &val, sizeof(val));
  }

  void Write(const char* const val) { Write(val, strlen(val)); }

  void Write(off_t offset, const char* const val) {
    Write(offset, val, strlen(val));
  }

  /**
   * write from the iovec
   */
  void Write(iovec* vectors, size_t& vector_size);

 private:
  unsigned char* buf_;  // underlying buffer
  size_t capcity_;      // capcity of this Buffer
  size_t length_;       // data length in the Buffer

  // RingBuffer structure specifics
  off_t read_ptr_;   // read pointer
  off_t write_ptr_;  // write pointer

  // private functions

  /**
   * fit the size when the capcity changed
   */
  void fit(size_t len);

  // debug or test functions

#ifdef DEBUG
 public:
  unsigned char* GetUnderlyingBuffer() { return buf_; }
  void SetUnderlyingBuffer(unsigned char* buf) { buf_ = buf; }
  void DebugFitFunc(size_t len) { fit(len); }
  off_t GetReadPointer() { return read_ptr_; }
  off_t GetWritePointer() { return write_ptr_; }
#endif  // DEBUG
};

extern const Buffer kNullBuffer;

}  // namespace skunk

}  // namespace zoo

#endif  // !__SKUNK_BUFFER_H__