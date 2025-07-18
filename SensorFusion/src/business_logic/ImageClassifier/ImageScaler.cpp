#include "ImageScaler.h"
#include "services/Logger/LoggerMacros.h"
#include "stb/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize2.h"

namespace business_logic
{
namespace ImageClassifier
{

void ImageScaler::initialization()
{

}

void ImageScaler::rgb2Gray(const uint8_t* input)
{
//    for (int i = 0; i < m_width * m_height; ++i)
//    {
//        int r = m_inputImage[i * 3 + 0];
//        int g = m_inputImage[i * 3 + 1];
//        int b = m_inputImage[i * 3 + 2];
//        // Conversion estandar luminosidad perceptual
//        m_inputImage[i] = static_cast<unsigned char>(0.299 * r + 0.587 * g + 0.114 * b);
//    }
}

std::vector<uint8_t> ImageScaler::scaleImage( ScaleTipe type, const uint8_t* input, int inputWidth, int inputHeight, int outputWidth, int outputHeight)
{
    std::vector<uint8_t> output_pixels(outputWidth * outputHeight);

    stbir_resize_uint8_linear(
        input, inputWidth, inputHeight, 0,
        output_pixels.data(), outputWidth, outputHeight, 0,
        STBIR_1CHANNEL
    );

    return output_pixels;
}


}
}
