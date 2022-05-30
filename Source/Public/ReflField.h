#pragma once
#include "ReflRecord.h"
#include "ReflType.h"

enum EReflFieldFlag {
    RPF_NoFlag = 0x00000000,
};

class RStruct;
class RClass;

class REFL_API RField : public RRecord
{
public:
    RField(const std::string& InName = "", uint32_t InOffset = 0)
        : RRecord(InName)
        , Offset(InOffset)
    {}
    ~RField() = default;
    RField& operator=(const RField&) = default;
    RField& operator=(RField&&) = default;

    RField* GetNextField() { return static_cast<RField*>(NextNode); }


    void* GetRowPtr(void const* BasePtr) const { return const_cast<char *>(static_cast<char const*>(BasePtr) + Offset); }
    template<typename TValue>
    TValue* GetValuePtr(void const* BasePtr) const
    { 
        static_assert(!std::is_reference_v<TValue>);
        return static_cast<TValue*>(GetRowPtr()); 
    }
    
    template<typename TValue>
    void SetValue(void const* BasePtr, TValue& ValueRef)
    {
        static_assert(!std::is_reference_v<TValue>);
    }
    // BoolField
    virtual void SetBool(void* BasePtr, bool Value) const {}
    virtual bool GetBool(void const* BasePtr) const { return false; }
    virtual std::string GetBoolToString(void const* BasePtr) const { return ""; }
    // NumericField
    virtual void SetUInt(void* BasePtr, uint64_t Value) const {}
    virtual void SetSInt(void* BasePtr, int64_t Value) const {}
    virtual void SetFloat(void* BasePtr, double Value) const {}
    virtual void SetNumericFromString(void* BasePtr, char const* Value) const {}
    virtual std::string GetNumericToString(void const* BasePtr) const { return ""; }
    virtual int64_t GetSInt(void const* BasePtr) const { return 0; }
    virtual uint64_t GetUInt(void const* BasePtr) const { return 0; }
    virtual double GetFloat(void const* BasePtr) const { return 0.F; }
    // StringField
    virtual std::string GetString(void const* BasePtr) const { return ""; }
    virtual const char* GetCString(void const* BasePtr) const { return ""; }
    virtual void SetString(void* BasePtr, const std::string& Value) const {}
    virtual void SetString(void* BasePtr, const char* Value) const {}
    virtual void SetString(void* BasePtr, uint64_t Value) const {}
    virtual void SetString(void* BasePtr, int64_t Value) const {}
    virtual void SetString(void* BasePtr, double Value) const {}

    RType* GetType() { return Type; }
    RClass* GetClass() { return (RClass*)Type; }

protected:
    void SetNextField(RField* InField) { NextNode = InField; }
    void SetType(RType* InType) { Type = InType; }

    template<typename T> friend RField* NewField(const std::string&, uint32_t);
private:
    friend class RClass;
    RType* Type{ nullptr };
    uint32_t Offset{ 0 };
};

template<typename T>
struct TFieldAccessor
{
    enum
    {
        CppSize = sizeof(T),
        CppAlignment = alignof(T)
    };

    static T const* GetPtr(void const* A) { return (T const*)(A); }
    static T* GetPtr(void* A) { return (T*)(A); }
    static T const& Get(void const* A) { return *GetPtr(A); }

    static T GetDefault() { return T(); }
    static T GetOptional(void const* A) { return A ? Get(A) : GetDefault(); }

    static void Set(void* A, T const& Value) { *GetPtr(A) = Value; }

    static T* Initialize(void* A) { return new (A) T(); }
    static void Deinitialize(void* A) { GetPtr(A)->~T(); }
    static void CopyAssign(void* A, void* B) { *GetPtr(A) = *GetPtr(B); }
    static void MoveAssign(void* A, void* B) { *GetPtr(A) = std::move(*GetPtr(B)); }
};


class REFL_API RBoolField : public RField
{
    using IFieldAccessor = TFieldAccessor<bool>;
public:
    RBoolField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}

    virtual void SetBool(void* BasePtr, bool Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), (bool)Value);
    }
    virtual bool GetBool(void const* BasePtr) const override
    {
        return IFieldAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual std::string GetBoolToString(void const* BasePtr) const
    {
        return GetBool(BasePtr) ? "True" : "False";
    }
};


template <typename T>
class TNumericField : public RField
{
    using IFieldAccessor = TFieldAccessor<T>;
public:
    TNumericField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}

    virtual void SetUInt(void* BasePtr, uint64_t Value) const override
    {
        assert(std::is_integral_v<T>);
        IFieldAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetSInt(void* BasePtr, int64_t Value) const override
    {
        assert(std::is_integral_v<T>);
        IFieldAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetFloat(void* BasePtr, double Value) const override
    {
        assert(std::is_floating_point_v<T>);
        IFieldAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetNumericFromString(void* BasePtr, char const* Value) const override
    {
        *IFieldAccessor::GetPtr(GetRowPtr(BasePtr)) = (T)atof(Value);
    }
    virtual std::string GetNumericToString(void const* BasePtr) const override
    {
        return std::to_string(IFieldAccessor::Get(GetRowPtr(BasePtr)));
    }
    virtual int64_t GetSInt(void const* BasePtr) const override
    {
        assert(std::is_integral_v<T>);
        return (int64_t)IFieldAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual uint64_t GetUInt(void const* BasePtr) const override
    {
        assert(std::is_integral_v<T>);
        return (uint64_t)IFieldAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual double GetFloat(void const* BasePtr) const override
    {
        assert(std::is_floating_point_v<T>);
        return (double)IFieldAccessor::Get(GetRowPtr(BasePtr));
    }
};

template class REFL_API TNumericField<int8_t>;
template class REFL_API TNumericField<int16_t>;
template class REFL_API TNumericField<int32_t>;
template class REFL_API TNumericField<int64_t>;
template class REFL_API TNumericField<int8_t>;
template class REFL_API TNumericField<int16_t>;
template class REFL_API TNumericField<int32_t>;
template class REFL_API TNumericField<int64_t>;
template class REFL_API TNumericField<float>;
template class REFL_API TNumericField<double>;

using RSInt8Field = TNumericField<int8_t>;
using RSInt16Field = TNumericField<int16_t>;
using RSInt32Field = TNumericField<int32_t>;
using RSInt64Field = TNumericField<int64_t>;
using RUInt8Field = TNumericField<int8_t>;
using RUInt16Field = TNumericField<int16_t>;
using RUInt32Field = TNumericField<int32_t>;
using RUInt64Field = TNumericField<int64_t>;
using RFloatField = TNumericField<float>;
using RDoubleField = TNumericField<double>;

class REFL_API RStringField : public RField
{
    using IFieldAccessor = TFieldAccessor<std::string>;
public:

    RStringField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}

    virtual std::string GetString(void const* BasePtr) const override
    {
        return IFieldAccessor::Get(GetRowPtr(BasePtr));
    }

    virtual const char* GetCString(void const* BasePtr) const override
    {
        return IFieldAccessor::Get(GetRowPtr(BasePtr)).c_str();
    }

    virtual void SetString(void* BasePtr, const std::string& Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), Value);
    }

    virtual void SetString(void* BasePtr, const char* Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), Value);
    }

    virtual void SetString(void* BasePtr, uint64_t Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), std::to_string(Value));
    }

    virtual void SetString(void* BasePtr, int64_t Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), std::to_string(Value));
    }

    virtual void SetString(void* BasePtr, double Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), std::to_string(Value));
    }
};

class REFL_API REnumField : public RField
{
public:
    REnumField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}

};

template<typename T>
class TEnumField : public REnumField
{
    using IFieldAccessor = TFieldAccessor<T>;
public:
    TEnumField(const std::string& InName = "", uint32_t InOffset = 0)
        : REnumField(InName, InOffset)
    {}

    virtual void SetUInt(void* BasePtr, uint64_t Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetSInt(void* BasePtr, int64_t Value) const override
    {
        IFieldAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual int64_t GetSInt(void const* BasePtr) const override
    {
        return (int64_t)IFieldAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual uint64_t GetUInt(void const* BasePtr) const override
    {
        return (uint64_t)IFieldAccessor::Get(GetRowPtr(BasePtr));
    }
};


class REFL_API RClassField : public RField
{
public:
    RClassField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}
    
};

class REFL_API RVectorField : public RField
{
public:
    RVectorField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}
    ~RVectorField() 
    {
        if (ElementField)
        {
            delete ElementField;
            ElementField = nullptr;
        }
    }

    RField* GetElementField()
    {
        return ElementField;
    }

protected:
    RField* ElementField{ nullptr };
};

template<typename T>
class TVectorField : public RVectorField
{
public:
    TVectorField(const std::string& InName = "", uint32_t InOffset = 0)
        : RVectorField(InName, InOffset)
    {
        ElementField = NewField<IsStdVector<T>::ElementType>("", 0);
    }
};

class REFL_API RSetField : public RField
{
public:
    RSetField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}

    ~RSetField()
    {
        if (ElementField)
        {
            delete ElementField;
            ElementField = nullptr;
        }
    }

    RField* GetElementField()
    {
        return ElementField;
    }

protected:
    RField* ElementField{ nullptr };
};

template<typename T>
class TSetField : public RSetField
{
public:
    TSetField(const std::string& InName = "", uint32_t InOffset = 0)
        : RSetField(InName, InOffset)
    {
        ElementField = NewField<IsStdSet<T>::ElementType>("", 0);
    }
};

class REFL_API RMapField : public RField
{
public:
    RMapField(const std::string& InName = "", uint32_t InOffset = 0)
        : RField(InName, InOffset)
    {}
    ~RMapField()
    {
        if (KeyField)
        {
            delete KeyField;
            KeyField = nullptr;
        }
        if (ValueField)
        {
            delete ValueField;
            ValueField = nullptr;
        }
    }
    RField* GetKeyField()
    {
        return KeyField;
    }

    RField* GetValueField()
    {
        return ValueField;
    }

protected:
    RField* KeyField{ nullptr };
    RField* ValueField{ nullptr };
};

template<typename T>
class TMapField : public RMapField
{
public:
    TMapField(const std::string& InName = "", uint32_t InOffset = 0)
        : RMapField(InName, InOffset)
    {
        KeyField = NewField<IsStdMap<T>::KeyType>("", 0);
        ValueField = NewField<IsStdMap<T>::ValueType>("", 0);
    }
};

template<typename T>
constexpr bool NewFieldNotSupported = false;

template<typename T>
RField* NewField(const std::string& InName, uint32_t InOffset)
{
    static_assert(!std::is_reference_v<T>);
    RField* Field = nullptr;
         if constexpr (std::is_same_v<bool, T>        ) Field = new RBoolField(InName, InOffset);
    else if constexpr (std::is_arithmetic_v<T>        ) Field = new TNumericField<T>(InName, InOffset);
    else if constexpr (std::is_same_v<T, std::string> ) Field = new RStringField(InName, InOffset);
    else if constexpr (HasStaticClass<T>::value       ) Field = new RClassField(InName, InOffset);
    else if constexpr (std::is_enum_v<T>              ) Field = new TEnumField<T>(InName, InOffset);
    else if constexpr (IsStdVector<T>::value          ) Field = new TVectorField<T>(InName, InOffset);
    else if constexpr (IsStdSet<T>::value             ) Field = new TSetField<T>(InName, InOffset);
    else if constexpr (IsStdMap<T>::value             ) Field = new TMapField<T>(InName, InOffset);
    else                                                static_assert(NewFieldNotSupported<T> && "UNSUPPORTED TYPE");
    Field->SetType(GetReflType<T>());
    return Field;
}
