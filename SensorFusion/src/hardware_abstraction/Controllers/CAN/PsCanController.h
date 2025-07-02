#pragma once
#include <business_logic/Communication/ICommunication.h>
#include <hardware_abstraction/Controllers/CAN/CanFrame.h>
#include "sleep.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xcanps.h"
#include "business_logic/Osal/MutexHandler.h"
#include <memory>
#include "xscugic.h"
#include <vector>

namespace hardware_abstraction
{
namespace Controllers
{
const uint8_t XCANPS_MAX_FRAME_SIZE_IN_WORDS = 32;

class PsCanController
{
private:
	int m_deviceId;
	bool m_initialized;
	XCanPs_Config *m_config;

	static inline XCanPs m_canPs;

	XScuGic_Config *m_irqCtrlConfig;
	CanTimingParams m_timingConfiguration;
	std::shared_ptr<business_logic::Osal::MutexHandler> canMutex;
	static inline volatile int m_loopbackError = false;
	static inline volatile int m_recvDone = false;
	static inline volatile int m_sendDone = false;
	static inline bool m_endOfImage;
	static inline uint8_t m_errorCounter = 0;
	static inline uint32_t m_txFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
	static inline uint32_t m_rxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
	static inline std::vector<CanFrame> m_rxMsgVector;
	static inline TaskHandle_t m_receiveTaskToNotify;


	void modifyRegister(uint8_t reg, uint8_t mask, uint8_t value);
	void writeRegister(uint8_t reg, uint8_t *data, uint32_t nData);
	void clearRegister(uint8_t reg, uint32_t nData);
	void configureTransceiver(CanPsMode mode);
	void transmit(CanFrame msg);
	int setupInterruptSystem(XScuGic *IntcInstancePtr, XCanPs *CanInstancePtr, u16 CanIntrId);
public:
	PsCanController();
	virtual ~PsCanController();

	void initialize();
	bool transmitMsg(uint8_t idMsg, uint8_t *TxMsg, uint8_t msgLength);
	std::vector<CanFrame> receiveMsg(bool& assembleFrame, bool& endOfImage);
	void clearBuffer();
	bool selfTest();
	void resetController();
	static void sendHandler(void *CallBackRef);
	static void recvHandler(void *CallBackRef);
	static void errorHandler(void *CallBackRef, u32 ErrorMask);
	static void eventHandler(void *CallBackRef, u32 IntrMask);
	void registerTaskToNotify(const TaskHandle_t& xReceiveTaskToNotify);

};
}
}
