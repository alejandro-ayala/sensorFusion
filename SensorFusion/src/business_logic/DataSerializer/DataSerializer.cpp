#include "DataSerializer.h"

namespace business_logic
{

void DataSerializer::serialize(const ISerializableMessage& msg, std::vector<uint8_t>& msgSerialized) const
{
    msg.serialize(msgSerialized);
}

void DataSerializer::deserialize(ISerializableMessage& msg, const std::vector<uint8_t>& data)
{
    msg.deserialize(data);
}
}


