// Includes
#include <cstdint>     // for uint8_t
#include <string>      // for std::string

//Image Class
class Image
{
    private:
        int m_width;
        int m_height;
        int Read_jpeg_priv(struct jpeg_decompress_struct *cinfo,
                        std::string infilename);

    public:
        uint8_t *m_data;

        Image(); // Default constructor
        Image(int w, int h);    // Alocate memory for the Array

        bool operator==(const Image &other) const;            
        
        uint8_t GetPixalRed(uint8_t x, uint8_t y); // Get the red value of a pixel
        uint8_t GetPixalGreen(uint8_t x, uint8_t y); // Get the green value of a pixel
        uint8_t GetPixalBlue(uint8_t x, uint8_t y); // Get the blue value of a pixel

        void SetPixalRed(uint8_t x, uint8_t y,uint8_t r); // Set the red value of a pixel
        void SetPixalGreen(uint8_t x, uint8_t y, uint8_t g); // Set the green value of a pixel
        void SetPixalBlue(uint8_t x, uint8_t y, uint8_t b); // Set the blue value of a pixel

        bool Save_png(std::string filePath); // Save the image to a png file
        bool Read_png(std::string filePath); // Read the image from a png file

        bool Save_jpeg(std::string filename, int quality); // Save the image to a jpg file
        int Read_jpeg_pub(std::string infilename);

        ~Image(); // Free memory
};