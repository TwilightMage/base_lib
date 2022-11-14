#pragma once
#include "framework.h"

template<typename SourceType>
class ConversionPointer
{
public:
    FORCEINLINE SourceType& get() const
    {
        return *ptr_;
    }
    
    template<typename InputType>
    FORCEINLINE void set(InputType val) const
    {
        *ptr_ = SourceType(val);
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
    SourceType* ptr_ = nullptr;
};
