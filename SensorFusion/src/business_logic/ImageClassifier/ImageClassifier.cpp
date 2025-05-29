#include <business_logic/ImageClassifier/ImageClassifier.h>
#include "services/Logger/LoggerMacros.h"

namespace business_logic
{
namespace ImageClassifier
{
ImageClassifierManager::ImageClassifierManager(const std::shared_ptr<ImageProvider>& imageProvider) : m_imageProvider(imageProvider)
{

}

void ImageClassifierManager::initialization()
{

	  // Set up logging. Google style is to avoid globals or statics because of
	  // lifetime uncertainty, but since this has a trivial destructor it's okay.
	  // NOLINTNEXTLINE(runtime-global-variables)
	  static tflite::MicroErrorReporter micro_error_reporter;
	  error_reporter = &micro_error_reporter;

	  // Map the model into a usable data structure. This doesn't involve any
	  // copying or parsing, it's a very lightweight operation.
	  model = tflite::GetModel(g_person_detect_model_data);
	  if (model->version() != TFLITE_SCHEMA_VERSION) {
	    TF_LITE_REPORT_ERROR(error_reporter,
	                         "Model provided is schema version %d not equal "
	                         "to supported version %d.",
	                         model->version(), TFLITE_SCHEMA_VERSION);
	    return;
	  }

	  // Pull in only the operation implementations we need.
	  // This relies on a complete list of all the ops needed by this graph.
	  // An easier approach is to just use the AllOpsResolver, but this will
	  // incur some penalty in code space for op implementations that are not
	  // needed by this graph.
	  //
	  // tflite::ops::micro::AllOpsResolver resolver;
	  // NOLINTNEXTLINE(runtime-global-variables)
	  static tflite::MicroOpResolver<3> micro_op_resolver;
	  micro_op_resolver.AddBuiltin(
	      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
	      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
	  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
	                               tflite::ops::micro::Register_CONV_2D());
	  micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_AVERAGE_POOL_2D,
	                               tflite::ops::micro::Register_AVERAGE_POOL_2D());

	  // Build an interpreter to run the model with.
	  static tflite::MicroInterpreter static_interpreter(
	      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
	  interpreter = &static_interpreter;

	  // Allocate memory from the tensor_arena for the model's tensors.
	  TfLiteStatus allocate_status = interpreter->AllocateTensors();
	  if (allocate_status != kTfLiteOk) {
	    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
	    return;
	  }

	  // Get information about the memory area to use for the model's input.
	  input = interpreter->input(0);
}

void ImageClassifierManager::performInference()
{
	  // Get image from provider.
	constexpr uint16_t rawImageWidth = 320;
	constexpr uint16_t rawImageHeight = 240;

	LOG_INFO("ImageClassifierManager::performInference triggered");
	std::array<uint8_t, rawImageWidth*rawImageHeight> rawImage;
	const bool scaleImage = true;
	const auto validImage = m_imageProvider->retrieveSharedImage(rawImage.data(), rawImageWidth, rawImageHeight, scaleImage);
	if(validImage)
	{
		input->data.uint8 = rawImage.data();
		LOG_INFO("ImageClassifierManager::performInference raw image retrieved. Performing scaled");
	}
//	if (kTfLiteOk != GetImage(error_reporter, kNumCols, kNumRows, kNumChannels, input->data.uint8))
//	{
//		TF_LITE_REPORT_ERROR(error_reporter, "Image capture failed.");
//	}

	// Run the model on this input and make sure it succeeds.
	if (kTfLiteOk != interpreter->Invoke())
	{
		TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed.");
	}

	TfLiteTensor* output = interpreter->output(0);

	// Process the inference results.
	uint8_t person_score = output->data.uint8[kPersonIndex];
	uint8_t no_person_score = output->data.uint8[kNotAPersonIndex];
	RespondToDetection(error_reporter, person_score, no_person_score);
	LOG_INFO("ImageClassifierManager::performInference finished");
}
}
}
