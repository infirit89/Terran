#include <LibCore/Buffer.h>

#include <gtest/gtest.h>

namespace Terran::Tests {

class ByteBufferTest : public testing::Test {
protected:
    ByteBufferTest() = default;
    ~ByteBufferTest() = default;
};

TEST_F(ByteBufferTest, empty_constructor_initializes_null_data)
{
    Core::ByteBuffer buffer;
    ASSERT_EQ(buffer.data(), nullptr);
    ASSERT_EQ(buffer.size(), 0);
}

TEST_F(ByteBufferTest, constructor_with_size_allocates_buffer)
{
    constexpr size_t buffer_size = 10;
    Core::ByteBuffer buffer(buffer_size);
    ASSERT_NE(buffer.data(), nullptr);
    ASSERT_EQ(buffer.size(), buffer_size);
}

TEST_F(ByteBufferTest, constructor_with_data_and_size_coppies_data_with_a_given_size)
{
    constexpr size_t buffer_size = 3;
    int* data = new int[buffer_size] { 1, 2, 3 };
    Core::ByteBuffer buffer(data, sizeof(int) * buffer_size);
    EXPECT_EQ(buffer.size(), buffer_size * sizeof(int));

    int* data_res = (int*)buffer.data();
    EXPECT_EQ(data_res, data);
    EXPECT_EQ(data_res[0], data[0]);
    EXPECT_EQ(data_res[1], data[1]);
    EXPECT_EQ(data_res[2], data[2]);
}

TEST_F(ByteBufferTest, bool_operator_returns_false_when_buffer_is_invalid)
{
    Core::ByteBuffer buffer;
    ASSERT_FALSE(buffer);
    ASSERT_EQ(buffer.data(), nullptr);
    ASSERT_EQ(buffer.size(), 0);
}

TEST_F(ByteBufferTest, bool_operator_returns_true_when_buffer_is_valid)
{
    Core::ByteBuffer buffer(sizeof(int) * 10);
    ASSERT_TRUE(buffer);
    ASSERT_NE(buffer.data(), nullptr);
    ASSERT_GT(buffer.size(), 0);
}

TEST_F(ByteBufferTest, free_deallocates_buffer_memory)
{
    constexpr size_t buffer_size = 10;
    Core::ByteBuffer buffer(buffer_size);
    buffer.free();
    ASSERT_FALSE(buffer);
}

TEST_F(ByteBufferTest, allocate_allocates_memory_of_a_given_size)
{
    constexpr size_t buffer_size = 10;
    Core::ByteBuffer buffer;
    ASSERT_FALSE(buffer);
    buffer.allocate(sizeof(int) * buffer_size);
    ASSERT_TRUE(buffer);
    ASSERT_EQ(buffer.size(), sizeof(int) * buffer_size);
}

TEST_F(ByteBufferTest, copy_of_another_byte_buffer_allocates_new_buffer)
{
    constexpr size_t buffer_size = 3;
    int* data = new int[buffer_size] { 1, 2, 3 };
    Core::ByteBuffer buffer(data, sizeof(int) * buffer_size);
    Core::ByteBuffer another_buffer = Core::ByteBuffer::Copy(buffer);
    EXPECT_NE(another_buffer.data(), buffer.data());
    EXPECT_EQ(buffer.size(), another_buffer.size());
    int* data_res = (int*)another_buffer.data();
    EXPECT_EQ(data_res[0], data[0]);
    EXPECT_EQ(data_res[1], data[1]);
    EXPECT_EQ(data_res[2], data[2]);
}

TEST_F(ByteBufferTest, copy_of_another_raw_data_allocates_new_buffer)
{
    constexpr size_t buffer_size = 3;
    int* data = new int[buffer_size] { 1, 2, 3 };
    Core::ByteBuffer buffer = Core::ByteBuffer::Copy(data, sizeof(int) * buffer_size);
    EXPECT_EQ(buffer.size(), sizeof(int) * buffer_size);
    int* data_res = (int*)buffer.data();
    EXPECT_NE(data, data_res);
    EXPECT_EQ(data_res[0], data[0]);
    EXPECT_EQ(data_res[1], data[1]);
    EXPECT_EQ(data_res[2], data[2]);
}

TEST_F(ByteBufferTest, read_returns_out_of_bounds_when_offset_bigger_than_size)
{
    constexpr size_t buffer_size = 3;
    int* data = new int[buffer_size] { 1, 2, 3 };
    Core::ByteBuffer buffer = Core::ByteBuffer::Copy(data, sizeof(int) * buffer_size);
    auto read_result_bigger_than_size = buffer.read(4 * sizeof(int));
    ASSERT_FALSE(read_result_bigger_than_size.is_ok());
    EXPECT_EQ(read_result_bigger_than_size.error(), Core::ByteBufferError::OutOfBounds);
}

TEST_F(ByteBufferTest, read_returns_data_when_offset_is_in_bounds)
{
    constexpr size_t buffer_size = 3;
    int* data = new int[buffer_size] { 1, 2, 3 };
    Core::ByteBuffer buffer = Core::ByteBuffer::Copy(data, sizeof(int) * buffer_size);
    auto res = buffer.read(1 * sizeof(int));
    ASSERT_TRUE(res.is_ok());
    int* data_res = (int*)res.value();
    EXPECT_EQ(*data_res, data[1]);
}

TEST_F(ByteBufferTest, write_returns_overflow_when_overflowing) {
    constexpr size_t buffer_size = 3;
    int* data = new int[buffer_size] { 1, 2, 3 };
    Core::ByteBuffer buffer(buffer_size * sizeof(int));
    auto write_result = buffer.write(data, 2, buffer_size * sizeof(int));
    ASSERT_FALSE(write_result.is_ok());
    EXPECT_EQ(write_result.error(), Core::ByteBufferError::Overflow);
}

TEST_F(ByteBufferTest, write_returns_empty_when_valid) {
    constexpr size_t buffer_size = 3;
    int* data = new int[buffer_size] { 1, 2, 3 };
    Core::ByteBuffer buffer(buffer_size * sizeof(int));
    auto write_result = buffer.write(data, 0, buffer_size * sizeof(int));
    ASSERT_TRUE(write_result.is_ok());
}

}
