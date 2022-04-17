#pragma once
#include "ReflType.h"

class REFL_API RStdSetContainer : public RType
{
public:
    static constexpr int32_t InvalidIndex = -1;
    RStdSetContainer(const std::string& InName)
        : RType(InName)
    {}

    virtual void Add(void const* BasePtr, void* ElementPtr) = 0;
    virtual void Remove(void const* BasePtr, void* ElementPtr) = 0;
    virtual void* Find(void const* BasePtr, void* ElementPtr) = 0;
    virtual int32_t GetSize(void const* BasePtr) = 0;
    virtual bool IsEmpty(void const* BasePtr) = 0;
    virtual void Clear(void const* BasePtr) = 0;

    RType* GetElementType() { return ElementType; }

protected:
    void SetElementType(RType* InElementType) { ElementType = InElementType; }

private:
    RType* ElementType;
};

template<typename AElementType>
class TStdSetContainer : public RStdSetContainer
{
public:
    using TStdSet = std::set<AElementType>;

    TStdSetContainer()
        : RStdSetContainer(typeid(TStdSet).name())
    {
        SetElementType(GetReflType<AElementType>());
    }

    TStdSet& ContainerRef(void const* BasePtr) { return *(TStdSet*)BasePtr; }

    virtual void Add(void const* BasePtr, void* ElementPtr) override
    {
        TStdSet& StdSet = ContainerRef(BasePtr);
        AElementType& Element = *(AElementType*)ElementPtr;
        StdSet.insert(Element);
    }
    virtual void Remove(void const* BasePtr, void* ElementPtr) override
    {
        TStdSet& StdSet = ContainerRef(BasePtr);
        AElementType& Element = *(AElementType*)ElementPtr;
        StdSet.erase(Element);
    }
    virtual void* Find(void const* BasePtr, void* ElementPtr) override
    {
        TStdSet& StdSet = ContainerRef(BasePtr);
        AElementType& Element = *(AElementType*)ElementPtr;
        auto It = StdSet.find(Element);
        if (It != StdSet.end()) return const_cast<int*>(&(*It));
        return nullptr;
    }
    virtual int32_t GetSize(void const* BasePtr) override
    {
        TStdSet& StdSet = ContainerRef(BasePtr);
        return static_cast<int32_t>(StdSet.size());
    }
    virtual bool IsEmpty(void const* BasePtr) override
    {
        TStdSet& StdSet = ContainerRef(BasePtr);
        return StdSet.empty();
    }
    virtual void Clear(void const* BasePtr) override
    {
        TStdSet& StdSet = ContainerRef(BasePtr);
        return StdSet.clear();
    }

    REFL_TYPE_IMPL_VIRTUAL_FUNCTION(TStdSet)
};
