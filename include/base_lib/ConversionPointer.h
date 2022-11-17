#pragma once

#include "framework.h"
#include "IConvertible.h"

template<typename TargetType>
class ConversionPointer
{
public:
    TargetType& get() const
    {
        return *ptr_;
    }
    
    template<typename InputType>
    void set(InputType val) const requires Convertible<InputType, TargetType>
    {
        convert<InputType, TargetType>(val, *ptr_);
    }

    operator bool() const
    {
        return ptr_ != nullptr;
    }

    bool operator==(const ConversionPointer& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }

    bool operator!=(const ConversionPointer& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }

    bool operator==(nullptr_t rhs) const
    {
        return ptr_ == rhs;
    }

    bool operator!=(nullptr_t rhs) const
    {
        return ptr_ != rhs;
    }

private:
    TargetType* ptr_ = nullptr;
};
