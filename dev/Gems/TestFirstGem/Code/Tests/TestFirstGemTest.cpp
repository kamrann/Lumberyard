
#include "TestFirstGem_precompiled.h"

#include <AzTest/AzTest.h>

class TestFirstGemTest
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

TEST_F(TestFirstGemTest, ExampleTest)
{
    ASSERT_TRUE(true);
}

AZ_UNIT_TEST_HOOK();
