#pragma once
#include "business_logic/Osal/MutexHandler.h"

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
    unsigned char* getImageCopy(int& outW, int& outH);

private:
    unsigned char* m_imageData = nullptr;
    int m_width = 0;
    int m_height = 0;
    std::shared_ptr<business_logic::Osal::MutexHandler> m_mutex;
};
}
}
