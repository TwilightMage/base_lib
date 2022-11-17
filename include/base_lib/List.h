#pragma once

#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

#include "Array.h"
#include "BasicTypes.h"
#include "IConvertible.h"
#include "StreamUtils.h"

template<typename ValueType>
class List : public Array<ValueType>
{
public:
    using Array<ValueType>::begin;
    using Array<ValueType>::end;

    using Array<ValueType>::get_data;
    using Array<ValueType>::operator==;
    using Array<ValueType>::operator!=;

    List()
    {
    }

    List(const List& rhs)
        : List(rhs.inner_, rhs.length())
    {
    }

    List(ValueType* inner, uint length)
    {
        allocated_length_ = get_allocate_size(length);
        length_ = length;
        if (length > 0)
        {
            inner_ = new ValueType[allocated_length_];
            if constexpr (Data<ValueType>)
            {
                memcpy(inner_, inner, sizeof(ValueType) * length_);
            }
            else
            {
                for (uint i = 0; i < length; i++)
                {
                    inner_[i] = inner[i];
                }
            }
        }
    }

    List(const ValueType* inner, uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            inner_ = new ValueType[allocated_length_];
            if constexpr (std::is_convertible<ValueType, std::size_t>::value)
            {
                memcpy(inner_, inner, sizeof(ValueType) * length_);
            }
            else
            {
                for (uint i = 0; i < length; i++)
                {
                    inner_[i] = inner[i];
                }
            }
        }
    }

    List(const std::vector<ValueType>& vec)
        : List(vec.data(), static_cast<uint>(vec.size()))
    {
    }

    List(const std::initializer_list<ValueType>& il)
        : List(static_cast<const ValueType*>(il.begin()), static_cast<uint>(il.size()))
    {
    }

    List(uint length)
    {
        allocated_length_ = get_allocate_size(length);
        length_ = length;
        if (length > 0)
        {
            inner_ = new ValueType[allocated_length_];
            if constexpr (Data<ValueType>)
            {
                ZeroMemory(inner_, sizeof(ValueType) * length_);
            }
            else
            {
                for (uint i = 0; i < length; i++)
                {
                    inner_[i] = std::move(ValueType());
                }
            }
        }
    }

    List(uint length, std::function<ValueType(uint index)> generator)
        : List(length)
    {
        for (uint i = 0; i < length; i++)
        {
            inner_[i] = generator(i);
        }
    }

    List(uint length, const ValueType& placeholder)
        : List(length)
    {
        for (uint i = 0; i < length; i++)
        {
            inner_[i] = placeholder;
        }
    }

    template<typename ...Items>
    static List of(Items... items)
    {
        return {{items...}};
    }

    List operator=(const List& rhs)
    {
        if (this == &rhs) return *this;

        delete[] inner_;

        length_ = rhs.length_;
        allocated_length_ = rhs.allocated_length_;

        if (length_ > 0)
        {
            inner_ = new ValueType[allocated_length_];
            if constexpr (std::is_convertible<ValueType, std::size_t>::value)
            {
                memcpy(inner_, rhs.inner_, sizeof(ValueType) * length_);
            }
            else
            {
                for (uint i = 0; i < length_; i++)
                {
                    inner_[i] = std::move(rhs.inner_[i]);
                }
            }
        }
        else
        {
            inner_ = nullptr;
        }

        return *this;
    }

    void add(const ValueType& item)
    {
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        inner_[length_++] = std::move(item);
    }

    template<typename... Items>
    void add_many(const ValueType& first, const Items&... rest)
    {
        add(first);

        if constexpr (sizeof...(rest) > 0)
        {
            add_many(rest...);
        }
    }

    void add_many(const List<ValueType>& items)
    {
        if (length_ + items.length() > allocated_length_)
        {
            reallocate(get_allocate_size(length_ + items.length()));
        }

        if constexpr (std::is_convertible<ValueType, std::size_t>::value)
        {
            if (items.length() > 0)
            {
                memcpy(inner_ + length_, items.inner_, sizeof(ValueType) * items.length_);
                length_ += items.length_;
            }
        }
        else
        {
            for (const auto& item : items)
            {
                inner_[length_++] = std::move(item);
            }
        }
    }

    void add_unique(const ValueType& item)
    {
        if (length_ > 0)
        {
            for (uint i = 0; i < length_; i++)
            {
                if (inner_[i] == item) return;;
            }
        }

        add(item);
    }

    bool contains(const ValueType& item) const
    {
        if (length_ == 0) return false;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return true;
        }
        return false;
    }

    int index_of(const ValueType& item) const
    {
        if (length_ == 0) return -1;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return i;
        }
        return -1;
    }

    int index_of(std::function<bool(const ValueType& item)> predicate) const
    {
        if (length_ == 0) return -1;
        
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return i;
        }
        return -1;
    }

    void remove_at(uint index)
    {
        if (index >= length_)
        {
            throw std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        if (index < length_ - 1)
        {
            for (uint i = index; i < length_ - 1; i++)
            {
                inner_[i] = std::move(inner_[i + 1]);
            }
        }

        inner_[length_ - 1] = ValueType();
        
        --length_;
    }

    void remove_range(uint start, uint size)
    {
        if (start + size >= length_)
        {
            throw std::out_of_range("last remove index is greater than last item index");
        }

        const uint new_allocated_length = get_allocate_size(length_ - size);
        if (new_allocated_length != allocated_length_)
        {
            allocated_length_ = new_allocated_length;
            length_ -= size;
            ValueType* new_inner;

            if (allocated_length_ > 0)
            {
                new_inner = new ValueType[allocated_length_];

                for (uint i = 0; i < start; i++)
                {
                    new_inner[i] = std::move(inner_[i]);
                }

                for (uint i = start; i < length_; i++)
                {
                    new_inner[i] = std::move(inner_[i + size]);
                }
            }
            else
            {
                new_inner = nullptr;
            }

            delete inner_;
            inner_ = new_inner;
        }
        else
        {
            for (uint i = start + size; i < length_ - size; i++)
            {
                inner_[i - size] = std::move(inner_[i]);
                inner_[i] = ValueType();
            }
        }
    }

    void replace(const ValueType& from, const ValueType& to)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == from)
            {
                inner_[i] = to;
            }
        }
    }

    void resize(uint new_size, const ValueType& placeholder = ValueType())
    {
        if (length_ < new_size)
        {
            if (new_size >= allocated_length_)
            {
                reallocate(get_allocate_size(new_size));
            }

            for (; length_ < new_size; ++length_)
            {
                inner_[length_] = placeholder;
            }
        }
        else if (length_ > new_size)
        {
            const auto new_allocated_length = get_allocate_size(new_size);
            if (new_allocated_length < allocated_length_)
            {
                reallocate(new_allocated_length);
            }

            for (length_ = allocated_length_; length_ > new_size; --length_)
            {
                inner_[length_] = ValueType();
            }
        }
    }

    void remove(const ValueType& item)
    {
        uint offset = 0;
        for (uint i = 0; i < length_; i++)
        {
            if (i + offset < length_)
            {
                while (inner_[i + offset] == item && i + offset < length_) ++offset;
                if (offset > 0)
                {
                    if (i + offset < length_)
                    {
                        inner_[i] = std::move(inner_[i + offset]);
                    }
                    else
                    {
                        inner_[i] = ValueType();
                    }
                }
            }
            else
            {
                inner_[i] = ValueType();
            }
        }

        length_ -= offset;
    }

    void remove(bool(* predicate)(const ValueType&))
    {
        uint offset = 0;
        for (uint i = 0; i < length_; i++)
        {
            if (i + offset < length_)
            {
                while (predicate(inner_[i + offset]) && i + offset < length_) ++offset;
                if (offset > 0)
                {
                    if (i + offset < length_)
                    {
                        inner_[i] = std::move(inner_[i + offset]);
                    }
                    else
                    {
                        inner_[i] = ValueType();
                    }
                }
            }
            else
            {
                inner_[i] = ValueType();
            }
        }

        length_ -= offset;
    }

    List without(const ValueType& item) const
    {
        List result = *this;
        result.remove(item);
        return result;
    }

    List without(bool(* predicate)(const ValueType&)) const
    {
        List result = *this;
        result.remove(predicate);
        return result;
    }
    
    ValueType& operator[](uint index)
    {
        return at(index);
    }

    const ValueType& operator[](uint index) const
    {
        return at(index);
    }

    ValueType& at(uint index)
    {
        if (index >= length_)
        {
            throw std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    const ValueType& at(uint index) const
    {
        if (index >= length_)
        {
            throw std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    ValueType& first()
    {
        return operator[](0);
    }

    const ValueType& first() const
    {
        return operator[](0);
    }

    ValueType first_or_default()
    {
        return length_ > 0 ? operator[](0) : ValueType();
    }

    ValueType& first_or_default(ValueType& defaultValue)
    {
        return length_ > 0 ? operator[](0) : defaultValue;
    }

    const ValueType& first_or_default(const ValueType& defaultValue) const
    {
        return length_ > 0 ? operator[](0) : defaultValue;
    }
    
    ValueType& last()
    {
        return operator[](length_ - 1);
    }

    const ValueType& last() const
    {
        return operator[](length_ - 1);
    }

    ValueType last_or_default()
    {
        return length_ > 0 ? operator[](length_ - 1) : ValueType();
    }

    ValueType& last_or_default(ValueType& defaultValue)
    {
        return length_ > 0 ? operator[](length_ - 1) : defaultValue;
    }

    const ValueType& last_or_default(const ValueType& defaultValue) const
    {
        return length_ > 0 ? operator[](length_ - 1) : defaultValue;
    }

    bool all(std::function<bool(const ValueType& item)> predicate) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (!predicate(inner_[i])) return false;
        }

        return  true;
    }

    bool any(std::function<bool(const ValueType& item)> predicate) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return true;
        }

        return  false;
    }
    
    bool none(std::function<bool(const ValueType& item)> predicate) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return false;
        }

        return  true;
    }

    bool count(std::function<bool(const ValueType& item)> predicate) const
    {
        uint counter = 0;
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i]))
            {
                counter++;
            }
        }

        return  counter;
    }

    List<ValueType> where(std::function<bool(const ValueType& item)> predicate) const
    {
        List<ValueType> result;
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i]))
            {
                result.add(inner_[i]);
            }
        }
        
        return result;
    }

    template<typename ResultType>
    List<ResultType> select(std::function<ResultType(const ValueType& item)> processor) const
    {
        List<ResultType> result(length_);

        for (uint i = 0; i < length_; i++)
        {
            result[i] = processor(operator[](i));
        }

        return result;
    }

    List<ValueType>& for_each(std::function<void(ValueType& item, uint i)> function)
    {
        for (uint i = 0; i < length_; i++)
        {
            function(inner_[i], i);
        }

        return *this;
    }

    const List<ValueType>& for_each(std::function<void(const ValueType& item, uint i)> function) const
    {
        for (uint i = 0; i < length_; i++)
        {
            function(inner_[i], i);
        }

        return *this;
    }

    template<typename NewValueType>
    List<NewValueType> convert() const requires Convertible<ValueType, NewValueType>
    {
        List<NewValueType> result(length_);

        for (uint i = 0; i < length_; i++)
        {
            ::convert(inner_[i], result[i]);
        }

        return result;
    }

    void insert(const ValueType& item, uint indexAt)
    {
        if (indexAt > length_)
        {
            throw std::out_of_range("Parameter \"indexAt\" is greater than last available index");
        }

        ValueType item_copy = item;
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        ++length_;
        
        if (indexAt < length_)
        {
            for (uint i = 0; i < length_ - indexAt - 1; i++)
            {
                inner_[length_ - i - 1] = std::move(inner_[length_ - i - 2]);
            }
        }

        inner_[indexAt] = std::move(item_copy);
    }

    void clear()
    {
        reallocate(0);
        length_ = 0;
    }

    void sort() requires CanCheckLess<ValueType>
    {
        if constexpr (HaveLessOperator<ValueType>)
        {
            std::sort(begin(), end());
        }
        else if constexpr (Data<ValueType>)
        {
            const static bool(*op)(const ValueType&, const ValueType&) = [](const ValueType& lhs, const ValueType& rhs) -> bool const
            {
                return memcmp(&lhs, &rhs, sizeof(ValueType)) < 0;
            };

            std::sort(begin(), end(), op);;
        }
    }

    void sort_reverse() requires CanCheckLess<ValueType>
    {
        if constexpr (HaveLessOperator<ValueType>)
        {
            std::sort(begin(), end(), std::greater<ValueType>());
        }
        else if constexpr (Data<ValueType>)
        {
            const static bool(*op)(const ValueType&, const ValueType&) = [](const ValueType& lhs, const ValueType& rhs) -> bool
            {
                return memcmp(&lhs, &rhs, sizeof(ValueType)) > 0;
            };

            std::sort(begin(), end(), op);;
        }
    }

    template<class Predicate>
    void sort_predicate()
    {
        std::sort(begin(), end(), Predicate());
    }

    void sort_predicate(const std::function<bool(ValueType a, ValueType b)> predicate)
    {
        std::sort(begin(), end(), predicate);
    }

    List operator+(const ValueType& rhs)
    {
        List result = *this;
        result.add(rhs);

        return result;
    }

    List& operator+=(const ValueType& rhs)
    {
        add(rhs);

        return *this;
    }

    List operator+(const List& rhs)
    {
        List result = *this;
        result.add_many(rhs);

        return result;
    }

    List& operator+=(const List& rhs)
    {
        add_many(rhs);

        return *this;
    }

    List operator*(const List& rhs) const
    {
        List result;
        for (auto& entry : *this)
        {
            if (rhs.contains(entry))
            {
                result.add(entry);
            }
        }

        return result;
    }

    List& operator*=(const List& rhs)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (!rhs.contains(inner_[i]))
            {
                remove_at(i--);
            }
        }

        return *this;
    }

    List operator-(const List& rhs) const
    {
        List result;
        for (auto& entry : *this)
        {
            if (!rhs.contains(entry))
            {
                result.add(entry);
            }
        }

        return result;
    }

    List& operator-=(const List& rhs)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (rhs.contains(inner_[i]))
            {
                remove_at(i--);
            }
        }

        return *this;
    }

    void write_to_stream(std::ostream& stream) const requires Serializable<ValueType>
    {
        StreamUtils::write<uint>(stream, length_);
        for (uint i = 0; i < length_; i++)
        {
            StreamUtils::write(stream, inner_[i]);
        }
    }

    void read_from_stream(std::istream& stream) requires Serializable<ValueType>
    {
        clear();
        uint new_length = StreamUtils::read<uint>(stream);
        reallocate(get_allocate_size(new_length));
        length_ = new_length;
        for (uint i = 0; i < length_; i++)
        {
            inner_[i] = StreamUtils::read<ValueType>(stream);
        }
    }

private:
    using Array<ValueType>::get_allocate_size;
    using Array<ValueType>::reallocate;
    using Array<ValueType>::slack;

    using Array<ValueType>::inner_;
    using Array<ValueType>::length_;
    using Array<ValueType>::allocated_length_;
};