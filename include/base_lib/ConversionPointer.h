#pragma once

#include "framework.h"
#include "IConvertible.h"

template<typename TargetType>
class ConversionPointer
{
public:
    FORCEINLINE TargetType& get() const
    {
        return *ptr_;
    }
    
    template<typename InputType>
    FORCEINLINE void set(InputType val) const requires Convertible<InputType, TargetType>
    {
        convert<InputType, TargetType>(val, *ptr_);
    }

    FORCEINLINE operator bool() const
    {
        return ptr_ != nullptr;
    }

    FORCEINLINE bool operator==(const ConversionPointer& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }

    FORCEINLINE bool operator!=(const ConversionPointer& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }

    FORCEINLINE bool operator==(nullptr_t rhs) const
    {
        return ptr_ == rhs;
    }

    FORCEINLINE bool operator!=(nullptr_t rhs) const
    {
        return ptr_ != rhs;
    }

private:
    TargetType* ptr_ = nullptr;
};
