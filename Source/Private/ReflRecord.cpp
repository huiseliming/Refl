#include "ReflRecord.h"

std::atomic<int32_t> RRecord::IdCounter{ 0 };
std::vector<RRecord*> RRecord::Records = { nullptr };

bool RRecord::ContainsMetadata(const std::string& InKey)
{
    return Metadata.contains(InKey);
}

RRecord* RRecord::FindRecord(int32_t Id)
{
    return Records[Id];
}