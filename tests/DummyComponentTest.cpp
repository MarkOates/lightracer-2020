
#include <gtest/gtest.h>

#include <DummyComponent.hpp>

TEST(DummyComponentTest, can_be_created_without_blowing_up)
{
   DummyComponent dummy_component;
}

TEST(DummyComponentTest, run__returns_the_expected_response)
{
   DummyComponent dummy_component;
   std::string expected_string = "Hello World!";
   EXPECT_EQ(expected_string, dummy_component.run());
}
