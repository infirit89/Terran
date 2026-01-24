#include <LibCore/UUID.h>

#include <gtest/gtest.h>
#include <string>

namespace Terran::Tests {

class UUIDTests : public testing::Test {

protected:
    UUIDTests() = default;
    ~UUIDTests() = default;
};

TEST_F(UUIDTests, from_string_returns_invalid_if_empty_string)
{
    Core::UUID id = Core::UUID::from_string("");
    ASSERT_EQ(id, Core::UUID::invalid());
}

TEST_F(UUIDTests, from_string_retuns_invalid_if_string_is_too_small)
{
    Core::UUID id = Core::UUID::from_string("1234-1234-1234-1234-1234");
    ASSERT_EQ(id, Core::UUID::invalid());
}

TEST_F(UUIDTests, from_string_retuns_invalid_if_string_contains_non_hex_digit_character)
{
    Core::UUID id = Core::UUID::from_string("654!c83f-de90-4533-8052-34125eee6c8f");
    ASSERT_EQ(id, Core::UUID::invalid());
}

TEST_F(UUIDTests, from_string_retuns_valid_if_string_contains_valid_id)
{
    Core::UUID id = Core::UUID::from_string("6549c83f-de90-4533-8052-34125eee6c8f");
    ASSERT_NE(id, Core::UUID::invalid());
}

TEST_F(UUIDTests, is_valid_returns_false_if_invalid)
{
    Core::UUID const& id = Core::UUID::invalid();
    ASSERT_FALSE(id.is_valid());
}

TEST_F(UUIDTests, is_valid_returns_true_if_valid)
{
    Core::UUID id = Core::UUID::from_string("6549c83f-de90-4533-8052-34125eee6c8f");
    ASSERT_TRUE(id.is_valid());
}

TEST_F(UUIDTests, default_constructor_generates_valid_id)
{
    Core::UUID id;
    ASSERT_TRUE(id.is_valid());
}

TEST_F(UUIDTests, to_string_converts_to_a_valid_uuid)
{
    Core::UUID id;
    std::string id_str = std::to_string(id);
    Core::UUID parse_id = Core::UUID::from_string(id_str);
    ASSERT_EQ(id, parse_id);
}

}
