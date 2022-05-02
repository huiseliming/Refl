
class REFL_API RStdVectorContainer : public RType
{
public:
    static constexpr int32_t InvalidIndex = -1;
    RStdVectorContainer(const std::string& InName = "")
        : RType(InName)
    {}

    virtual void Add(void const* BasePtr, void* ElementPtr = nullptr) = 0;
    //virtual void Remove(void const* BasePtr, void* ElementPtr) = 0;
    virtual void Remove(void const* BasePtr, int32_t Index) = 0;
    virtual void Resize(void const* BasePtr, int32_t Size) = 0;
    //virtual void* Find(void const* BasePtr, void* ElementPtr) = 0;
    //virtual int32_t FindElementIndex(void const* BasePtr, void* ElementPtr) = 0;
    virtual void* GetElement(void const* BasePtr, int32_t Index) = 0;
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
class TStdVectorContainer : public RStdVectorContainer
{
public:
    using TStdVector = std::vector<AElementType>;

    TStdVectorContainer()
        : RStdVectorContainer(typeid(TStdVector).name())
    {
        SetElementType(GetReflType<AElementType>());
    }

    TStdVector& ContainerRef(void const* BasePtr) { return *(TStdVector*)BasePtr; }

    virtual void Add(void const* BasePtr, void* ElementPtr) override
    {
        TStdVector& StdVector = ContainerRef(BasePtr);
        if (ElementPtr == nullptr)
        {
            StdVector.push_back({});
        }
        else
        {
            AElementType& Element = *(AElementType*)ElementPtr;
            StdVector.push_back(Element);
        }
    }
    //virtual void Remove(void const* BasePtr, void* ElementPtr) override
    //{
    //    TStdVector& StdVector = ContainerRef(BasePtr);
    //    AElementType& Element = *(AElementType*)ElementPtr;
    //    StdVector.erase(std::remove(StdVector.begin(), StdVector.end(), Element), StdVector.end());
    //}
    virtual void Remove(void const* BasePtr, int32_t Index) override
    {
        TStdVector& StdVector = ContainerRef(BasePtr);
        assert(StdVector.size() > Index);
        StdVector.erase(StdVector.begin() + Index);
    }
    virtual void Resize(void const* BasePtr, int32_t Size) override
    {
        TStdVector& StdVector = ContainerRef(BasePtr);
        StdVector.resize(Size);
    }
    //virtual void* Find(void const* BasePtr, void* ElementPtr) override
    //{
    //    TStdVector& StdVector = ContainerRef(BasePtr);
    //    AElementType& Element = *(AElementType*)ElementPtr;
    //    auto It = std::find_if(StdVector.begin(), StdVector.end(), [&] (AElementType& Ref) { return Ref == Element; });
    //    if(It != StdVector.end()) return &(*It);
    //    return nullptr;
    //}
    //virtual int32_t FindElementIndex(void const* BasePtr, void* ElementPtr) override
    //{
    //    TStdVector& StdVector = ContainerRef(BasePtr);
    //    AElementType& Element = *(AElementType*)ElementPtr;
    //    for (size_t i = 0; i < StdVector.size(); i++)
    //    {
    //        if (StdVector[i] == Element)
    //        {
    //            return static_cast<int32_t>(i);
    //        }
    //    }
    //    return InvalidIndex;
    //}
    virtual void* GetElement(void const* BasePtr, int32_t Index) override
    {
        TStdVector& StdVector = ContainerRef(BasePtr);
        assert(StdVector.size() > Index);
        return &StdVector.at(Index);
    }
    virtual int32_t GetSize(void const* BasePtr) override
    {
        TStdVector& StdVector = ContainerRef(BasePtr);
        return static_cast<int32_t>(StdVector.size());
    }
    virtual bool IsEmpty(void const* BasePtr) override
    {
        TStdVector& StdVector = ContainerRef(BasePtr);
        return StdVector.empty();
    }
    virtual void Clear(void const* BasePtr) override
    {
        TStdVector& StdVector = ContainerRef(BasePtr);
        return StdVector.clear();
    }
    REFL_TYPE_OPERATOR_FUNCTION_IMPL(TStdVector)
};
