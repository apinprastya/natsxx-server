#include "buffer.h"
#include <gtest/gtest.h>

TEST (Buffer, Buffer) {
    CharBuffer buff;
    buff.write ("MSG ");
    buff.write ("subsribeID");
    GTEST_LOG_ (INFO) << buff.getBuffer ().size ();
    auto str = std::string (buff.getBuffer ().begin (), buff.getBuffer ().end ());
    EXPECT_EQ (str, std::string ("MSG subsribeID"));
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest (&argc, argv);
    return RUN_ALL_TESTS ();
}