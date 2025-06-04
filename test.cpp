/**
 * @file test.cpp 
 * @brief Tests for functions. 
*/

#include "gtest/gtest.h"
#include "Functions.h"
#include "BMPheaders.h"

/**
 * @brief Tests for Clamp, check if it returns correct values. 
*/
TEST(ClampTest, BasicCases) {
    EXPECT_EQ(clamp(5, 0, 10), 5);
    EXPECT_EQ(clamp(-5, 0, 10), 0);
    EXPECT_EQ(clamp(15, 0, 10), 10);
    EXPECT_EQ(clamp(0, 0, 0), 0);
    EXPECT_FLOAT_EQ(clamp(5.5f, 0.0f, 10.0f), 5.5f);
}

/**
 * @brief Test for loading: imgdata from empty file should be nullptr. 
*/
TEST(LoadTest, EmptyFile) {
    BMPinfo info;
    size_t imgsize;
    uint8_t* imgdata = load("empty.bmp", imgsize, info);
    EXPECT_EQ(imgdata, nullptr); 

    delete[] imgdata;
}

/**
 * @brief Test for load if it returns false when BMP is not 24 bit. 
*/
TEST(LoadTest, InvalidBMPFormatReturnsNullptr) {
    BMPinfo info;
    size_t imgsize;
    uint8_t* imgdata = load("bmp_08.bmp", imgsize, info);
    EXPECT_EQ(imgdata, nullptr);

    delete[] imgdata;
}

/**
 * @brief Test for reading BMP info, not existing file should not be read. 
*/
TEST(ReadBMPInfoTest, NotExists) {
    BMPinfo info;
    EXPECT_FALSE(readBMPinfo("non_existent_file.bmp", info));
}

/**
 * @brief Test if 'save' saves valid data
*/
TEST(SaveTest, SaveValidData) {
    BMPinfo info;
    size_t imgsize;
    uint8_t* imgdata = load("4pixel.bmp", imgsize, info);
    ASSERT_NE(imgdata, nullptr);

    save("saved_4pixel.bmp", imgdata, imgsize, info);

    BMPinfo saved_info;
    size_t saved_imgsize;
    uint8_t* saved_imgdata = load("saved_4pixel.bmp", saved_imgsize, saved_info);
    ASSERT_NE(saved_imgdata, nullptr);
    EXPECT_EQ(saved_imgsize, imgsize);

    delete[] imgdata;
}

/**
 * @brief Rotate forward test that check that image data is same. 
*/
TEST(RotateTest, RotateForwardCorrect) {
    BMPinfo info;
    size_t imgsize;
    uint8_t* imgdata = load("4pixel.bmp", imgsize, info);
    ASSERT_NE(imgdata, nullptr);

    int original_width = info.width;
    int original_height = info.height;
    size_t original_imgsize = imgsize;

    rotateforward(imgdata, info, imgsize);

    EXPECT_EQ(info.width, original_height); 
    EXPECT_EQ(info.height, original_width); 
    EXPECT_EQ(info.width, 2);
    EXPECT_EQ(info.height, 2);

    EXPECT_EQ(imgsize, original_imgsize);

    delete[] imgdata;
}

/**
 * @brief Rotate backwards test that check that image data is same. 
*/
TEST(RotateTest, RotateBackwardsCorrect) {
    BMPinfo info;
    size_t imgsize;
    uint8_t* imgdata = load("4pixel.bmp", imgsize, info);
    ASSERT_NE(imgdata, nullptr);

    int original_width = info.width;
    int original_height = info.height;
    size_t original_imgsize = imgsize;

    rotatebackwards(imgdata, info, imgsize);

    EXPECT_EQ(info.width, original_height); 
    EXPECT_EQ(info.height, original_width); 
    EXPECT_EQ(info.width, 2);
    EXPECT_EQ(info.height, 2);

    EXPECT_EQ(imgsize, original_imgsize);

    delete[] imgdata;
}

/**
 * @brief Check if after rotations image is the same. 
*/
TEST(RotateTest, RotateForwardAndBackwardsCorrect) {
    BMPinfo info;
    size_t imgsize;
    uint8_t* imgdata = load("4pixel.bmp", imgsize, info);
    ASSERT_NE(imgdata, nullptr);

    int original_width = info.width;
    int original_height = info.height;
    size_t original_imgsize = imgsize;

    rotateforward(imgdata, info, imgsize);
    rotatebackwards(imgdata, info, imgsize);

    EXPECT_EQ(info.width, original_height); 
    EXPECT_EQ(info.height, original_width);
    EXPECT_EQ(imgsize, original_imgsize);

    delete[] imgdata;
}

/**
 * @brief Check if after rotations image is the same. 
*/
TEST(BlurTest, BlurDoesNotChangeSizeAndOther) {
    BMPinfo info;
    size_t imgsize;
    uint8_t* imgdata = load("4pixel.bmp", imgsize, info);
    ASSERT_NE(imgdata, nullptr);

    int original_width = info.width;
    int original_height = info.height;
    size_t original_imgsize = imgsize;

    blur(imgdata, info);

    EXPECT_EQ(info.width, original_height); 
    EXPECT_EQ(info.height, original_width);
    EXPECT_EQ(imgsize, original_imgsize);

    delete[] imgdata;
}
