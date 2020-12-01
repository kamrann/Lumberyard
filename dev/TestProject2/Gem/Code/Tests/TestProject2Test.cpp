
#include "TestProject2_precompiled.h"

#include <AzTest/AzTest.h>

class TestProject2Test
    : public ::testing::Test
{
protected:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }
};

TEST_F(TestProject2Test, ExampleTest)
{
    ASSERT_TRUE(true);
}

AZ_UNIT_TEST_HOOK();
