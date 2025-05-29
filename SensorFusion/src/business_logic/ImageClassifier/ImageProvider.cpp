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

bool ImageProvider::retrieveSharedImage(unsigned char* imageBuffer, int expectedImageWidth, int expectedImageHeight, bool scaleImage)
{
	int imageWidth = 0;
	int imageHeight = 0;
    imageBuffer = m_sharedImage->getImageCopy(imageWidth, imageHeight);
    bool result;
    if((expectedImageWidth != imageWidth) || (expectedImageHeight != imageHeight))
    {
    	const std::string erroMsg = "ImageProvider::retrieveSharedImage expected image size: " + std::to_string(expectedImageWidth) + "x" + std::to_string(expectedImageHeight) + " does not match with retrieved size: " + std::to_string(imageWidth) + "x" + std::to_string(imageHeight);
    	LOG_ERROR(erroMsg);
    	result = false;
    }
    else
    {
        if(scaleImage && imageBuffer)
        {
        	int scaledImageWidth = 96;
        	int scaledImageHeight = 96;
        	imageBuffer = m_imageScaler->scaleImage(ScaleTipe::Bilinear, imageBuffer, imageWidth, imageHeight, scaledImageWidth, scaledImageHeight);
        }
        result = (imageBuffer != nullptr);
    }

    return result;
}

void ImageProvider::getImage()
{

}

void ImageProvider::preprocessImage()
{

}

}
}
