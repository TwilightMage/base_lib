#pragma once

#include "BasicTypes.h"
#include "FastOperator.h"
#include "String.h"

class EXPORT Name : public ISerializable, public IConvertible<String>
{
public:
    Name();
    Name(const Name& rhs);
    Name(const char* c_str);
    explicit Name(const String& str);
    ~Name();

    const char* c() const;

    FAST_OPERATOR(Name, ==, hash_)
    FAST_OPERATOR(Name, <=, hash_)
    FAST_OPERATOR(Name, <, hash_)
    FAST_OPERATOR(Name, !=, hash_)
    FAST_OPERATOR(Name, >, hash_)
    FAST_OPERATOR(Name, >=, hash_)

    Name& operator=(const Name& rhs);

    String to_string() const { return String(data_, size_); }

    bool is_valid() const { return hash_ != 0; }

    void write_to_stream(std::ostream& stream) const override;
    void read_from_stream(std::istream& stream) override;

    void convert_to(String& to) const override;
    void convert_from(const String& from) override;

private:
    void recalculate_hash();

    char* data_;
    uint size_;
    uint64 hash_;
};
