#pragma once

#include <assert.h>

#include "String.h"

template<typename T>
class Array2D
{
public:
    Array2D()
        : data_(nullptr)
        , size_x_(0)
        , size_y_(0)
    {
        
    }
    
    Array2D(uint size_x, uint size_y)
        : size_x_(size_x)
        , size_y_(size_y)
    {
        if (get_size_total() > 0)
        {
            data_ = new T[get_size_total()];
            for (uint i = 0; i < get_size_total(); i++)
            {
                data_[i] = T();
            }
        }
    }

    Array2D(uint size_x, uint size_y, const T& fill)
        : size_x_(size_x)
        , size_y_(size_y)
    {
        if (get_size_total() > 0)
        {
            data_ = new T[get_size_total()];
            for (uint i = 0; i < get_size_total(); i++)
            {
                data_[i] = fill;
            }
        }
    }

    Array2D(uint size_x, uint size_y, const List<T>& data)
        : size_x_(size_x)
        , size_y_(size_y)
    {
        if (data.length() == size_x * size_y)
        {
            data_ = new T[get_size_total()];
            for (uint i = 0; i < get_size_total(); i++)
            {
                data_[i] = data[i];
            }
        }
    }

    Array2D(const Array2D& rhs)
        : size_x_(rhs.size_x_)
        , size_y_(rhs.size_y_)
    {
        data_ = new T[get_size_total()];
        for (uint i = 0; i < get_size_total(); i++)
        {
            data_[i] = std::move(rhs.data_[i]);
        }
    }

    Array2D& operator=(const Array2D& rhs)
    {
        if (this == &rhs) return *this;
        
        if (get_size_total() != rhs.get_size_total())
        {
            size_x_ = rhs.size_x_;
            size_y_ = rhs.size_y_;

            delete[] data_;
            data_ = new T[get_size_total()];
        }

        for (uint i = 0; i < get_size_total(); i++)
        {
            data_[i] = std::move(rhs.data_[i]);
        }

        return *this;
    }

    T* begin()
    {
        return data_;
    }

    T* end()
    {
        return data_ + size_x_ * size_y_;
    }

    const T* begin() const
    {
        return data_;
    }

    const T* end() const
    {
        return data_ + size_x_ * size_y_;
    }

    ~Array2D()
    {
        delete[] data_;
    }

    T& at(uint x, uint y)
    {
        assert(x < size_x_);
        assert(y < size_y_);
        
        return data_[y * size_x_ + x];
    }

    const T& at(uint x, uint y) const
    {
        assert(x < size_x_);
        assert(y < size_y_);
        
        return data_[y * size_x_ + x];
    }

    uint get_size_x() const
    {
        return size_x_;
    }

    uint get_size_y() const
    {
        return size_y_;
    }

    uint get_size_total() const
    {
        return size_x_ * size_y_;
    }

    List<T> to_list() const
    {
        return get_size_total() > 0 ? List<T>(data_, size_x_ * size_y_) : List<T>();
    }

private:
    T* data_ = nullptr;
    uint size_x_;
    uint size_y_;
};
