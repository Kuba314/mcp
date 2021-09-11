#include "gtest/gtest.h"

extern "C" {
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <stdint.h>

    #include "varint.h"
}

class VarType : public ::testing::Test {
  protected:
    virtual void SetUp() override {
        int err = pipe(fd);

        // throw exception if pipe allocation failed
        if(err != 0) {
            throw std::bad_alloc();
        }
    }
    virtual void TearDown() override {
        close(fd[0]);
        close(fd[1]);
    }

    int fd[2];
};

// ---------------------------------------------------------------- read varint
TEST_F(VarType, VIntReadSingleByte) {
    const uint8_t bytes[] = { 0x7f };
    const int32_t expected = 127;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    int err = read_varint(fd[0], &value);
    ASSERT_EQ(err, 0);

    EXPECT_EQ(expected, value);
}
TEST_F(VarType, VIntReadMultipleBytes) {
    const uint8_t bytes[] = { 0xff, 0x03 };
    const int32_t expected = 511;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    int err = read_varint(fd[0], &value);
    ASSERT_EQ(err, 0);

    EXPECT_EQ(expected, value);
}
TEST_F(VarType, VIntReadNegativeNumber) {
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0x0f };
    const int32_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    int err = read_varint(fd[0], &value);
    ASSERT_EQ(err, 0);

    EXPECT_EQ(expected, value);
}
TEST_F(VarType, VIntReadTooManyBytes) {
    const uint8_t bytes[] = { 0x80, 0x80, 0x80, 0x80, 0x80 };
    const int32_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int32_t value;
    int err = read_varint(fd[0], &value);
    ASSERT_EQ(err, 1);
}

// ---------------------------------------------------------------- write varint
TEST_F(VarType, VIntWriteSingleByte) {
    const int32_t value = 127;
    const uint8_t bytes[] = { 0x7f };
    int err = write_varint(fd[1], value);
    ASSERT_EQ(err, 0);

    for(uint8_t i = 0; i < sizeof(bytes); i++) {
        uint8_t byte;
        EXPECT_EQ(read(fd[0], &byte, 1), 1);
        EXPECT_EQ(byte, bytes[i]);
    }
}
TEST_F(VarType, VIntWriteMultipleBytes) {
    const int32_t value = 511;
    const uint8_t bytes[] = { 0xff, 0x03 };
    int err = write_varint(fd[1], value);
    ASSERT_EQ(err, 0);

    for(uint8_t i = 0; i < sizeof(bytes); i++) {
        uint8_t byte;
        EXPECT_EQ(read(fd[0], &byte, 1), 1);
        EXPECT_EQ(byte, bytes[i]);
    }
}
TEST_F(VarType, VIntWriteNegativeNumber) {
    const int32_t value = -1;
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0x0f };
    int err = write_varint(fd[1], value);
    ASSERT_EQ(err, 0);

    for(uint8_t i = 0; i < sizeof(bytes); i++) {
        uint8_t byte;
        EXPECT_EQ(read(fd[0], &byte, 1), 1);
        EXPECT_EQ(byte, bytes[i]);
    }
}

// ---------------------------------------------------------------- read varlong
TEST_F(VarType, VLongReadSingleByte) {
    const uint8_t bytes[] = { 0x7f };
    const int64_t expected = 127;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    int err = read_varlong(fd[0], &value);
    ASSERT_EQ(err, 0);

    EXPECT_EQ(expected, value);
}
TEST_F(VarType, VLongReadMultipleBytes) {
    const uint8_t bytes[] = { 0xff, 0x03 };
    const int64_t expected = 511;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    int err = read_varlong(fd[0], &value);
    ASSERT_EQ(err, 0);

    EXPECT_EQ(expected, value);
}
TEST_F(VarType, VLongReadNegativeNumber) {
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01 };
    const int64_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    int err = read_varlong(fd[0], &value);
    ASSERT_EQ(err, 0);

    EXPECT_EQ(expected, value);
}
TEST_F(VarType, VLongReadTooManyBytes) {
    const uint8_t bytes[] = { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80 };
    const int64_t expected = -1;
    write(fd[1], bytes, sizeof(bytes));

    int64_t value;
    int err = read_varlong(fd[0], &value);
    ASSERT_EQ(err, 1);
}

// ---------------------------------------------------------------- write varlong
TEST_F(VarType, VLongWriteSingleByte) {
    const int64_t value = 127;
    const uint8_t bytes[] = { 0x7f };
    int err = write_varlong(fd[1], value);
    ASSERT_EQ(err, 0);

    for(uint8_t i = 0; i < sizeof(bytes); i++) {
        uint8_t byte;
        EXPECT_EQ(read(fd[0], &byte, 1), 1);
        EXPECT_EQ(byte, bytes[i]);
    }
}
TEST_F(VarType, VLongWriteMultipleBytes) {
    const int64_t value = 511;
    const uint8_t bytes[] = { 0xff, 0x03 };
    int err = write_varlong(fd[1], value);
    ASSERT_EQ(err, 0);

    for(uint8_t i = 0; i < sizeof(bytes); i++) {
        uint8_t byte;
        EXPECT_EQ(read(fd[0], &byte, 1), 1);
        EXPECT_EQ(byte, bytes[i]);
    }
}
TEST_F(VarType, VLongWriteNegativeNumber) {
    const int64_t value = -1;
    const uint8_t bytes[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x01 };
    int err = write_varlong(fd[1], value);
    ASSERT_EQ(err, 0);

    for(uint8_t i = 0; i < sizeof(bytes); i++) {
        uint8_t byte;
        EXPECT_EQ(read(fd[0], &byte, 1), 1);
        EXPECT_EQ(byte, bytes[i]);
    }
}
