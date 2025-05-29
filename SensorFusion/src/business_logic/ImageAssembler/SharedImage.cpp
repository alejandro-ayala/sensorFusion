#include "SharedImage.h"
#include "services/Logger/LoggerMacros.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace business_logic
{
namespace ImageAssembler
{
SharedImage::SharedImage()
{
	m_mutex = std::make_shared<business_logic::Osal::MutexHandler>();
}

SharedImage::~SharedImage()
{
	if (m_imageData)
	{
		stbi_image_free(m_imageData);
	}
}

void SharedImage::setImage(unsigned char* newImage, int w, int h)
{
	m_mutex->lock();
	if (m_imageData)
	{
		stbi_image_free(m_imageData);
		LOG_WARNING("SharedImage::setImage free previous shared image buffer");
	}
	m_imageData = newImage;
	m_width = w;
	m_height = h;
	m_mutex->unlock();
}

unsigned char* SharedImage::getImageCopy(int& outW, int& outH)
{
	unsigned char* result = nullptr;
	m_mutex->lock();
	result = m_imageData; // avoid copy
	outW = m_width;
	outH = m_height;
	m_mutex->unlock();

	return result;
}
}
}
