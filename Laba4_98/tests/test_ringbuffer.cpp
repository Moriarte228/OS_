#include "../RingBuffer.hpp"
#include <gtest/gtest.h>
#include <cstdio>  // std::remove

const char* TEST_FILENAME = "test_ringbuffer.dat";

class RingBufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Удалим файл, если он остался от предыдущих запусков
        std::remove(TEST_FILENAME);
    }

    void TearDown() override {
        std::remove(TEST_FILENAME);
    }
};

TEST_F(RingBufferTest, CreateAndOpenBuffer) {
RingBuffer buffer(TEST_FILENAME, 5, true);
EXPECT_TRUE(buffer.isOpened());
}

TEST_F(RingBufferTest, WriteAndReadMessage) {
RingBuffer buffer(TEST_FILENAME, 5, true);
ASSERT_TRUE(buffer.isOpened());

Message msg;
ASSERT_TRUE(msg.setText("Hello"));

EXPECT_TRUE(buffer.writeMessage(msg));

Message result;
EXPECT_TRUE(buffer.readMessage(result));
EXPECT_STREQ(result.text, "Hello");
}

TEST_F(RingBufferTest, MessageTooLongRejected) {
Message msg;
std::string longText(50, 'X');  // Превышает 20 символов
EXPECT_FALSE(msg.setText(longText));
}

TEST_F(RingBufferTest, ReadFromEmptyBufferFailsGracefully) {
RingBuffer buffer(TEST_FILENAME, 5, true);
ASSERT_TRUE(buffer.isOpened());

Message msg;
EXPECT_FALSE(buffer.readMessage(msg, 200));  // Пустой буфер — читать нечего
}
