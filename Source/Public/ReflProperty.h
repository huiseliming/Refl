#pragma once
#include "ReflRecord.h"
#include "ReflClass.h"

class REFL_API RProperty : public RRecord
{
public:
    RProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RRecord(InName)
        , Offset(InOffset)
    {}

    RProperty* GetNextProperty() { return static_cast<RProperty*>(NextNode); }


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
    // BoolProperty
    virtual void SetBool(void* BasePtr, bool Value) const {}
    virtual bool GetBool(void const* BasePtr) const { return false; }
    virtual std::string GetBoolToString(void const* BasePtr) const { return ""; }
    // NumericProperty
    virtual void SetUInt(void* BasePtr, uint64_t Value) const {}
    virtual void SetSInt(void* BasePtr, int64_t Value) const {}
    virtual void SetFloat(void* BasePtr, double Value) const {}
    virtual void SetNumericFromString(void* BasePtr, char const* Value) const {}
    virtual std::string GetNumericToString(void const* BasePtr) const { return ""; }
    virtual int64_t GetSInt(void const* BasePtr) const { return 0; }
    virtual uint64_t GetUInt(void const* BasePtr) const { return 0; }
    virtual double GetFloat(void const* BasePtr) const { return 0.F; }
    // StringProperty
    virtual std::string GetString(void const* BasePtr) const { return ""; }
    virtual const char* GetCString(void const* BasePtr) const { return ""; }
    virtual void SetString(void* BasePtr, const std::string& Value) const {}
    virtual void SetString(void* BasePtr, const char* Value) const {}
    virtual void SetString(void* BasePtr, uint64_t Value) const {}
    virtual void SetString(void* BasePtr, int64_t Value) const {}
    virtual void SetString(void* BasePtr, double Value) const {}

    RType* GetType() { return Type; }
    RClass* GetClass() { return static_cast<RClass*>(Type); }

protected:
    void SetNextProperty(RProperty* InProperty) { NextNode = InProperty; }
    void SetType(RType* InType) { Type = InType; }

    template<typename T> friend RProperty* NewProperty(const std::string&, uint32_t);
private:
    friend class RClass;
    RType* Type{ nullptr };
    uint32_t Offset{ 0 };
};

template<typename T>
struct TPropertyAccessor
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


class REFL_API RBoolProperty : public RProperty
{
    using IPropertyAccessor = TPropertyAccessor<bool>;
public:
    RBoolProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}

    virtual void SetBool(void* BasePtr, bool Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), (bool)Value);
    }
    virtual bool GetBool(void const* BasePtr) const override
    {
        return IPropertyAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual std::string GetBoolToString(void const* BasePtr) const
    {
        return GetBool(BasePtr) ? "True" : "False";
    }
};


template <typename T>
class TNumericProperty : public RProperty
{
    using IPropertyAccessor = TPropertyAccessor<T>;
public:
    TNumericProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}

    virtual void SetUInt(void* BasePtr, uint64_t Value) const override
    {
        assert(std::is_integral_v<T>);
        IPropertyAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetSInt(void* BasePtr, int64_t Value) const override
    {
        assert(std::is_integral_v<T>);
        IPropertyAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetFloat(void* BasePtr, double Value) const override
    {
        assert(std::is_floating_point_v<T>);
        IPropertyAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetNumericFromString(void* BasePtr, char const* Value) const override
    {
        *IPropertyAccessor::GetPtr(GetRowPtr(BasePtr)) = (T)atof(Value);
    }
    virtual std::string GetNumericToString(void const* BasePtr) const override
    {
        return std::to_string(IPropertyAccessor::Get(GetRowPtr(BasePtr)));
    }
    virtual int64_t GetSInt(void const* BasePtr) const override
    {
        assert(std::is_integral_v<T>);
        return (int64_t)IPropertyAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual uint64_t GetUInt(void const* BasePtr) const override
    {
        assert(std::is_integral_v<T>);
        return (uint64_t)IPropertyAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual double GetFloat(void const* BasePtr) const override
    {
        assert(std::is_floating_point_v<T>);
        return (double)IPropertyAccessor::Get(GetRowPtr(BasePtr));
    }
};

template class REFL_API TNumericProperty<int8_t>;
template class REFL_API TNumericProperty<int16_t>;
template class REFL_API TNumericProperty<int32_t>;
template class REFL_API TNumericProperty<int64_t>;
template class REFL_API TNumericProperty<int8_t>;
template class REFL_API TNumericProperty<int16_t>;
template class REFL_API TNumericProperty<int32_t>;
template class REFL_API TNumericProperty<int64_t>;
template class REFL_API TNumericProperty<float>;
template class REFL_API TNumericProperty<double>;

using RSInt8Property = TNumericProperty<int8_t>;
using RSInt16Property = TNumericProperty<int16_t>;
using RSInt32Property = TNumericProperty<int32_t>;
using RSInt64Property = TNumericProperty<int64_t>;
using RUInt8Property = TNumericProperty<int8_t>;
using RUInt16Property = TNumericProperty<int16_t>;
using RUInt32Property = TNumericProperty<int32_t>;
using RUInt64Property = TNumericProperty<int64_t>;
using RFloatProperty = TNumericProperty<float>;
using RDoubleProperty = TNumericProperty<double>;

class REFL_API RStringProperty : public RProperty
{
    using IPropertyAccessor = TPropertyAccessor<std::string>;
public:

    RStringProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}

    virtual std::string GetString(void const* BasePtr) const override
    {
        return IPropertyAccessor::Get(GetRowPtr(BasePtr));
    }

    virtual const char* GetCString(void const* BasePtr) const override
    {
        return IPropertyAccessor::Get(GetRowPtr(BasePtr)).c_str();
    }

    virtual void SetString(void* BasePtr, const std::string& Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), Value);
    }

    virtual void SetString(void* BasePtr, const char* Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), Value);
    }

    virtual void SetString(void* BasePtr, uint64_t Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), std::to_string(Value));
    }

    virtual void SetString(void* BasePtr, int64_t Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), std::to_string(Value));
    }

    virtual void SetString(void* BasePtr, double Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), std::to_string(Value));
    }
};

class REFL_API REnumProperty : public RProperty
{
public:
    REnumProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}

};

template<typename T>
class TEnumProperty : public REnumProperty
{
    using IPropertyAccessor = TPropertyAccessor<T>;
public:
    TEnumProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : REnumProperty(InName, InOffset)
    {}

    virtual void SetUInt(void* BasePtr, uint64_t Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual void SetSInt(void* BasePtr, int64_t Value) const override
    {
        IPropertyAccessor::Set(GetRowPtr(BasePtr), (T)Value);
    }
    virtual int64_t GetSInt(void const* BasePtr) const override
    {
        return (int64_t)IPropertyAccessor::Get(GetRowPtr(BasePtr));
    }
    virtual uint64_t GetUInt(void const* BasePtr) const override
    {
        return (uint64_t)IPropertyAccessor::Get(GetRowPtr(BasePtr));
    }
};


class REFL_API RClassProperty : public RProperty
{
public:
    RClassProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}
    
};

class REFL_API RVectorProperty : public RProperty
{
public:
    RVectorProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}
    ~RVectorProperty() 
    {
        if (ElementProp)
        {
            delete ElementProp;
            ElementProp = nullptr;
        }
    }

    RProperty* GetElementProp()
    {
        return ElementProp;
    }

protected:
    RProperty* ElementProp;
};

template<typename T>
class TVectorProperty : public RVectorProperty
{
public:
    TVectorProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RVectorProperty(InName, InOffset)
    {
        ElementProp = NewProperty<IsStdVector<T>::ElementType>("", 0);
    }
};

class REFL_API RSetProperty : public RProperty
{
public:
    RSetProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}

    RProperty* GetElementProp()
    {
        return ElementProp;
    }

protected:
    RProperty* ElementProp;
};

template<typename T>
class TSetProperty : public RSetProperty
{
public:
    TSetProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RSetProperty(InName, InOffset)
    {
        ElementProp = NewProperty<IsStdSet<T>::ElementType>("", 0);
    }
};

class REFL_API RMapProperty : public RProperty
{
public:
    RMapProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RProperty(InName, InOffset)
    {}

    RProperty* GetKeyProp()
    {
        return KeyProp;
    }

    RProperty* GetValueProp()
    {
        return ValueProp;
    }

protected:
    RProperty* KeyProp;
    RProperty* ValueProp;
};

template<typename T>
class TMapProperty : public RMapProperty
{
public:
    TMapProperty(const std::string& InName = "", uint32_t InOffset = 0)
        : RMapProperty(InName, InOffset)
    {
        KeyProp = NewProperty<IsStdMap<T>::KeyType>("", 0);
        ValueProp = NewProperty<IsStdMap<T>::ValueType>("", 0);
    }
};

template<typename T>
constexpr bool NewPropertyNotSupported = false;

template<typename T>
RProperty* NewProperty(const std::string& InName, uint32_t InOffset)
{
    static_assert(!std::is_reference_v<T>);
    RProperty* Prop = nullptr;
         if constexpr (std::is_same_v<bool, T>        ) Prop = new RBoolProperty(InName, InOffset);
    else if constexpr (std::is_arithmetic_v<T>        ) Prop = new TNumericProperty<T>(InName, InOffset);
    else if constexpr (std::is_same_v<T, std::string> ) Prop = new RStringProperty(InName, InOffset);
    else if constexpr (HasStaticClass<T>::value       ) Prop = new RClassProperty(InName, InOffset);
    else if constexpr (std::is_enum_v<T>              ) Prop = new TEnumProperty<T>(InName, InOffset);
    else if constexpr (IsStdVector<T>::value          ) Prop = new TVectorProperty<T>(InName, InOffset);
    else if constexpr (IsStdSet<T>::value             ) Prop = new TSetProperty<T>(InName, InOffset);
    else if constexpr (IsStdMap<T>::value             ) Prop = new TMapProperty<T>(InName, InOffset);
    else                                                static_assert(NewPropertyNotSupported<T> && "UNSUPPORTED TYPE");
    Prop->SetType(GetReflType<T>());
    return Prop;
}
