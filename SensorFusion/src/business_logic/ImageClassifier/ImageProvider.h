#pragma once

#include <business_logic/Osal/StreamBufferHandler.h>
#include "business_logic/ImageClassifier/ImageScaler.h"
#include "business_logic/ImageAssembler/SharedImage.h"
#include <memory>

namespace business_logic
{
namespace ImageClassifier
{

class ImageProvider
{
private:
	std::unique_ptr<ImageScaler> m_imageScaler;
	std::shared_ptr<business_logic::ImageAssembler::SharedImage> m_sharedImage;

	void preprocessImage();
    std::vector<uint8_t> m_inputImage;
    std::vector<uint8_t> m_outputImage;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
public:
	ImageProvider(const std::shared_ptr<business_logic::ImageAssembler::SharedImage>& sharedImg);
	virtual ~ImageProvider() = default;
	void initialization();
	bool loadImage(unsigned char* imageBuffer, int imageWidth, int imageHeight);
	bool retrieveSharedImage(unsigned char* imageBuffer, int imageWidth, int imageHeight, bool scaleImage);
	void getImage();
};
}
}
