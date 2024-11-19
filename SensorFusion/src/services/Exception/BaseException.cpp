#include "BaseException.h"

namespace services
{
const char* BaseException::what() const noexcept
{
	return m_errorMsg.c_str();
}

uint32_t BaseException::getErrorId() const
{
	return m_errorId;
}

}
