#pragma once

#include <business_logic/Osal/StreamBufferHandler.h>
#include <vector>

namespace business_logic
{
namespace ImageClassifier
{
enum class ScaleTipe
{
	Bilinear
};

class ImageScaler
{
private:
	std::vector<uint8_t> m_inputImage;
    std::vector<uint8_t> m_outputImage;
    void rgb2Gray(const uint8_t* input);
public:
	explicit ImageScaler() = default;
	virtual ~ImageScaler() = default;
	void initialization();
	uint8_t* scaleImage(ScaleTipe type, const uint8_t* input, int inputWidth, int inputHeight, int outputWidth, int outputHeight);
};
}
}
