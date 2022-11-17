#pragma once

//#define USE_OWN_SHARED_POINTERS

#include "Assert.h"
#include "BasicTypes.h"
#include "framework.h"

#ifdef USE_OWN_SHARED_POINTERS

#pragma warning(disable: 4150)

#include <concepts>

template<typename T>
class Weak;

template<typename T>
class Shared;

struct MemBlock
{
    uint64 shared_references;
    uint64 weak_references;
};


template<typename T>
class EXPORT EnableSharedFromThis
{
    friend Shared<T>;

public:
    Shared<T> shared_from_this() const;

    Weak<T> weak_from_this() const;

    MemBlock* last_valid_mem_block = nullptr;
};

template<typename T>
concept CanEnableShared = requires
{
    { T::last_valid_mem_block } -> std::same_as<MemBlock*&>;
};

template<typename T>
class EXPORT Shared
{
    friend Weak<T>;
    friend EnableSharedFromThis<T>;
    
public:
    Shared()
        : ptr_(nullptr)
        , mem_block_(nullptr)
    {}

    Shared(T* ptr)
        : ptr_(ptr)
        , mem_block_(nullptr)
    {
        if (ptr)
        {
            mem_block_ = new MemBlock{1, 0};
            if constexpr (CanEnableShared<T>)
            {
                ptr->last_valid_mem_block = mem_block_;
            }
        }
    }

    Shared(T* ptr, MemBlock* mem_block)
        : ptr_(ptr)
        , mem_block_(mem_block)
    {
        if (ptr_) incref();
    }

    Shared(const Shared& rhs)
        : ptr_(rhs.ptr_)
        , mem_block_(rhs.mem_block_)
    {
        if (ptr_) incref();
    }

    ~Shared()
    {
        if (ptr_)
        {
            decref();
        }
    }

    Shared& operator=(const Shared& rhs)
    {
        if (&rhs == this) return *this;

        if (ptr_)
        {
            decref();
        }

        ptr_ = rhs.ptr_;
        mem_block_ = rhs.mem_block_;

        if (ptr_)
        {
            incref();
        }
        
        return *this;
    }

    template<typename To>
    requires std::is_convertible<T*, To*>::value
    operator Shared<To>() const
    {
        return Shared<To>(ptr_, mem_block_);
    }

    MemBlock* get_mem_block() const
    {
        return mem_block_;
    }

    Shared& operator=(nullptr_t)
    {
        if (ptr_)
        {
            decref();
        }

        ptr_ = nullptr;
        mem_block_ = nullptr;
        
        return *this;
    }

    void reset()
    {
        if (ptr_)
        {
            decref();
        }

        ptr_ = nullptr;
        mem_block_ = nullptr;
    }

    T* get() const { return ptr_; }

    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }

    T* operator->() { return ptr_; }
    T* operator->() const { return ptr_; }

    bool operator==(nullptr_t) const { return ptr_ == nullptr; }
    bool operator!=(nullptr_t) const { return ptr_ != nullptr; }
    operator bool() const { return ptr_; }

    template<typename T1>
    static Shared<T1> cast_to(const Shared& from)
    {
        if (T1* casted = dynamic_cast<T1*>(from.get()))
        {
            return Shared<T1>(casted, from.get_mem_block());
        }

        return nullptr;
    }
    
private:
    void decref() const
    {
        if (--(*mem_block_).shared_references == 0)
        {
            delete ptr_;
            delete mem_block_;
        }
    }

    void incref() const
    {
        ++(*mem_block_).shared_references;
    }

    T* ptr_;
    MemBlock* mem_block_;
};

template<class T, class... ArgTypes>
Shared<T> MakeShared(ArgTypes&&... Args)
{
    return Shared<T>(new T(std::forward<ArgTypes>(Args)...));
}

#define MakeSharedInternal(T, ...) (Shared<T>(new T(__VA_ARGS__)))

template<typename T>
class EXPORT Weak
{
    friend EnableSharedFromThis<T>;

public:
    Weak()
        : ptr_(nullptr)
        , mem_block_(nullptr)
    {}

    Weak(T* ptr, MemBlock* mem_block)
        : ptr_(ptr)
        , mem_block_(mem_block)
    {
        if (ptr_) incref();
    }

    Weak(const Shared<T>& rhs)
        : ptr_(rhs.ptr_)
        , mem_block_(rhs.mem_block_)
    {
        if (mem_block_) incref();
    }

    ~Weak()
    {
        if (ptr_)
        {
            decref();
        }
    }

    template<typename To>
    requires std::is_convertible<T*, To*>::value
    operator Weak<To>() const
    {
        return Weak<To>(ptr_, mem_block_);
    }

    Weak& operator=(const Weak& rhs)
    {
        if (&rhs == this) return *this;

        if (ptr_)
        {
            decref();
        }

        ptr_ = rhs.ptr_;
        mem_block_ = rhs.mem_block_;

        if (ptr_)
        {
            incref();
        }
        
        return *this;
    }

    Weak& operator=(nullptr_t)
    {
        if (ptr_)
        {
            decref();
        }
        
        ptr_ = nullptr;
        mem_block_ = nullptr;
        
        return *this;
    }

    Shared<T> lock() const
    {
        return ptr_ && mem_block_->shared_references > 0 ? Shared<T>(ptr_, mem_block_) : nullptr;
    }

    void reset()
    {
        if (ptr_)
        {
            decref();
        }
        
        ptr_ = nullptr;
        mem_block_ = nullptr;
    }

    T* get() const { return ptr_; }

    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }

    T& operator->() { return *ptr_; }
    const T& operator->() const { return *ptr_; }

    bool operator==(nullptr_t) const { return ptr_ == nullptr; }
    bool operator!=(nullptr_t) const { return ptr_ != nullptr; }
    operator bool() const { return ptr_; }

private:
    void decref() const
    {
        if (--(*mem_block_).weak_references == 0 && (*mem_block_).shared_references == 0)
        {
            delete mem_block_;
        }
    }
    
    void incref() const
    {
        ++(*mem_block_).weak_references;
    }
    
    T* ptr_;
    MemBlock* mem_block_;
};

template<typename T>
class EXPORT Reference
{
public:
    Reference()
        : ptr_(nullptr)
    {}

    Reference(T* ptr)
        : ptr_(ptr)
    {}

    ~Reference()
    {
        delete ptr_;
    }

    void reset(T* ptr)
    {
        delete ptr_;
        ptr_ = ptr;
    }

    void free()
    {
        ptr_ = nullptr;
    }

    T* get() const { return ptr_; }

    T& operator*() { return *ptr_; }
    const T& operator*() const { return *ptr_; }

    T* operator->() { return ptr_; }
    T* operator->() const { return ptr_; }

private:
    T* ptr_;
};

template <typename T>
Shared<T> EnableSharedFromThis<T>::shared_from_this() const
{
    AssertMsg(last_valid_mem_block != nullptr, "Memory", "Object was never constructed as shared");
    return Shared<T>((T*)this, last_valid_mem_block);
}

template <typename T>
Weak<T> EnableSharedFromThis<T>::weak_from_this() const
{
    AssertMsg(last_valid_mem_block != nullptr, "Memory", "Object was never constructed as shared");
    return Weak<T>((T*)this, last_valid_mem_block);
}

template<typename To, typename From>
To* cast(From* obj)
{
    return dynamic_cast<To*>(obj);
}

template<typename To, typename From>
Shared<To> cast(const Shared<From>& obj)
{
    return Shared<From>::template cast_to<To>(obj);
}

template<typename To, typename From>
Shared<const To> cast(const Shared<const From>& obj)
{
    return Shared<const From>::template cast_to<const To>(obj);
}

template<typename To, typename From>
Shared<To> cast(const Weak<From>& obj)
{
    return Shared<From>::template cast_to<To>(obj.lock());
}

template<typename T1, typename T2>
bool operator==(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() == rhs.get(); };

template<typename T1, typename T2>
bool operator<=(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() <= rhs.get(); };

template<typename T1, typename T2>
bool operator<(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() < rhs.get(); };

template<typename T1, typename T2>
bool operator!=(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() != rhs.get(); };

template<typename T1, typename T2>
bool operator>(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() > rhs.get(); };

template<typename T1, typename T2>
bool operator>=(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() >= rhs.get(); };


template<typename T1, typename T2>
bool operator==(const Weak<T1>& lhs, const Weak<T2>& rhs) { return lhs.get() == rhs.get(); };

template<typename T1, typename T2>
bool operator<=(const Weak<T1>& lhs, const Weak<T2>& rhs) { return lhs.get() <= rhs.get(); };

template<typename T1, typename T2>
bool operator<(const Weak<T1>& lhs, const Weak<T2>& rhs) { return lhs.get() < rhs.get(); };

template<typename T1, typename T2>
bool operator!=(const Weak<T1>& lhs, const Weak<T2>& rhs) { return lhs.get() != rhs.get(); };

template<typename T1, typename T2>
bool operator>(const Weak<T1>& lhs, const Weak<T2>& rhs) { return lhs.get() > rhs.get(); };

template<typename T1, typename T2>
bool operator>=(const Weak<T1>& lhs, const Weak<T2>& rhs) { return lhs.get() >= rhs.get(); };

#define null_weak(type) (nullptr)

#else

#include <memory>

template<typename T>
using Reference = std::unique_ptr<T>;

template<typename T>
using Weak = std::weak_ptr<T>;

#define null_weak(type) (Weak<type>())

template<typename T>
using Shared = std::shared_ptr<T>;

template<class T, class... Types>
Shared<T> MakeShared(Types&&... Args)
{
    return std::make_shared<T>(std::forward<Types>(Args)...);
}

#define MakeSharedInternal(T, ...) (Shared<T>(new T(__VA_ARGS__)))

template<typename T>
using EnableSharedFromThis = std::enable_shared_from_this<T>;

template<typename To, typename From>
To* cast(From* obj)
{
    return dynamic_cast<To*>(obj);
}

template<typename To, typename From>
To const* cast(From const* obj)
{
    return dynamic_cast<To const*>(obj);
}

template<typename To, typename From>
Shared<To> cast(const Shared<From>& obj)
{
    return std::dynamic_pointer_cast<To>(obj);
}

template<typename To, typename From>
Shared<const To> cast(const Shared<const From>& obj)
{
    return std::dynamic_pointer_cast<const To>(obj);
}

template<typename To, typename From>
Shared<To> cast(const Weak<From>& obj)
{
    return std::dynamic_pointer_cast<To>(obj.lock());
}

#endif

template<typename T>
using ConstPtr = T const*;