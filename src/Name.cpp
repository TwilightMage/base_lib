#include "../include/Name.h"

Name::Name()
    : data_(nullptr)
    , size_(0)
    , hash_(0)
{
}

Name::Name(const Name& rhs)
{
    if (rhs.hash_ != 0)
    {
        size_ = rhs.size_;
        data_ = new char[size_ + 1];
        memcpy(data_, rhs.data_, size_ + 1);
        hash_ = rhs.hash_;
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
        hash_ = 0;
    }
}

Name::Name(const char* c_str)
{
    size_ = strlen(c_str);
    if (size_ > 0)
    {
        data_ = new char[size_ + 1];
        memcpy(data_, c_str, size_ + 1);

        recalculate_hash();
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
        hash_ = 0;
    }
}

Name::Name(const String& str)
{
    if (str.length() > 0)
    {
        data_ = new char[str.length() + 1];
        memcpy(data_, str.c(), str.length() + 1);
        size_ = str.length();

        recalculate_hash();
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
        hash_ = 0;
    }
}

Name::~Name()
{
    delete[] data_;
}

const char* Name::c() const
{
    return data_;
}

Name& Name::operator=(const Name& rhs)
{
    if (&rhs == this) return *this;

    if (rhs.hash_ != 0)
    {
        size_ = rhs.size_;
        data_ = new char[size_ + 1];
        memcpy(data_, rhs.data_, size_ + 1);
        hash_ = rhs.hash_;
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
        hash_ = 0;
    }

    return *this;
}

void Name::write_to_stream(std::ostream& stream) const
{
    StreamUtils::write_c_string(stream, data_);
}

void Name::read_from_stream(std::istream& stream)
{
    delete data_;
    StreamUtils::read_c_string(stream, data_, size_);
    recalculate_hash();
}

void Name::convert_to(String& to) const
{
    to = to_string();
}

void Name::convert_from(const String& from)
{
    if (from.length() > 0)
    {
        data_ = new char[from.length() + 1];
        memcpy(data_, from.c(), from.length() + 1);

        recalculate_hash();
    }
    else
    {
        data_ = nullptr;
        size_ = 0;
        hash_ = 0;
    }
}

void Name::recalculate_hash()
{
    const int p = 31;
    const int m = static_cast<int>(1e9 + 9);
    hash_ = 0;
    uint64 p_pow = 1;
    for (uint i = 0; i < size_; i++)
    {
        hash_ = (hash_ + (data_[i] - 'a' + 1) * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
}
