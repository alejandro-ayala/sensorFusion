#include "SharedImage.h"
#include "services/Logger/LoggerMacros.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include <cstring>

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
	if (m_imageData.data())
	{
		stbi_image_free(m_imageData.data());
	}
}

void SharedImage::setImage(unsigned char* newImage, int w, int h)
{
	m_mutex->lock();
	if (m_imageData.data())
	{
		//stbi_image_free(m_imageData.data());
		LOG_WARNING("SharedImage::setImage free previous shared image buffer");
	}
    size_t size = static_cast<size_t>(w) * h;
    if (size > m_imageData.size())
    {
        throw std::runtime_error("Buffer size exceeds destination array");
        LOG_ERROR("SharedImage::setImage Buffer size exceeds destination array");
    }
    else
    {
        std::memcpy(m_imageData.data(), newImage, size);
    	m_width = w;
    	m_height = h;
    }

	m_mutex->unlock();
}

const uint8_t* SharedImage::getSharedImage(int& outW, int& outH)
{
	m_mutex->lock();
	outW = m_width;
	outH = m_height;
	m_mutex->unlock();

	return m_imageData.data();;
}
}
}
