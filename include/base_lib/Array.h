#pragma once

#include "IData.h"

template<typename ValueType>
class Array
{
public:   
    ~Array()
    {
        delete[] inner_;
    }

    const ValueType* begin() const
    {
        return inner_;
    }

    const ValueType* end() const
    {
        return inner_ + length_;
    }

    ValueType* begin()
    {
        return inner_;
    }

    ValueType* end()
    {
        return inner_ + length_;
    }

    [[nodiscard]] uint length() const
    {
        return length_;
    }

    ValueType* get_data()
    {
        return inner_;
    }

    const ValueType* get_data() const
    {
        return inner_;
    }

    bool operator==(const Array& rhs) const
    {
        if (length_ != rhs.length_) return false;

        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] != rhs.inner_[i]) return false;
        }

        return true;
    }

    bool operator!=(const Array& rhs) const
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
