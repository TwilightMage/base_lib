#pragma once

#include <set>
#include <stdexcept>

#include "Array.h"

template<typename T>
class Set : public Array<T>
{
    using Array<T>::get_allocate_size;
    using Array<T>::reallocate;
    using Array<T>::slack;
    
public:
    using Array<T>::begin;
    using Array<T>::end;

    using Array<T>::get_data;
    using Array<T>::operator==;
    using Array<T>::operator!=;

    Set()
    {
        inner_ = nullptr;
        length_ = 0;
        allocated_length_ = 0;
    }

    Set(const Set& rhs)
    {
        reallocate(rhs.length_);
        length_ = rhs.length();

        for (uint i = 0; i < length_; i++)
        {
            inner_[i] = std::move(rhs.inner_[i]);
        }
    }

    Set(T* data_ptr, uint length)
    {
        reallocate(length);

        for (uint i = 0; i < length; i++)
        {
            add(data_ptr[i]);
        }

        slack();
    }

    Set(const std::initializer_list<T>& il)
    {
        reallocate((uint)il.size());

        for (const auto& item : il)
        {
            add(item);
        }

        slack();
    }

    explicit Set(const List<T>& list)
    {
        reallocate((uint)list.length());

        for (const auto& item : list)
        {
            add(item);
        }

        slack();
    }

    Set& operator=(const Set& rhs)
    {
        if (this == &rhs) return *this;

        reallocate(rhs.length_);
        length_ = rhs.length_;

        for (uint i = 0; i < length_; i++)
        {
            inner_[i] = std::move(rhs.inner_[i]);
        }

        return *this;
    }

    void add(const T& item)
    {
        uint l = 0;
        uint r = length_;
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (inner_[m] < item)
            {
                l = m + 1;
            }
            else if (inner_[m] > item)
            {
                r = m;
            }
            else
            {
                return;
            }
        }

        m = (l + r) / 2;

        T item_copy = item;
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        ++length_;
        
        if (m < length_)
        {
            for (uint i = 0; i < length_ - m - 1; i++)
            {
                inner_[length_ - i - 1] = std::move(inner_[length_ - i - 2]);
            }
        }

        inner_[m] = std::move(item_copy);
    }

    void remove(const T& item)
    {
        uint l = 0;
        uint r = length_;
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (inner_[m] < item)
            {
                l = m + 1;
            }
            else if (inner_[m] > item)
            {
                r = m;
            }
            else
            {
                if (m < length_ - 1)
                {
                    for (uint i = m; i < length_ - 1; i++)
                    {
                        inner_[i] = std::move(inner_[i + 1]);
                    }
                }

                inner_[length_ - 1] = T();
        
                --length_;
                
                return;
            }
        }
    }

    void clear()
    {
        reallocate(0);
        length_ = 0;
    }

    bool contains(const T& rhs) const
    {
        uint l = 0;
        uint r = length_;
        uint m = 0;

        while (l < r)
        {
            m = (l + r) / 2;
            if (inner_[m] < rhs)
            {
                l = m + 1;
            }
            else if (inner_[m] > rhs)
            {
                r = m;
            }
            else
            {
                return true;
            }
        }

        return false;
    }

    List<T> to_list() const
    {
        return List<T>(inner_, length_);
    }
    
    const T& operator[](uint index) const
    {
        if (index >= length_)
        {
            throw std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    template<typename... ArgTypes>
    static Set of(ArgTypes... args)
    {
        const std::initializer_list<T> args_list = {args...};
        Set result;
        for (auto& arg : args_list)
        {
            result.add(arg);
        }
        return result;
    }

    Set operator+(const Set& rhs) const
    {
        if (this == &rhs) return *this;
        
        Set result = *this;
        for (auto& item : rhs)
        {
            result.add(item);
        }

        return result;
    }

    Set& operator+=(const Set& rhs)
    {
        if (this == &rhs) return *this;
            
        for (const auto& item : rhs)
        {
            add(item);
        }

        return *this;
    }

    Set operator-(const Set& rhs) const
    {
        if (this == &rhs) return Set();
        
        Set result = *this;
        for (auto& item : rhs)
        {
            result.remove(item);
        }

        return result;
    }

    Set& operator-=(const Set& rhs)
    {
        if (this == &rhs) return Set();
            
        for (const auto& item : rhs)
        {
            add(item);
        }

        return *this;
    }

private:
    using Array<T>::inner_;
    using Array<T>::length_;
    using Array<T>::allocated_length_;
};
