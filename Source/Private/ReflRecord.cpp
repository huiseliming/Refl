#include "ReflRecord.h"

int32_t RRecord::IdCounter{ 0 };
std::vector<RRecord*> RRecord::Records = { nullptr };

RRecord* RRecord::FindRecord(int32_t Id)
{
    return Records[Id];
}
