
class REFL_API RStdMapContainer : public RType
{
public:
    static constexpr int32_t InvalidIndex = -1;
    RStdMapContainer(const std::string& InName)
        : RType(InName)
    {}

    virtual bool Add(void const* BasePtr, void* KeyPtr, void* ValuePtr) = 0;
    virtual void Remove(void const* BasePtr, void* KeyPtr) = 0;
    virtual void* Find(void const* BasePtr, void* KeyPtr) = 0;
    virtual int32_t GetSize(void const* BasePtr) = 0;
    virtual bool IsEmpty(void const* BasePtr) = 0;
    virtual void Clear(void const* BasePtr) = 0;

    RType* GetKeyType() { return KeyType; }
    RType* GetValueType() { return ValueType; }
protected:
    void SetKeyType(RType* InKeyType) { KeyType = InKeyType; }
    void SetValueType(RType* InValueType) { ValueType = InValueType; }

private:
    RType* KeyType;
    RType* ValueType;
};

template<typename AKeyType, typename AValueType>
class TStdMapContainer : public RStdMapContainer
{
public:
    using TStdMap = std::map<AKeyType, AValueType>;

    TStdMapContainer()
        : RStdMapContainer(typeid(TStdMap).name())
    {
        SetKeyType(GetReflType<AKeyType>());
        SetValueType(GetReflType<AValueType>());
    }

    TStdMap& ContainerRef(void const* BasePtr) { return *(TStdMap*)BasePtr; }

    virtual bool Add(void const* BasePtr, void* KeyPtr, void* ValuePtr) override
    {
        TStdMap& StdMap = ContainerRef(BasePtr);
        AKeyType& Key = *(AKeyType*)KeyPtr;
        AValueType& Value = *(AValueType*)ValuePtr;
        auto Result = StdMap.insert(std::make_pair(Key, Value));
        return Result.second;
    }
    virtual void Remove(void const* BasePtr, void* KeyPtr) override
    {
        TStdMap& StdMap = ContainerRef(BasePtr);
        AKeyType& Key = *(AKeyType*)KeyPtr;
        StdMap.erase(Key);
    }
    virtual void* Find(void const* BasePtr, void* KeyPtr) override
    {
        TStdMap& StdMap = ContainerRef(BasePtr);
        AKeyType& Key = *(AKeyType*)KeyPtr;
        auto It = StdMap.find(Key);
        if (It != StdMap.end()) return &It->second;
        return nullptr;
    }
    virtual int32_t GetSize(void const* BasePtr) override
    {
        TStdMap& StdMap = ContainerRef(BasePtr);
        return static_cast<int32_t>(StdMap.size());
    }
    virtual bool IsEmpty(void const* BasePtr) override
    {
        TStdMap& StdMap = ContainerRef(BasePtr);
        return StdMap.empty();
    }
    virtual void Clear(void const* BasePtr) override
    {
        TStdMap& StdMap = ContainerRef(BasePtr);
        return StdMap.clear();
    }

    REFL_TYPE_OPERATOR_FUNCTION_IMPL(TStdMap)
};
