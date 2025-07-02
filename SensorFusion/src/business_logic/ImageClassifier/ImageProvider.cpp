#include "ImageProvider.h"
#include "services/Logger/LoggerMacros.h"

namespace business_logic
{
namespace ImageClassifier
{
ImageProvider::ImageProvider(const std::shared_ptr<business_logic::ImageAssembler::SharedImage>& sharedImg) : m_sharedImage(sharedImg)
{

}

void ImageProvider::initialization()
{

}

bool ImageProvider::loadImage(unsigned char* imageBuffer, int imageWidth, int imageHeight)
{
	m_sharedImage->setImage(imageBuffer, imageWidth, imageHeight);
}

bool ImageProvider::retrieveSharedImage(const uint8_t*& imageBuffer, std::vector<uint8_t>& scaledImageBuffer, int expectedImageWidth, int expectedImageHeight, bool scaleImage)
{
    int imageWidth = 0;
    int imageHeight = 0;

    imageBuffer = m_sharedImage->getSharedImage(imageWidth, imageHeight);

    if ((expectedImageWidth != imageWidth) || (expectedImageHeight != imageHeight)) {
        LOG_ERROR("Image size mismatch");
        return false;
    }

    if (scaleImage && imageBuffer) {
        int scaledImageWidth = 96;
        int scaledImageHeight = 96;

        scaledImageBuffer = m_imageScaler->scaleImage(
            ScaleTipe::Bilinear,
            imageBuffer,
            imageWidth, imageHeight,
            scaledImageWidth, scaledImageHeight
        );
    }

    return true;
}


void ImageProvider::getImage()
{

}

void ImageProvider::preprocessImage()
{

}

}
}
