// Includes
#include <cstdint>     // for uint8_t
#include <string>      // for std::string

//Image Class
class Image
{
    private:
        int m_width;
        int m_height;
        int m_buffSize;           // Resolution for JPEG compression

        int openJPEG(struct jpeg_decompress_struct *cinfo,
                        std::string infilename);

    public:
        uint8_t *m_data;

        Image(); // Default constructor
        Image(int w, int h);    // Alocate memory for the Array

        bool operator==(const Image &other) const;      
        bool compare(const Image &other, double maxPercentError = 0.0) const; // Compare two images      
        
        uint8_t GetPixelRed(uint8_t x, uint8_t y);          // Get the red value of a pixel
        uint8_t GetPixelGreen(uint8_t x, uint8_t y);        // Get the green value of a pixel
        uint8_t GetPixelBlue(uint8_t x, uint8_t y);         // Get the blue value of a pixel

        void SetPixelRed(uint8_t x, uint8_t y,uint8_t r);       // Set the red value of a pixel
        void SetPixelGreen(uint8_t x, uint8_t y, uint8_t g);    // Set the green value of a pixel
        void SetPixelBlue(uint8_t x, uint8_t y, uint8_t b);     // Set the blue value of a pixel

        bool SavePNG(std::string filePath);     // Save the image to a png file
        bool OpenPNG(std::string filePath);     // Read the image from a png file

        bool SaveJPEG(std::string filename, int quality = 100); // Save the image to a jpg file
        int OpenJPEG(std::string infilename);

        bool SaveFile(std::string infilename, int quality = 100);
        bool OpenFile(std::string infilename);

        ~Image(); // Free memory
};