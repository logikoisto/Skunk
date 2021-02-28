#include "skunk/buffer.h"

#include <stdint.h>
#include <sys/types.h>

#include <string>

#include "gtest/gtest.h"

#ifndef DEBUG
#define DEBUG
#endif  // !DEBUG

// test the buffer constructor
TEST(BufferTest, BufferBuild) {
  zoo::skunk::Buffer buffer;
  ASSERT_EQ(buffer.Capcity(), 16);
  ASSERT_EQ(buffer.Readable(), 0);
  ASSERT_EQ(buffer.Writeable(), 16);
  buffer.Write('a');
  ASSERT_EQ(buffer.Capcity(), 16);
  ASSERT_EQ(buffer.Readable(), 1);
  ASSERT_EQ(buffer.Writeable(), 15);
  buffer.Reset();
  ASSERT_EQ(buffer.Capcity(), 16);
  ASSERT_EQ(buffer.Readable(), 0);
  ASSERT_EQ(buffer.Writeable(), 16);
}

TEST(BufferTest, ReadWritePrimitives) {
  zoo::skunk::Buffer buffer;

  char c = 0x01;
  unsigned char uc = 0x02;
  int16_t i16 = INT16_MAX;
  int32_t i32 = INT32_MAX;
  int64_t i64 = INT64_MAX;
  uint16_t u16 = UINT16_MAX;
  uint32_t u32 = UINT32_MAX;
  uint64_t u64 = UINT64_MAX;

  std::string stdStr = "0123456789abcdef";  // to the bound 16 capcity
  char cStr[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x10};

  buffer.Write(c);
  ASSERT_EQ(buffer.Readable(), 1);
  ASSERT_EQ(buffer.Writeable(), 15);
  char oc;
  buffer.Read(oc);
  ASSERT_EQ(c, oc);

  buffer.Write(uc);
  ASSERT_EQ(buffer.Readable(), 1);
  ASSERT_EQ(buffer.Writeable(), 15);
  unsigned char ouc;
  buffer.Read(ouc);
  ASSERT_EQ(uc, ouc);

  buffer.Write(i16);
  ASSERT_EQ(buffer.Readable(), 2);
  ASSERT_EQ(buffer.Writeable(), 14);
  int16_t oi16;
  buffer.Read(oi16);
  ASSERT_EQ(i16, oi16);

  buffer.Write(i32);
  ASSERT_EQ(buffer.Readable(), 4);
  ASSERT_EQ(buffer.Writeable(), 12);
  int32_t oi32;
  buffer.Read(oi32);
  ASSERT_EQ(i32, oi32);

  buffer.Write(i64);
  ASSERT_EQ(buffer.Readable(), 8);
  ASSERT_EQ(buffer.Writeable(), 8);
  int64_t oi64;
  buffer.Read(oi64);
  ASSERT_EQ(i64, oi64);

  buffer.Write(u16);
  ASSERT_EQ(buffer.Readable(), 2);
  ASSERT_EQ(buffer.Writeable(), 14);
  uint16_t oui16;
  buffer.Read(oui16);
  ASSERT_EQ(u16, oui16);

  buffer.Write(u32);
  ASSERT_EQ(buffer.Readable(), 4);
  ASSERT_EQ(buffer.Writeable(), 12);
  uint32_t oui32;
  buffer.Read(oui32);
  ASSERT_EQ(u32, oui32);

  buffer.Write(u64);
  ASSERT_EQ(buffer.Readable(), 8);
  ASSERT_EQ(buffer.Writeable(), 8);
  uint64_t oui64;
  buffer.Read(oui64);
  ASSERT_EQ(u64, oui64);

  buffer.Write(stdStr.c_str());
  ASSERT_EQ(buffer.Readable(), 16);
  ASSERT_EQ(buffer.Writeable(), 0);  // FIXME: if expand at low water mark so
                                     // this will be 32 double expand
  char *ostdChs = (char *)malloc(16);
  buffer.Read(ostdChs, 16);
  std::string ostdStr(ostdChs, 16);
  ASSERT_EQ(stdStr, ostdStr);
  free(ostdChs);

  buffer.Write(cStr);
  ASSERT_EQ(buffer.Readable(), 16);
  ASSERT_EQ(buffer.Writeable(), 0);  // FIXME: if expand at low water mark so
                                     // this will be 32 double expand
  buffer.Read(ostdChs, 16);
  ASSERT_EQ(cStr, ostdChs);
}

// test the no expand read write
TEST(BufferTest, ReadWriteWithoutExpand) {
  zoo::skunk::Buffer buffer;

  // init capcity is 16 so use the 8 size buffer
  char halfFillInput[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  buffer.Write(halfFillInput);
  ASSERT_EQ(buffer.Capcity(), 16);
  ASSERT_EQ(buffer.Readable(), 8);
  ASSERT_EQ(buffer.Writeable(), 8);
  char halfFillOutput[8];
  buffer.Read(halfFillOutput, 8);
  ASSERT_EQ(halfFillInput, halfFillOutput);
  ASSERT_EQ(buffer.Readable(), 0);
  ASSERT_EQ(buffer.Writeable(), 0);
}

// test buffer write expand
TEST(BufferTest, WriteWithExpand) {
  zoo::skunk::Buffer buffer;

  char segmentOne[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  char segmentTwo[8] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};
  char segmentExpandOne[8] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27};
  char segmentExpandTwo[16] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                               0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x40};

  buffer.Write(segmentOne);
  buffer.Write(segmentTwo);
  ASSERT_EQ(buffer.Capcity(), 16);
  ASSERT_EQ(buffer.Readable(), 16);
  ASSERT_EQ(buffer.Writeable(), 0);

  buffer.Write(segmentExpandOne);
  ASSERT_EQ(buffer.Capcity(), 32);
  ASSERT_EQ(buffer.Readable(), 24);
  ASSERT_EQ(buffer.Writeable(), 8);

  buffer.Write(segmentExpandTwo);
  ASSERT_EQ(buffer.Capcity(), 64);
  ASSERT_EQ(buffer.Readable(), 42);
  ASSERT_EQ(buffer.Writeable(), 22);
}

// write with offset
TEST(BufferTest, WriteWithOffset) {
  zoo::skunk::Buffer buffer;
  char originSegment[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
  char offsetSegment[4] = {0x14, 0x15, 0x16, 0x17};
  buffer.Write(originSegment);
  buffer.Write(4, offsetSegment);
  ASSERT_EQ(buffer.Capcity(), 16);
  ASSERT_EQ(buffer.Readable(), 8);
  ASSERT_EQ(buffer.Writeable(), 8);

  char readSegment[8];
  char resultSegment[8] = {0x00, 0x01, 0x02, 0x03, 0x14, 0x15, 0x16, 0x17};
  buffer.Read(readSegment, 8);
  ASSERT_EQ(resultSegment, readSegment);
}

// After seek read from the buffer
TEST(BufferTest, ReadAfterSeek) {}

// Write with the whence
TEST(BufferTest, WriteWithWhence) { zoo::skunk::Buffer buffer; }

TEST(BufferTest, WriteWithIovec) {}

TEST(BufferTest, ReadWithIovec) {}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}