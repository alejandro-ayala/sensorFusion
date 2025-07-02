#pragma once
#include "business_logic/Osal/MutexHandler.h"

#include <array>
#include <memory>
namespace business_logic
{
namespace ImageAssembler
{
class SharedImage {
public:
    SharedImage();
    ~SharedImage();
    void setImage(unsigned char* newImage, int w, int h);
    const uint8_t* getSharedImage(int& outW, int& outH);
    static constexpr uint32_t RAW_IMAGE_SIZE = 76800;
private:
    //unsigned char* m_imageData = nullptr;
    std::array<uint8_t, RAW_IMAGE_SIZE> m_imageData;
    int m_width = 0;
    int m_height = 0;
    std::shared_ptr<business_logic::Osal::MutexHandler> m_mutex;
};
}
}
