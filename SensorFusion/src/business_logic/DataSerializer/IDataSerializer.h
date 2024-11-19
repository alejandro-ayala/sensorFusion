#pragma once
#include <business_logic/DataSerializer/ISerializableMessage.h>

namespace business_logic
{

class IDataSerializer
{
public:
	virtual void deserialize(ISerializableMessage& msg, const std::vector<uint8_t>& data) = 0;
	virtual void serialize(const ISerializableMessage& msg, std::vector<uint8_t>& msgSerialized) const = 0;

};

}
