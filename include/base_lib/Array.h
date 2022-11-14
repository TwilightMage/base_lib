#pragma once

#include "framework.h"
#include "IData.h"

template<typename ValueType>
class Array
{
public:   
    ~Array()
    {
        delete[] inner_;
    }

    FORCEINLINE const ValueType* begin() const
    {
        return inner_;
    }

    FORCEINLINE const ValueType* end() const
    {
        return inner_ + length_;
    }

    FORCEINLINE ValueType* begin()
    {
        return inner_;
    }

    FORCEINLINE ValueType* end()
    {
        return inner_ + length_;
    }

    FORCEINLINE [[nodiscard]] uint length() const
    {
        return length_;
    }

    FORCEINLINE ValueType* get_data()
    {
        return inner_;
    }

    FORCEINLINE const ValueType* get_data() const
    {
        return inner_;
    }

    FORCEINLINE bool operator==(const Array& rhs) const
    {
        if (length_ != rhs.length_) return false;

        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] != rhs.inner_[i]) return false;
        }

        return true;
    }

    FORCEINLINE bool operator!=(const Array& rhs) const
    {
        return !operator==(rhs);
    }

protected:
    static uint get_allocate_size(uint len)
    {
        if (len == 0) return 0;

        uint result = 1;
        while (result < len)
        {
            result *= 2;
        }

        return result;
    }

    void reallocate(uint newLength)
    {
        allocated_length_ = newLength;
        if (newLength > 0)
        {
            ValueType* new_inner = new ValueType[newLength];

            if constexpr (Data<ValueType>)
            {
                memcpy(new_inner, inner_, sizeof(ValueType) * length_);
            }
            else
            {
                for (uint i = 0; i < (length_ < newLength ? length_ : newLength); i++)
                {
                    new_inner[i] = inner_[i];
                }
            }

            delete[] inner_;
            inner_ = new_inner;
        }
        else
        {
            delete[] inner_;
            inner_ = nullptr;
        }
    }

    void slack()
    {
        const uint desired_size = get_allocate_size(length_);

        if (desired_size != allocated_length_)
        {
            reallocate(desired_size);
        }
    }

    ValueType* inner_ = nullptr;
    uint length_ = 0;
    uint allocated_length_ = 0;
};
