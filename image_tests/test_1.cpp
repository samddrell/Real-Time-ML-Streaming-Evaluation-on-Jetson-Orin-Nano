#include <cstdint>
#include <string>
#include <gtest/gtest.h>
#include "image.h"
#include <fstream>
#include <filesystem>


Image* make_color_band(Image* image, uint8_t height, uint8_t width);
Image* make_gradient(Image* image, uint8_t height, uint8_t width);

// Required for gtest main
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


class ImageTest : public ::testing::Test 
{
protected:
    std::vector<std::string> files_to_delete;

    void TearDown() override 
    {
        for (const auto& file : files_to_delete) 
        {
            std::filesystem::remove(file);
        }
    }

    void TrackFile(const std::string& filename) 
    {
        files_to_delete.push_back(filename);
    }
};


TEST_F(ImageTest, DimensionMismatch)
{
    Image img1(100, 100);
    Image img2(200, 100); // Different width

    EXPECT_FALSE(img1 == img2) << "Should fail on size mismatch";
    EXPECT_FALSE(img1.compare(img2, 0.1)) << "Should fail on size mismatch even with tolerance";
}

TEST_F(ImageTest, JPEGCompareFailsWithTightTolerance)
{
    int width = 255;
    int height = 255;
    double allowedError = 0.0001; // Extremely strict
    int quality = 10;

    Image* original = make_gradient(new Image(width, height), height, width);
    ASSERT_TRUE(original->SaveJPEG("gradient_lowqual.jpg", quality)) << "Failed to save JPEG";
    TrackFile("gradient_lowqual.jpg");

    Image* loaded = new Image();
    ASSERT_TRUE(loaded->OpenJPEG("gradient_lowqual.jpg")) << "Failed to read JPEG";

    EXPECT_FALSE(*original == *loaded) << "JPEGs should not match exactly";
    EXPECT_FALSE(original->compare(*loaded, allowedError)) << "Should exceed tight error threshold";

    delete original;
    delete loaded;
}

TEST_F(ImageTest, JPEGCompareWithLossTolerance)
{
    int width = 255;
    int height = 255;
    double allowedError = 0.01; // 1% error tolerance
    int quality = 85;

    Image* original = make_gradient(new Image(width, height), height, width);
    ASSERT_TRUE(original->SaveJPEG("gradient_test.jpg", quality)) << "Failed to save JPEG";
    TrackFile("gradient_test.jpg");

    Image* loaded = new Image();
    ASSERT_TRUE(loaded->OpenJPEG("gradient_test.jpg")) << "Failed to read JPEG";

    EXPECT_FALSE(*original == *loaded) << "JPEGs should not match exactly due to compression";
    EXPECT_TRUE(original->compare(*loaded, allowedError)) << "JPEGs should be similar within error threshold";

    delete original;
    delete loaded;
}

TEST_F(ImageTest, ExactPNGMatch)
{
    int width = 255;
    int height = 255;

    Image* img1 = make_color_band(new Image(width, height), height, width);
    ASSERT_TRUE(img1->SavePNG("color_band_test.png")) << "Failed to save PNG";
    TrackFile("color_band_test.png");

    Image* img2 = new Image();
    ASSERT_TRUE(img2->OpenPNG("color_band_test.png")) << "Failed to read PNG";

    EXPECT_TRUE(*img1 == *img2) << "PNG images should match exactly";

    delete img1;
    delete img2;
}

TEST_F(ImageTest, OpenHandlesUppercaseExtension) {
    Image img(10, 10);
    EXPECT_TRUE(img.SaveFile("test_uppercase.JPG"));
    TrackFile("test_uppercase.JPG");

    Image loaded;
    EXPECT_TRUE(loaded.OpenFile("test_uppercase.JPG"));
}


TEST_F(ImageTest, OpenFailsOnMissingFile) {
    Image img;
    EXPECT_FALSE(img.OpenFile("nonexistent.jpg"));
}


TEST_F(ImageTest, SaveFailsOnUnsupportedExtension) {
    Image img(10, 10);
    EXPECT_FALSE(img.SaveFile("test.unsupported"));
    TrackFile("test.unsupported"); // Track for cleanup, even if it fails
}

TEST_F(ImageTest, OpenFailsOnUnsupportedExtension) {
    Image img;
    EXPECT_FALSE(img.OpenFile("test.unsupported"));
}


TEST_F(ImageTest, SaveAndLoadJPEGThroughGeneralInterface) {
    Image img(10, 10);
    img.SetPixelRed(2, 2, 90);
    img.SetPixelGreen(2, 2, 140);
    img.SetPixelBlue(2, 2, 190);

    EXPECT_TRUE(img.SaveFile("test_general.jpeg"));
    TrackFile("test_general.jpeg");

    Image loaded;
    EXPECT_TRUE(loaded.OpenFile("test_general.jpeg"));

    // JPEG is lossy, so allow for minor difference
    EXPECT_NEAR(loaded.GetPixelRed(2, 2), 90, 10);
    EXPECT_NEAR(loaded.GetPixelGreen(2, 2), 140, 10);
    EXPECT_NEAR(loaded.GetPixelBlue(2, 2), 190, 10);
}


TEST_F(ImageTest, SaveAndLoadPNGThroughGeneralInterface) {
    Image img(10, 10);
    img.SetPixelRed(1, 1, 50);
    img.SetPixelGreen(1, 1, 100);
    img.SetPixelBlue(1, 1, 150);

    EXPECT_TRUE(img.SaveFile("test_general.png"));
    TrackFile("test_general.png");

    Image loaded;
    EXPECT_TRUE(loaded.OpenFile("test_general.png"));

    EXPECT_EQ(loaded.GetPixelRed(1, 1), 50);
    EXPECT_EQ(loaded.GetPixelGreen(1, 1), 100);
    EXPECT_EQ(loaded.GetPixelBlue(1, 1), 150);
}


TEST_F(ImageTest, SaveJPGReturnsTrueAndCreatesFile) {
    int width = 3840;
    int height = 2160;
    int quality = 1;
    const char* filename = "test_output.jpg";

    // Create test image
    Image img(width, height);

    // Call Save_jpg and assert it returns true
    EXPECT_TRUE(img.SaveJPEG(const_cast<char*>(filename), quality));
    TrackFile(filename); // Track the file for cleanup

    // Optionally: Check if the file was created
    std::ifstream f(filename);
    EXPECT_TRUE(f.good());
}

TEST_F(ImageTest, ZeroSizeImage) {
    Image img(0, 0);
    EXPECT_FALSE(img.SavePNG("zero.png"));  // Should not save
    TrackFile("zero.png"); // Track for cleanup, even if it fails
}


TEST_F(ImageTest, ReadInvalidFile) {
    Image img;
    EXPECT_FALSE(img.OpenPNG("non_existent_file.png"));
}


TEST_F(ImageTest, BoundsCheckIfImplemented) {
    Image img(10, 10);

    // Expect the program to not crash or assert
    // Add exception or bounds checks in implementation if needed
    img.SetPixelRed(15, 15, 255); // Should ideally be a no-op or assert
    EXPECT_EQ(img.GetPixelRed(15, 15), 0); // If bounds not checked, may crash
}


TEST_F(ImageTest, SaveAndReadConsistency) {
    Image img(32, 32);
    img.SetPixelRed(0, 0, 100);
    img.SetPixelGreen(0, 0, 150);
    img.SetPixelBlue(0, 0, 200);

    img.SavePNG("temp_test_image.png");
    TrackFile("temp_test_image.png");

    Image loaded;
    ASSERT_TRUE(loaded.OpenPNG("temp_test_image.png"));
    EXPECT_EQ(loaded.GetPixelRed(0, 0), 100);
    EXPECT_EQ(loaded.GetPixelGreen(0, 0), 150);
    EXPECT_EQ(loaded.GetPixelBlue(0, 0), 200);
}

TEST_F(ImageTest, SetAndGetPixelValues) {
    Image img(10, 10);
    img.SetPixelRed(5, 5, 123);
    img.SetPixelGreen(5, 5, 45);
    img.SetPixelBlue(5, 5, 200);

    EXPECT_EQ(img.GetPixelRed(5, 5), 123);
    EXPECT_EQ(img.GetPixelGreen(5, 5), 45);
    EXPECT_EQ(img.GetPixelBlue(5, 5), 200);
}

TEST_F(ImageTest, ConstructorInitializesCorrectly) {
    Image img(100, 100);
    // Assuming (0,0) should be default initialized to 0
    EXPECT_EQ(img.GetPixelRed(0, 0), 0);
    EXPECT_EQ(img.GetPixelGreen(0, 0), 0);
    EXPECT_EQ(img.GetPixelBlue(0, 0), 0);
}

Image* make_gradient(Image* image, uint8_t height, uint8_t width)
{
    int practical_depth = 256; // Practical depth for JPEG
    int row_offset = 0; // Offset for the current row in the image buffer
    
    for (int y = 0; y < height; y++)
    {
        row_offset = 3 * width * y; // Offset for the current row in the image buffer
        for (int x = 0; x < width; x++)
        {
            image->m_data[row_offset + 3 * x] = (x * (practical_depth) / width) % (practical_depth);
            image->m_data[row_offset + 3 * x + 1] = (y * (practical_depth) / height) % (practical_depth);
            image->m_data[row_offset + 3 * x + 2] = (y * (practical_depth) / height + 
                x * (practical_depth) / width) % (practical_depth);
        }
    }

    return image;
}

Image* make_color_band(Image* image, uint8_t height, uint8_t width)
{
    int brand_width = width / 8;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (i < brand_width)
            {
                image->SetPixelRed(i,j,255);
                image->SetPixelGreen(i,j,255);
                image->SetPixelBlue(i,j,255);
            }
            else if (i < brand_width * 2)
            {
                image->SetPixelRed(i,j,255);
                image->SetPixelGreen(i,j,255);
                image->SetPixelBlue(i,j,16);
            }
            else if (i < brand_width * 3)
            {
                image->SetPixelRed(i,j,16);
                image->SetPixelGreen(i,j,255);
                image->SetPixelBlue(i,j,255);
            }
            else if (i < brand_width * 4)
            {
                image->SetPixelRed(i,j,16);
                image->SetPixelGreen(i,j,255);
                image->SetPixelBlue(i,j,16);
            }
            else if (i < brand_width * 5)
            {
                image->SetPixelRed(i,j,255);
                image->SetPixelGreen(i,j,16);
                image->SetPixelBlue(i,j,255);
            }
            else if (i < brand_width * 6)
            {
                image->SetPixelRed(i,j,255);
                image->SetPixelGreen(i,j,16);
                image->SetPixelBlue(i,j,16);
            }
            else if (i < brand_width * 7)
            {
                image->SetPixelRed(i,j,16);
                image->SetPixelGreen(i,j,16);
                image->SetPixelBlue(i,j,255);
            }
            else
            {
                image->SetPixelRed(i,j,16);
                image->SetPixelGreen(i,j,16);
                image->SetPixelBlue(i,j,16);
            }
        }
    }
    return image;
}
