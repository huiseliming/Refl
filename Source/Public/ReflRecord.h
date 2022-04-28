#pragma once
#include "ReflExport.h"
#include "ReflModule.h"

class REFL_API RRecord
{
public:
    RRecord(const std::string& InName = "")
        : Name(InName)
    {}

    const std::string& GetName() { return Name; }

protected:
    virtual void Register()
    {
        assert(IReflModule::Ref().IsMainThread());
        Id = ++IdCounter;
        Records.push_back(this);
    }

    virtual void Deregister()
    {
        assert(IReflModule::Ref().IsMainThread());
        Id = 0;
        Records[Id] = nullptr;
    }

    void AddMetadata(const std::string& InKey, const std::string& InValue)
    {
        Metadata.insert(std::make_pair(InKey, InValue));
    }

    template<typename Iterator>
    typename std::iterator_traits<Iterator>::value_type
        sum(Iterator begin, Iterator end)
    {
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        value_type s = value_type();
        for (Iterator it = begin; it != end; it++) {
            s += *it;
        }
        return s;
    }

    template<typename AIterator>
    void AddMetadata(AIterator Begin, AIterator End)
    {
        Metadata.insert(Begin, End);
    }

    const std::string& GetMetadata(const std::string& InKey)
    {
        auto It = Metadata.find(InKey);
        if (It != Metadata.end()) return It->second;
        return IStaticVariable::EmptyString;
    }

    /**
     * 如果存在,获取指向的下一级
     */
    RRecord* NextNode{ nullptr };
private:
    int32_t Id{ 0 };
    std::string Name;
    std::unordered_map<std::string, std::string> Metadata;
private:
    template<typename T> friend struct TRecordIterator;
    template<class T> friend struct TStaticClass;

public:
    static RRecord* FindRecord(int32_t Id);

private:
    /**
     * Id计数器
     */
    static int32_t IdCounter;
    /**
     * 为所有Refl记录保存运行时Id
     */
    static std::vector<RRecord*> Records;
};

template<typename T>
struct TRecordIterator
{
    TRecordIterator(T* InRecord)
        : Record(InRecord)
    {
    }
    static_assert(std::is_base_of_v<RRecord, T>);

    T* operator++()
    {
        RRecord* Ret = Record;
        Record = Record->NextNode;
        return static_cast<T*>(Ret);
    }

    operator bool()
    {
        return Record != nullptr;
    }

    T* Ptr()
    {
        return static_cast<T*>(Record);
    }

    RRecord* Record{nullptr};
};
