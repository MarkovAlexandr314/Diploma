#include <gtest/gtest.h>
#include "Parser.h"

class ParserTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        v = std::make_unique<Parser>(
            "/home/alex/Diploma/CGNS/example/xyz-L-cut4-z1-ordered-T4.cgns");

        v->ReadCoordinates();
    }

    void TearDown() override
    {
        // очистка, если нужна
    }
    std::unique_ptr<Parser> v;
};

TEST_F(ParserTest, CheckZoneNumber)
{
    EXPECT_EQ(v->GetZoneNumber(), 19);
}

TEST_F(ParserTest, CheckDimensionsOfFirstZone)
{
    EXPECT_EQ(v->GetCoordinates()[0].size(), 213);
    EXPECT_EQ(v->GetCoordinates()[0][0].size(), 76);
    EXPECT_EQ(v->GetCoordinates()[0][0][0].size(), 2);
}