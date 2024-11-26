#pragma once

#include "IDataSerializer.h"
#include "ISerializableMessage.h"

namespace business_logic
{

class DataSerializer : public IDataSerializer
{
private:

public:
	DataSerializer() = default;
	virtual ~DataSerializer() = default;
	void deserialize(ISerializableMessage& msg, const std::vector<uint8_t>& data) override;
	void serialize(const ISerializableMessage& msg, std::vector<uint8_t>& msgSerialized) const override;
};
}
