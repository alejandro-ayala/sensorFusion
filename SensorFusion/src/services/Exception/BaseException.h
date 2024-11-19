 #pragma once
#include <iostream>
#include <exception>

namespace services
{

enum ErrorIdOffset
{
	ControllersLayer = 0x100,
	DevicesLayer     = ControllersLayer + 0x200,
	BusinessLogicLayer = DevicesLayer     + 0x200,
	ServicesLayer    = BusinessLogicLayer + 0x200
};


class BaseException : public std::exception {
public:
	BaseException(const uint32_t errorID, const std::string& errorMsg) : m_errorId(errorID), m_errorMsg(errorMsg) {};
    virtual ~BaseException() = default;
	virtual const char* what() const noexcept override;
    uint32_t getErrorId() const;

private:
	uint32_t    m_errorId;
	std::string m_errorMsg;
};
}
