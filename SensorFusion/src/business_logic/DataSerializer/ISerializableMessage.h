#pragma once

#include <cstdint>
#include <vector>
#include "json/include/nlohmann/json.hpp"

namespace business_logic
{

class ISerializableMessage
{
private:

public:
    virtual ~ISerializableMessage() = default;
    virtual void serialize(std::vector<uint8_t>& serializedMsg) const = 0;
    virtual void deserialize(const std::vector<uint8_t>& data) = 0;
};
}


