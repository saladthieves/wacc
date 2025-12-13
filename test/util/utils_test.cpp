#include "utils.hpp"

#include <gtest/gtest.h>

using wacc::utils::getFileInfo;

TEST(UtilsTest, getFileInfo) {
    // ARRANGE
    const auto path = "some/path/to/file.c";

    // ACT
    auto info = getFileInfo(path);

    // ASSERT
    ASSERT_STREQ(info.fileName.c_str(), "file.c");
    ASSERT_STREQ(info.stem.c_str(), "file");
    ASSERT_STREQ(info.extension.c_str(), ".c");
    ASSERT_STREQ(info.parent.c_str(), "some/path/to");
}

TEST(UtilsTest, getFileInfoNoExt) {
    // ARRANGE
    const auto path = "some/path/to/file";

    // ACT
    auto info = getFileInfo(path);

    // ASSERT
    ASSERT_STREQ(info.fileName.c_str(), "file");
    ASSERT_STREQ(info.stem.c_str(), "file");
    ASSERT_STREQ(info.extension.c_str(), "");
    ASSERT_STREQ(info.parent.c_str(), "some/path/to");
}