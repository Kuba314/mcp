#include "gtest/gtest.h"

extern "C" {
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <stdint.h>

    #include "varint.h"
    #include "unionstream.h"
}

class VarTypePipe : public ::testing::Test {
  protected:
    virtual void SetUp() override {
        int err = pipe(fd);

        // throw exception if pipe allocation failed
        if(err != 0) {
            throw std::bad_alloc();
        }
        us[0].sockfd = fd[0];
        us[1].sockfd = fd[1];
    }
    virtual void TearDown() override {
        close(fd[0]);
        close(fd[1]);
    }

    int fd[2];
    unionstream_t us[2];
};

// ---------------------------------------------------------------- read varint
TEST_F(VarTypePipe, VIntReadSingleByte) {
    const uint8_t bytes[] = { 0x7f };
    const int32_t expected = 127;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    uint8_t n_read;
    int err = read_varint(us[0], &value, &n_read);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(n_read, sizeof(bytes));

    EXPECT_EQ(expected, value);
}
TEST_F(VarTypePipe, VIntReadMultipleBytes) {
    const uint8_t bytes[] = { 0xff, 0x03 };
    const int32_t expected = 511;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    uint8_t n_read;
    int err = read_varint(us[0], &value, &n_read);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(n_read, sizeof(bytes));

    EXPECT_EQ(expected, value);
}
TEST_F(VarTypePipe, VIntReadNegativeNumber) {
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0x0f };
    const int32_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    uint8_t n_read;
    int err = read_varint(us[0], &value, &n_read);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(n_read, sizeof(bytes));

    EXPECT_EQ(expected, value);
}
TEST_F(VarTypePipe, VIntReadTooManyBytes) {
    const uint8_t bytes[] = { 0x80, 0x80, 0x80, 0x80, 0x80 };
    const int32_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    uint8_t n_read;
    int err = read_varint(us[0], &value, &n_read);
    ASSERT_EQ(err, 1);
    ASSERT_EQ(n_read, sizeof(bytes));
}

// ---------------------------------------------------------------- write varint
TEST(VarTypeFormat, VIntWriteSingleByte) {
    const int32_t value = 127;
    const uint8_t bytes[] = { 0x7f };
    uint8_t out_bytes[5];

    uint8_t n_written = format_varint(out_bytes, value);
    EXPECT_EQ(n_written, sizeof(bytes));
    EXPECT_EQ(memcmp(bytes, out_bytes, sizeof(bytes)), 0);
}
TEST(VarTypeFormat, VIntWriteMultipleBytes) {
    const int32_t value = 511;
    const uint8_t bytes[] = { 0xff, 0x03 };
    uint8_t out_bytes[5];

    uint8_t n_written = format_varint(out_bytes, value);
    EXPECT_EQ(n_written, sizeof(bytes));
    EXPECT_EQ(memcmp(bytes, out_bytes, sizeof(bytes)), 0);
}
TEST(VarTypeFormat, VIntWriteNegativeNumber) {
    const int32_t value = -1;
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0x0f };
    uint8_t out_bytes[5];

    uint8_t n_written = format_varint(out_bytes, value);
    EXPECT_EQ(n_written, sizeof(bytes));
    EXPECT_EQ(memcmp(bytes, out_bytes, sizeof(bytes)), 0);
}

// ---------------------------------------------------------------- read varlong
TEST_F(VarTypePipe, VLongReadSingleByte) {
    const uint8_t bytes[] = { 0x7f };
    const int64_t expected = 127;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    uint8_t n_read;
    int err = read_varlong(us[0], &value, &n_read);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(n_read, sizeof(bytes));

    EXPECT_EQ(expected, value);
}
TEST_F(VarTypePipe, VLongReadMultipleBytes) {
    const uint8_t bytes[] = { 0xff, 0x03 };
    const int64_t expected = 511;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    uint8_t n_read;
    int err = read_varlong(us[0], &value, &n_read);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(n_read, sizeof(bytes));

    EXPECT_EQ(expected, value);
}
TEST_F(VarTypePipe, VLongReadNegativeNumber) {
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01 };
    const int64_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    uint8_t n_read;
    int err = read_varlong(us[0], &value, &n_read);
    ASSERT_EQ(err, 0);
    ASSERT_EQ(n_read, sizeof(bytes));

    EXPECT_EQ(expected, value);
}
TEST_F(VarTypePipe, VLongReadTooManyBytes) {
    const uint8_t bytes[] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    const int64_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    uint8_t n_read;
    int err = read_varlong(us[0], &value, &n_read);
    ASSERT_EQ(err, 1);
    ASSERT_EQ(n_read, sizeof(bytes));
}

// ---------------------------------------------------------------- write varlong
TEST(VarTypeFormat, VLongWriteSingleByte) {
    const int64_t value = 127;
    const uint8_t bytes[] = { 0x7f };
    uint8_t out_bytes[10];

    uint8_t n_written = format_varlong(out_bytes, value);
    EXPECT_EQ(n_written, sizeof(bytes));
    EXPECT_EQ(memcmp(bytes, out_bytes, sizeof(bytes)), 0);
}
TEST(VarTypeFormat, VLongWriteMultipleBytes) {
    const int64_t value = 511;
    const uint8_t bytes[] = { 0xff, 0x03 };
    uint8_t out_bytes[10];

    uint8_t n_written = format_varlong(out_bytes, value);
    EXPECT_EQ(n_written, sizeof(bytes));
    EXPECT_EQ(memcmp(bytes, out_bytes, sizeof(bytes)), 0);
}
TEST(VarTypeFormat, VLongWriteNegativeNumber) {
    const int64_t value = -1;
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01 };
    uint8_t out_bytes[10];

    uint8_t n_written = format_varlong(out_bytes, value);
    EXPECT_EQ(n_written, sizeof(bytes));
    EXPECT_EQ(memcmp(bytes, out_bytes, sizeof(bytes)), 0);
}
