#include <LibCore/RefPtr.h>

#include <gtest/gtest.h>

namespace Terran::Tests {

class MockRefCounted : public Core::RefCounted {
};

class RefPtrTest : public testing::Test {

protected:
    RefPtrTest() = default;
    virtual ~RefPtrTest() override = default;
};

TEST_F(RefPtrTest, default_constructor_initializes_null_data)
{
    Core::RefPtr<MockRefCounted> ref_ptr;
    ASSERT_EQ(ref_ptr.data(), nullptr);
}

TEST_F(RefPtrTest, nullptr_t_constructor_initializes_null_data)
{
    Core::RefPtr<MockRefCounted> ref_ptr(nullptr);
    ASSERT_EQ(ref_ptr.data(), nullptr);
}

TEST_F(RefPtrTest, value_pointer_constructor_sets_data)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    ASSERT_EQ(ref_ptr.data(), mock_object);
}

TEST_F(RefPtrTest, value_pointer_constructor_increments_ref_count)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    ASSERT_EQ(mock_object->ref_count(), 1);
}

TEST_F(RefPtrTest, reset_sets_null_if_ref_count_is_zero)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);

    ref_ptr.reset();
    ASSERT_EQ(ref_ptr.data(), nullptr);
}

TEST_F(RefPtrTest, destructor_decrements_ref_count)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);

    {
        Core::RefPtr<MockRefCounted> temp_ref_ptr = ref_ptr;
        ASSERT_EQ(mock_object->ref_count(), 2);
    }

    ASSERT_EQ(mock_object->ref_count(), 1);
}

TEST_F(RefPtrTest, release_exchanges_data_for_nullptr)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    MockRefCounted* data = ref_ptr.release();
    EXPECT_EQ(mock_object, data);
    EXPECT_EQ(ref_ptr.data(), nullptr);
    delete mock_object;
}

TEST_F(RefPtrTest, copy_constructor_sets_data)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr(ref_ptr);
    ASSERT_EQ(temp_ref_ptr.data(), mock_object);
}

TEST_F(RefPtrTest, copy_constructor_increments_ref_count)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr(ref_ptr);
    ASSERT_EQ(mock_object->ref_count(), 2);
}

TEST_F(RefPtrTest, move_constructor_sets_data)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr(std::move(ref_ptr));
    ASSERT_EQ(temp_ref_ptr.data(), mock_object);
}

TEST_F(RefPtrTest, move_constructor_releases_moved_data)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr(std::move(ref_ptr));
    ASSERT_EQ(ref_ptr, nullptr);
}

TEST_F(RefPtrTest, move_constructor_doesnt_increment_ref_count)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr(std::move(ref_ptr));
    ASSERT_EQ(mock_object->ref_count(), 1);
}

TEST_F(RefPtrTest, comparison_operator_returns_true_when_both_refptrs_point_to_same_object)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr(ref_ptr);
    ASSERT_TRUE(ref_ptr == temp_ref_ptr);
}

TEST_F(RefPtrTest, comparison_operator_returns_false_when_both_refptrs_dont_point_to_same_object)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr;
    ASSERT_FALSE(ref_ptr == temp_ref_ptr);
}

TEST_F(RefPtrTest, nullptr_assignment_operator_sets_data_to_nullptr)
{
    Core::RefPtr<MockRefCounted> ref_ptr = nullptr;
    ASSERT_EQ(ref_ptr.data(), nullptr);
}

TEST_F(RefPtrTest, nullptr_assignment_operator_decrements_refcount_when_previous_data_is_not_null)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr(ref_ptr);
    ASSERT_EQ(mock_object->ref_count(), 2);
    ref_ptr = nullptr;
    ASSERT_EQ(mock_object->ref_count(), 1);
}
TEST_F(RefPtrTest, copy_assignment_operator_increments_refcount)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr = ref_ptr;
    ASSERT_EQ(mock_object->ref_count(), 2);
}

TEST_F(RefPtrTest, move_assignment_operator_doesnt_increase_refcount)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr(mock_object);
    Core::RefPtr<MockRefCounted> temp_ref_ptr = std::move(ref_ptr);
    ASSERT_EQ(mock_object->ref_count(), 1);
}

TEST_F(RefPtrTest, value_pointer_assignment_operator_doesnt_increase_refcount)
{
    MockRefCounted* mock_object = new MockRefCounted();
    Core::RefPtr<MockRefCounted> ref_ptr = mock_object;
    ASSERT_EQ(mock_object->ref_count(), 1);
}

}
