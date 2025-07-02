#pragma once

#include <business_logic/Osal/StreamBufferHandler.h>


#include "business_logic/ImageClassifier/ImageProvider.h"

#include "tensorflow/lite/micro/examples/person_detection/detection_responder.h"
#include "tensorflow/lite/micro/examples/person_detection/image_provider.h"
#include "tensorflow/lite/micro/examples/person_detection/model_settings.h"
#include "tensorflow/lite/micro/examples/person_detection/person_detect_model_data.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"


#include <memory>

namespace business_logic
{
namespace ImageClassifier
{

class ImageClassifierManager
{
private:

	tflite::ErrorReporter* error_reporter = nullptr;
	const tflite::Model* model = nullptr;
	tflite::MicroInterpreter* interpreter = nullptr;
	TfLiteTensor* input = nullptr;

	// An area of memory to use for input, output, and intermediate arrays.
	constexpr static int kTensorArenaSize = 256 * 1024;
	static inline uint8_t tensor_arena[kTensorArenaSize];
	std::shared_ptr<ImageProvider> m_imageProvider;

public:
	ImageClassifierManager(const std::shared_ptr<ImageProvider>& imageProvider);
	virtual ~ImageClassifierManager() = default;
	void initialization();

	void performInference();
};
}
}
