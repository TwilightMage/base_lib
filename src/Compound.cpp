#include "../include/base_lib/Compound.h"

#include <deque>

#include "../include/base_lib/Math.h"

/*Map<int, SaveGame::Parser> parsers = {
    {1, {
        [](std::ifstream& file) -> std::shared_ptr<SaveGame::Metadata>
        {
            file.seekg(0);
            file.ignore(3);
            file.ignore(sizeof(int));
            file.ignore(sizeof(int));
            
            auto meta = std::make_shared<SaveGame::Metadata>();
            
            const int thumbnail_size = read<int>(file);

            meta->thumbnail.resize(thumbnail_size);
            memcpy(meta->thumbnail.data(), read_array<char>(file, thumbnail_size), thumbnail_size);

            meta->save_time = std::chrono::time_point<std::chrono::system_clock>{std::chrono::seconds{read<int>(file)}};

            return meta;
        },
        [](std::ifstream& file) -> std::shared_ptr<Compound::Value>
        {
            file.seekg(0);
            file.ignore(3);
            file.ignore(sizeof(int));
            const int meta_size = read<int>(file);
            file.ignore(meta_size);
            
            return std::make_shared<Compound::Value>(read_value(file));
        }
    }}
};*/

Compound::TypeMismatchException::TypeMismatchException(Type desired_type, Type actual_type, uint64 desired_binary_size, uint64 actual_binary_size)
    : desired_type(desired_type)
    , actual_type(actual_type)
    , desired_binary_size(desired_binary_size)
    , actual_binary_size(actual_binary_size)
{
}

char const* Compound::TypeMismatchException::what() const
{
    String desired = "[" + type_names[static_cast<int>(desired_type)] + "]";
    if (desired_binary_size > 0) desired += "<" + String::make(desired_binary_size) + " chars>";

    String actual = "[" + type_names[static_cast<int>(actual_type)] + "]";
    if (actual_binary_size > 0) actual += "<" + String::make(actual_binary_size) + " chars>";
    
    return String::format("Attempt to get value of type %s, when actual type was %s", desired.c(), actual.c()).c();
}

Compound::ParseException::ParseException(uint line, uint column, const String& bad_token)
    : line(line)
    , column(column)
    , bad_token(bad_token)
{
}

char const* Compound::ParseException::what() const
{
    if (bad_token.length() > 0)
    {
        return String::format("%i:%i bad token \"%s\"", line, column, bad_token.c()).c();
    }
    else
    {
        return String::format("%i:%i bad token", line, column).c();
    }
}

String Compound::Convert::JSON::format_value(const Value& val) const
{
    if (val.get_type() != Type::Object && val.get_type() != Type::Array) return "";

    std::stringstream stream;
    write(stream, val, 0);
    return stream.str();
}

bool Compound::Convert::JSON::write_to(std::ostream& stream, const Value& val) const
{
    if (val.get_type() != Type::Object && val.get_type() != Type::Array) return false;
    
    write(stream, val, 0);
    return true;
}

Compound::Value Compound::Convert::JSON::parse_value(const String& str) const
{
    reader = TextReader(str);
    if (reader.peek() == '[') return read_array();
    if (reader.peek() == '{') return read_object();
    throw ParseException(reader.get_line(), reader.get_column());
}

void Compound::Convert::JSON::write(std::ostream& stream, const Value& val, uint depth) const
{
    switch (val.get_type())
    {
    case Type::Null:
        stream << "null";
        break;
    case Type::String:
        stream << "\"" + ((String)val).escape_chars() + "\"";
        break;
    case Type::Char:
        stream << val.get_int32();
        break;
    case Type::Short:
        stream << val.get_short();
        break;
    case Type::Int32:
        stream << val.get_int32();
        break;
    case Type::Int64:
        stream << val.get_int64();
        break;
    case Type::Bool:
        stream << ((bool)val ? "true" : "false");
        break;
    case Type::Float:
        {
            String strf = String::make((float)val);
            if (!strf.contains('.')) strf += ".0";
            stream << strf;
        }
        break;
    case Type::Double:
        {
            String strd = String::make((double)val);
            if (!strd.contains('.')) strd += ".0";
            stream << strd;
        }
        break;
    case Type::Array:
        if (((Array)val).length() == 0)
        {
            stream << "[]";
        }
        else
        {
            stream << "[" << new_line() << tab_offset(depth + 1);
            const auto arr = val.get_array();
            for (uint i = 0; i < arr.length(); i++)
            {
                write(stream, arr[i], depth + 1);
                if (i < arr.length() - 1) stream << ',' << new_line() << tab_offset(depth + 1);
            }
            stream << new_line() << tab_offset(depth) << "]";
        }
        break;
    case Type::Object:
        if (((Object)val).size() == 0)
        {
            stream << "{}";
        }
        else
        {
            stream << "{" << new_line() << tab_offset(depth + 1);
            const auto obj = val.get_object();
            for (uint i = 0; i < obj.size(); i++)
            {
                stream << "\"" << obj.entries[i]->key.escape_chars() << "\": ";
                write(stream, obj.entries[i]->value, depth + 1);
                if (i < obj.size() - 1) stream << ',' << new_line() << tab_offset(depth + 1);
            }
            stream << new_line() << tab_offset(depth) << "}";
        }
        break;
    case Type::BinaryData:
        stream << "null"; // if you want to work with binary data - use native format, json is only for human-computer comunication
        break;
    }
}

Compound::Value Compound::Convert::JSON::read_value() const
{
    const auto ch = reader.peek();
    if (ch == '"')
    {
        return read_string();
    }
    if (isdigit(ch) || ch == '-')
    {
        return read_number();
    }
    if (tolower(ch) == 'n')
    {
        read_null();
        return Value();
    }
    if (tolower(ch) == 't' || tolower(ch) == 'f')
    {
        return read_bool();
    }
    if (ch == '{')
    {
        return read_object();
    }
    if (ch == '[')
    {
        return read_array();
    }

    throw ParseException(reader.get_line(), reader.get_column());
}

void Compound::Convert::JSON::read_null() const
{
    String token;
    while (!reader.is_done() && !space_chars.contains(reader.peek()) && reader.peek() != '}' && reader.peek() != ']' && reader.peek() != ',')
    {
        token += reader.get();
    }

    if (token.to_lower() != "null")
    {
        throw ParseException(reader.get_line(), reader.get_column(), token);
    }
}

String Compound::Convert::JSON::read_string() const
{
    if (reader.get() != '"') throw ParseException(reader.get_line(), reader.get_column());

    String token = "";
    bool protect = false;
    
    while(true)
    {
        if (reader.is_done()) throw ParseException(reader.get_line(), reader.get_column(), token);
        
        const char ch = reader.get();

        if (protect)
        {
            token += String::unescape_char(ch);
            protect = false;
        }
        else
        {
            if (ch == '\\')
            {
                protect = true;
            }
            else if (ch == '"')
            {
                break;
            }
            else
            {
                token += ch;
            }
        }
    }
    
    return token;
}

Compound::Value Compound::Convert::JSON::read_number() const
{
    String token = "";

    bool real = false;
    uint real_size = 0;

    byte stage = 0;
    
    while (true)
    {
        if (reader.is_done()) throw ParseException(reader.get_line(), reader.get_column(), token);

        const char ch = reader.peek();

        if (isdigit(ch) || ch == '.' || ch == '-' || ch == '+')
        {
            token += reader.get();
            if ((ch == '-' || ch == '+') && stage > 0) throw ParseException(reader.get_line(), reader.get_column(), token);
            if (isdigit(ch))
            {
                if (stage == 0 || stage == 2) ++stage;
                if (real) ++real_size;
            }
            if (ch == '.')
            {
                if (stage > 1) throw ParseException(reader.get_line(), reader.get_column(), token);
                real = true;
                stage = 2;
            }
        }
        else if (space_chars.contains(ch) || ch == ',' || ch == ']' || ch == '}')
        {
            break;
        }
        else
        {
            throw ParseException(reader.get_line(), reader.get_column(), token);
        }
    }
    
    if (real)
    {
        const double number = String::parse<double>(token);
        double whole;
        modf(number, &whole);
        if (number < (double)std::numeric_limits<float>::min() || number > (double)std::numeric_limits<float>::max() || real_size > 7) return number;
        return float(number);
    }
    else
    {
        const int64 number = String::parse<int64>(token);
        if (number > 127 || number < -128)
            if (number > 32767 || number < -32768)
                if (number > 2147483647 || number < -2147483648)
                    return number;
                else return int32(number);
            else return short(number);
        else return char(number);
    }
}

bool Compound::Convert::JSON::read_bool() const
{
    String token;
    while (!reader.is_done() && !space_chars.contains(reader.peek()) && reader.peek() != '}' && reader.peek() != ']' && reader.peek() != ',')
    {
        token += reader.get();
    }

    token = token.to_lower();

    if (token != "true" && token != "false")
    {
        throw ParseException(reader.get_line(), reader.get_column(), token);
    }

    return token == "true";
}

Compound::Object Compound::Convert::JSON::read_object() const
{
    if (reader.get() != '{') throw ParseException(reader.get_line(), reader.get_column());

    reader.skip(space_chars);

    Object object;

    while(true)
    {
        if (reader.is_done()) throw ParseException(reader.get_line(), reader.get_column());

        const bool comma = reader.peek() == ',';
        if (comma) reader.skip(1);

        reader.skip(space_chars);

        if (reader.peek() == '}')
        {
            reader.skip(1);
            break;
        }

        if (object.size() > 0 && !comma)
        {
            throw ParseException(reader.get_line(), reader.get_column());
        }
        
        const String key = read_string();

        reader.skip(space_chars);

        if (reader.get() != ':') throw ParseException(reader.get_line(), reader.get_column());

        reader.skip(space_chars);

        const Value value = read_value();

        reader.skip(space_chars);

        object[key] = value;
    }
    
    return object;
}

Compound::Array Compound::Convert::JSON::read_array() const
{
    if (reader.get() != '[') throw ParseException(reader.get_line(), reader.get_column());

    reader.skip(space_chars);

    Array array;

    while(true)
    {
        if (reader.is_done()) throw ParseException(reader.get_line(), reader.get_column());

        const bool comma = reader.peek() == ',';
        if (comma) reader.skip(1);

        reader.skip(space_chars);

        if (reader.peek() == ']')
        {
            reader.skip(1);
            break;
        }

        if (array.length() > 0 && !comma)
        {
            throw ParseException(reader.get_line(), reader.get_column());
        }

        const Value value = read_value();

        reader.skip(space_chars);

        array += value;
    }
    
    return array;
}

String Compound::Convert::YAML::format_value(const Value& val) const
{
    if (val.get_type() != Type::Object) return "";
    
    std::stringstream stream;
    stream << "---";
    write(stream, val, 0);
    return stream.str();
}

bool Compound::Convert::YAML::write_to(std::ostream& stream, const Value& val) const
{
    if (val.get_type() != Type::Object) return false;
    
    stream << "---";
    write(stream, val, 0);
    return true;
}

void Compound::Convert::YAML::write(std::ostream& stream, const Value& val, uint depth) const
{
    switch (val.get_type())
    {
    case Type::Null:
        stream << "null";
        break;
    case Type::String:
        stream << "\"" + ((String)val).escape_chars() + "\"";
        break;
    case Type::Char:
        stream << val.get_int32();
        break;
    case Type::Short:
        stream << val.get_short();
        break;
    case Type::Int32:
        stream << val.get_int32();
        break;
    case Type::Int64:
        stream << val.get_int64();
        break;
    case Type::Bool:
        stream << ((bool)val ? "true" : "false");
        break;
    case Type::Float:
        {
            String strf = String::make((float)val);
            if (!strf.contains('.')) strf += ".0";
            stream << strf;
        }
        break;
    case Type::Double:
        {
            String strd = String::make((double)val);
            if (!strd.contains('.')) strd += ".0";
            stream << strd;
        }
        break;
    case Type::Array:
        if (((Array)val).length() == 0)
        {
             stream << "null";
        }
        else
        {
            const auto arr = val.get_array();
            for (uint i = 0; i < arr.length(); i++)
            {
                stream << "\n";
                stream << String(' ', depth * 2) << "- ";
                write(stream, arr[i], depth + 1);
            }
        }
        break;
    case Type::Object:
        if (((Object)val).size() == 0)
        {
            stream << "null";
        }
        else
        {
            for (const auto& pair : val.get_object())
            {
                stream << "\n";
                stream << String(' ', depth * 2) << pair->key << ": ";
                write(stream, pair->value, depth + 1);
            }
        }
        break;
    case Type::BinaryData:
        stream << "null"; // if you want to work with binary data - use native format, json is only for human-computer comunication
        break;
    }
}

Compound::Array::Array()
    : List<Value>()
{
}

Compound::Array::Array(const Array& rhs)
    : List<Value>(rhs)
{
}

Compound::Array::Array(const List<Value>& rhs)
    : List<Value>(rhs)
{
}

Compound::Array::Array(const std::initializer_list<Value>& rhs)
    : List<Value>(rhs)
{
}

Compound::Array& Compound::Array::operator=(const Array& rhs)
{
    if (this == &rhs) return *this;

    List<Value>::operator=(rhs);

    return *this;
}

Compound::Array& Compound::Array::operator=(const List<Value>& rhs)
{
    List<Value>::operator=(rhs);

    return *this;
}

bool Compound::Array::operator==(const Array& rhs) const
{
    if (this == &rhs) return true;
    
    return List<Value>::operator==(rhs);
}

bool Compound::Array::operator==(const List<Value>& rhs) const
{
    return List<Value>::operator==(rhs);
}

Shared<Compound::ArrayDiff> Compound::Array::build_diff_against(const Array& base) const
{
    auto diff = MakeShared<ArrayDiff>();

    diff->newLength = length();

    for (uint i = 0; i < Math::min(length(), base.length()); i++)
    {
        if (at(i).get_type() == Type::Array && base.at(i).get_type() == Type::Array)
        {
            const auto val_diff = ((Array)at(i)).build_diff_against((Array)base.at(i));
            if (!val_diff->is_empty())
            {
                diff->merge[i] = val_diff;
            }
        }
        else if (at(i).get_type() == Type::Array && base.at(i).get_type() == Type::Array)
        {
            const auto val_diff = ((Object)at(i)).build_diff_against((Object)base.at(i));
            if (!val_diff->is_empty())
            {
                diff->merge[i] = val_diff;
            }
        }
        else if (at(i) != base.at(i))
        {
            diff->set[i] = at(i);
        }
    }

    for (uint i = base.length(); i < length(); i++)
    {
        diff->set[i] = at(i);
    }
    
    return diff;
}

void Compound::Array::apply_diff(const Shared<ArrayDiff>& diff)
{
    resize(diff->newLength);

    for (const auto& setted : diff->set)
    {
        at(setted->key) = setted->value;
    }

    for (const auto& merged : diff->merge)
    {
        auto& val = at(merged->key);
        val.set_type(merged->value->get_target_type());
        if (merged->value->get_target_type() == Type::Array)
        {
            val.access_array()->apply_diff(cast<ArrayDiff>(merged->value));
        }
        else if (merged->value->get_target_type() == Type::Object)
        {
            val.access_object()->apply_diff(cast<ObjectDiff>(merged->value));
        }
    }
}

Compound::Object::Object()
    : SimpleMap<String, Value>()
{
}

Compound::Object::Object(const Object& rhs)
    : SimpleMap<String, Value>(rhs)
{
}

Compound::Object::Object(const SimpleMap<String, Value>& rhs)
    : SimpleMap<String, Value>(rhs)
{
}

Compound::Object::Object(const std::initializer_list<Pair<String, Value>>& rhs)
    : SimpleMap<String, Value>(rhs)
{
}

Compound::Object& Compound::Object::operator=(const Object& rhs)
{
    if (this == &rhs) return *this;

    SimpleMap<String, Value>::operator=(rhs);

    return *this;
}

Compound::Object& Compound::Object::operator=(const SimpleMap<String, Value>& rhs)
{
    SimpleMap<String, Value>::operator=(rhs);

    return *this;
}

bool Compound::Object::operator==(const Object& rhs) const
{
    if (this == &rhs) return true;
    
    return SimpleMap<String, Value>::operator==(rhs);
}

bool Compound::Object::operator==(const SimpleMap<String, Value>& rhs) const
{
    return SimpleMap<String, Value>::operator==(rhs);
}

bool Compound::Object::contains_of_type(const String& key, Type type) const
{
    if (auto entry = find(key))
    {
        return entry->get_type() == type;
    }

    return false;
}

bool Compound::Object::contains_non_null(const String& key) const
{
    if (auto entry = find(key))
    {
        return entry->get_type() != Type::Null;
    }

    return false;
}

bool Compound::Object::contains_number(const String& key) const
{
    if (auto entry = find(key))
    {
        return entry->is_number();
    }

    return false;
}

Shared<Compound::ObjectDiff> Compound::Object::build_diff_against(const Object& base) const
{
    auto diff = MakeShared<ObjectDiff>();
    
    for (const auto& base_entry : base.entries)
    {
        if (!contains(base_entry->key))
        {
            diff->remove.add(base_entry->key);
        }
    }

    for (const auto& my_entry : entries)
    {
        if (const auto base_entry = base.find(my_entry->key))
        {
            if (base_entry->get_type() == Type::Array && my_entry->value.get_type() == Type::Array)
            {
                const auto val_diff = ((Array)my_entry->value).build_diff_against((Array)*base_entry);
                if (!val_diff->is_empty())
                {
                    diff->merge[my_entry->key] = val_diff;
                }
            }
            else if (base_entry->get_type() == Type::Object && my_entry->value.get_type() == Type::Object)
            {
                const auto val_diff = ((Object)my_entry->value).build_diff_against((Object)*base_entry);
                if (!val_diff->is_empty())
                {
                    diff->merge[my_entry->key] = val_diff;
                }
            }
            else if (*base_entry != my_entry->value)
            {
                diff->set[my_entry->key] = my_entry->value;
            }
        }
        else
        {
            diff->set[my_entry->key] = my_entry->value;
        }
    }

    return diff;
}

void Compound::Object::apply_diff(const Shared<ObjectDiff>& diff)
{
    for (const auto& removed : diff->remove)
    {
        remove(removed);
    }

    for (const auto& seted : diff->set)
    {
        operator[](seted->key) = seted->value;
    }

    for (const auto& merged : diff->merge)
    {
        auto& val = at(merged->key);
        val.set_type(merged->value->get_target_type());
        if (merged->value->get_target_type() == Type::Array)
        {
            val.access_array()->apply_diff(cast<ArrayDiff>(merged->value));
        }
        else if (merged->value->get_target_type() == Type::Object)
        {
            val.access_object()->apply_diff(cast<ObjectDiff>(merged->value));
        }
    }
}

// ----------------------------- Object String

String Compound::Object::get_string(const String& key, const String& default_value) const
{
    if (const auto value = find(key)) { return value->get_string(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_string(const String& key, const String& value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, const String& value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object char

char Compound::Object::get_char(const String& key, char default_value) const
{
    if (const auto value = find(key)) { return value->get_char(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_char(const String& key, char value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, char value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object short

short Compound::Object::get_short(const String& key, short default_value) const
{
    if (const auto value = find(key)) { return value->get_short(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_short(const String& key, short value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, short value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object int32

int32 Compound::Object::get_int32(const String& key, int32 default_value) const
{
    if (const auto value = find(key)) { return value->get_int32(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_int32(const String& key, int32 value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, int32 value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object int64

int64 Compound::Object::get_int64(const String& key, int64 default_value) const
{
    if (const auto value = find(key)) { return value->get_int64(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_int64(const String& key, int64 value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, int64 value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object bool

bool Compound::Object::get_bool(const String& key, bool default_value) const
{
    if (const auto value = find(key)) { return value->get_bool(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_bool(const String& key, bool value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, bool value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object float

float Compound::Object::get_float(const String& key, float default_value) const
{
    if (const auto value = find(key)) { return value->get_float(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_float(const String& key, float value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, float value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object double

double Compound::Object::get_double(const String& key, double default_value) const
{
    if (const auto value = find(key)) { return value->get_double(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_double(const String& key, double value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, double value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object Array

Compound::Array Compound::Object::get_array(const String& key, const Array& default_value) const
{
    if (const auto value = find(key)) { return value->get_array(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_array(const String& key, const Array& value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, const Array& value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object Object

Compound::Object Compound::Object::get_object(const String& key, const Object& default_value) const
{
    if (const auto value = find(key)) { return value->get_object(); }
    return default_value;
}

Compound::Object& Compound::Object::chain_object(const String& key, const Object& value)
{
    operator[](key) = value;
    return *this;
}

Compound::Object& Compound::Object::chain(const String& key, const Object& value)
{
    operator[](key) = value;
    return *this;
}

// ----------------------------- Object End

bool Compound::Convert::IParser::try_parse_value(const String& str, Value& out_value) const
{
    try
    {
        out_value = parse_value(str);
        return true;
    }
    catch (ParseException&)
    {
        out_value = Object();
        return false;
    }
    catch (...)
    {
        throw;
    }
}

void Compound::Value::write_to_stream(std::ostream& stream) const
{
    StreamUtils::write(stream, (char)type);
    switch (type)
    {
    case Type::Null:
        break;
    case Type::String:
        StreamUtils::write(stream, get_data<String>());
        break;
    case Type::Char:
        StreamUtils::write(stream, get_data<char>());
        break;
    case Type::Short:
        StreamUtils::write(stream, get_data<short>());
        break;
    case Type::Int32:
        StreamUtils::write(stream, get_data<int32>());
        break;
    case Type::Int64:
        StreamUtils::write(stream, get_data<int64>());
        break;
    case Type::Bool:
       StreamUtils::write(stream, get_data<bool>());
        break;
    case Type::Float:
       StreamUtils::write(stream, get_data<float>());
        break;
    case Type::Double:
        StreamUtils::write(stream, get_data<double>());
        break;
    case Type::Array:
        StreamUtils::write(stream, get_data<Array>());
        break;
    case Type::Object:
        StreamUtils::write(stream, get_data<Object>());
        break;
    case Type::BinaryData:
        StreamUtils::write(stream, binary_type_size);
        StreamUtils::write_array(stream, get_data<void*>(), binary_type_size);
        break;
    }
}

void Compound::Value::read_from_stream(std::istream& stream)
{
    auto value_type = (Type)StreamUtils::read<char>(stream);
    switch (value_type)
    {
    case Type::Null:
        set_null();
        break;
    case Type::String:
        set_string(StreamUtils::read<String>(stream));
        break;
    case Type::Char:
        set_char(StreamUtils::read<char>(stream));
        break;
    case Type::Short:
        set_short(StreamUtils::read<short>(stream));
        break;
    case Type::Int32:
        set_int32(StreamUtils::read<int32>(stream));
        break;
    case Type::Int64:
        set_int64(StreamUtils::read<int64>(stream));
        break;
    case Type::Bool:
        set_bool(StreamUtils::read<bool>(stream));
        break;
    case Type::Float:
        set_float(StreamUtils::read<float>(stream));
        break;
    case Type::Double:
        set_double(StreamUtils::read<double>(stream));
        break;
    case Type::Array:
        set_array(StreamUtils::read<Array>(stream));
        break;
    case Type::Object:
        set_object(StreamUtils::read<Object>(stream));
        break;
    case Type::BinaryData:
        const auto size = StreamUtils::read<uint64>(stream);
        char* chars = new char[size];
        StreamUtils::read_array(stream, chars, size);
        set_binary_data(chars, size);
        delete[] chars;
        break;
        
    }
}

Compound::Value::Value(const Value& rhs)
{
    copy_from(rhs);
}

Compound::Value::Value(Type type)
    : type(type)
{
    create_data();
}

// ----------------------------- Value Null

Compound::Value::Value()
    : data(nullptr)
    , type(Type::Null)
{
}

void Compound::Value::set_null()
{
    if (type == Type::Null) return;

    delete_data();

    type = Type::Null;
}

void Compound::Value::set(nullptr_t null)
{
    set_null();
}

Compound::Value& Compound::Value::operator=(nullptr_t null)
{
    set_null();

    return *this;
}

bool Compound::Value::operator==(nullptr_t null) const
{
    return type == Type::Null;
}

// ----------------------------- Value String

Compound::Value::Value(const String& str) { set_string_internal(str); }

void Compound::Value::set_string(const String& str)
{
    if (type != Type::String)
    {
        delete_data();
        type = Type::String;
        set_string_internal(str);
    }
    else get_data<String>() = str;
}

void Compound::Value::set(const String& str) { set_string(str); }

Compound::Value& Compound::Value::operator=(const String& str)
{
    set_string(str);
    return *this;
}

bool Compound::Value::operator==(const String& str) const { return type == Type::String && get_data<String>() == str; }

Compound::Value::operator String&()
{
    if (type != Type::String) throw TypeMismatchException(Type::String, type);
    return get_data<String>();
}

Compound::Value::operator const String&() const
{
    if (type != Type::String) throw TypeMismatchException(Type::String, type);
    return get_data<String>();
}

String Compound::Value::get_string(const String& default_value) const
{
    if (type != Type::String) return default_value;
    return get_data<String>();
}

void Compound::Value::convert_to(String& to) const
{
    to = get_string();
}

void Compound::Value::convert_from(const String& from)
{
    set_string(from);
}

// ----------------------------- Value char

Compound::Value::Value(char ch) { set_char_internal(ch); }

void Compound::Value::set_char(char ch)
{
    if (type != Type::Char)
    {
        delete_data();
        type = Type::Char;
        set_char_internal(ch);
    }
    else get_data<char>() = ch;
}

void Compound::Value::set(char ch) { set_char(ch); }

Compound::Value& Compound::Value::operator=(char ch)
{
    set_char(ch);
    return *this;
}

bool Compound::Value::operator==(char ch) const { return type == Type::Char && get_data<char>() == ch; }

Compound::Value::operator char&()
{
    if (type != Type::Char) throw TypeMismatchException(Type::Char, type);
    return get_data<char>();
}

Compound::Value::operator const char&() const
{
    if (type != Type::Char) throw TypeMismatchException(Type::Char, type);
    return get_data<char>();
}

char Compound::Value::get_char(char default_value) const
{
    if (!is_number()) return default_value;
    return get_data_numeric<char>();
}

void Compound::Value::convert_to(char& to) const
{
    to = get_char();
}

void Compound::Value::convert_from(const char& from)
{
    set_char(from);
}

// ----------------------------- Value short

Compound::Value::Value(short sh) { set_short_internal(sh); }

void Compound::Value::set_short(short sh)
{
    if (type != Type::Short)
    {
        delete_data();
        type = Type::Short;
        set_short_internal(sh);
    }
    else get_data<short>() = sh;
}

void Compound::Value::set(short sh) { set_short(sh); }

Compound::Value& Compound::Value::operator=(short sh)
{
    set_short(sh);
    return *this;
}

bool Compound::Value::operator==(short sh) const { return type == Type::Short && get_data<short>() == sh; }

Compound::Value::operator short&()
{
    if (type != Type::Short) throw TypeMismatchException(Type::Short, type);
    return get_data<short>();
}

Compound::Value::operator const short&() const
{
    if (type != Type::Short) throw TypeMismatchException(Type::Short, type);
    return get_data<short>();
}

short Compound::Value::get_short(short default_value) const
{
    if (!is_number()) return default_value;
    return get_data_numeric<short>();
}

void Compound::Value::convert_to(short& to) const
{
    to = get_short();
}

void Compound::Value::convert_from(const short& from)
{
    set_short(from);
}

// ----------------------------- Value int32

Compound::Value::Value(int32 i) { set_int32_internal(i); }

void Compound::Value::set_int32(int32 i)
{
    if (type != Type::Int32)
    {
        delete_data();
        type = Type::Int32;
        set_int32_internal(i);
    }
    else get_data<int32>() = i;
}

void Compound::Value::set(int32 i) { set_int32(i); }

Compound::Value& Compound::Value::operator=(int32 i)
{
    set_int32(i);
    return *this;
}

bool Compound::Value::operator==(int32 i) const { return type == Type::Int32 && get_data<int32>() == i; }

Compound::Value::operator int32&()
{
    if (type != Type::Int32) throw TypeMismatchException(Type::Int32, type);
    return get_data<int32>();
}

Compound::Value::operator const int32&() const
{
    if (type != Type::Int32) throw TypeMismatchException(Type::Int32, type);
    return get_data<int32>();
}

int32 Compound::Value::get_int32(int32 default_value) const
{
    if (!is_number()) return default_value;
    return get_data_numeric<int32>();
}

void Compound::Value::convert_to(int32& to) const
{
    to = get_int32();
}

void Compound::Value::convert_from(const int32& from)
{
    set_int32(from);
}

// ----------------------------- Value int64

Compound::Value::Value(int64 i) { set_int64_internal(i); }

void Compound::Value::set_int64(int64 i)
{
    if (type != Type::Int64)
    {
        delete_data();
        type = Type::Int64;
        set_int64_internal(i);
    }
    else get_data<int64>() = i;
}

void Compound::Value::set(int64 i) { set_int64(i); }

Compound::Value& Compound::Value::operator=(int64 i)
{
    set_int64(i);
    return *this;
}

bool Compound::Value::operator==(int64 i) const { return type == Type::Int64 && get_data<int64>() == i; }

Compound::Value::operator int64&()
{
    if (type != Type::Int64) throw TypeMismatchException(Type::Int64, type);
    return get_data<int64>();
}

Compound::Value::operator const int64&() const
{
    if (type != Type::Int64) throw TypeMismatchException(Type::Int64, type);
    return get_data<int64>();
}

int64 Compound::Value::get_int64(int64 default_value) const
{
    if (!is_number()) return default_value;
    return get_data_numeric<int64>();
}

void Compound::Value::convert_to(int64& to) const
{
    to = get_int64();
}

void Compound::Value::convert_from(const int64& from)
{
    set_int64(from);
}

// ----------------------------- Value bool

Compound::Value::Value(bool b) { set_bool_internal(b); }

void Compound::Value::set_bool(bool b)
{
    if (type != Type::Bool)
    {
        delete_data();
        type = Type::Bool;
        set_bool_internal(b);
    }
    else get_data<bool>() = b;
}

void Compound::Value::set(bool b) { set_bool(b); }

Compound::Value& Compound::Value::operator=(bool b)
{
    set_bool(b);
    return *this;
}

bool Compound::Value::operator==(bool b) const { return type == Type::Bool && get_data<bool>() == b; }

Compound::Value::operator bool&()
{
    if (type != Type::Bool) throw TypeMismatchException(Type::Bool, type);
    return get_data<bool>();
}

Compound::Value::operator const bool&() const
{
    if (type != Type::Bool) throw TypeMismatchException(Type::Bool, type);
    return get_data<bool>();
}

bool Compound::Value::get_bool(bool default_value) const
{
    if (!is_number()) return default_value;
    return get_data_numeric<bool>();
}

void Compound::Value::convert_to(bool& to) const
{
    to = get_bool();
}

void Compound::Value::convert_from(const bool& from)
{
    set_bool(from);
}

// ----------------------------- Value float

Compound::Value::Value(float f) { set_float_internal(f); }

void Compound::Value::set_float(float f)
{
    if (type != Type::Float)
    {
        delete_data();
        type = Type::Float;
        set_float_internal(f);
    }
    else get_data<float>() = f;
}

void Compound::Value::set(float f) { set_float(f); }

Compound::Value& Compound::Value::operator=(float f)
{
    set_float(f);
    return *this;
}

bool Compound::Value::operator==(float f) const { return type == Type::Float && get_data<float>() == f; }

Compound::Value::operator float&()
{
    if (type != Type::Float) throw TypeMismatchException(Type::Float, type);
    return get_data<float>();
}

Compound::Value::operator const float&() const
{
    if (type != Type::Float) throw TypeMismatchException(Type::Float, type);
    return get_data<float>();
}

float Compound::Value::get_float(float default_value) const
{
    if (!is_number()) return default_value;
    return get_data_numeric<float>();
}

void Compound::Value::convert_to(float& to) const
{
    to = get_float();
}

void Compound::Value::convert_from(const float& from)
{
    set_float(from);
}

// ----------------------------- Value double

Compound::Value::Value(double d) { set_double_internal(d); }

void Compound::Value::set_double(double d)
{
    if (type != Type::Double)
    {
        delete_data();
        type = Type::Double;
        set_double_internal(d);
    }
    else get_data<double>() = d;
}

void Compound::Value::set(double d) { set_double(d); }

Compound::Value& Compound::Value::operator=(double d)
{
    set_double(d);
    return *this;
}

bool Compound::Value::operator==(double d) const { return type == Type::Double && get_data<double>() == d; }

Compound::Value::operator double&()
{
    if (type != Type::Double) throw TypeMismatchException(Type::Double, type);
    return get_data<double>();
}

Compound::Value::operator const double&() const
{
    if (type != Type::Double) throw TypeMismatchException(Type::Double, type);
    return get_data<double>();
}

double Compound::Value::get_double(double default_value) const
{
    if (!is_number()) return default_value;
    return get_data_numeric<double>();
}

void Compound::Value::convert_to(double& to) const
{
    to = get_double();
}

void Compound::Value::convert_from(const double& from)
{
    set_double(from);
}

// ----------------------------- Value Array

Compound::Value::Value(const Array& arr) { set_array_internal(arr); }

void Compound::Value::set_array(const Array& arr)
{
    if (type != Type::Array)
    {
        delete_data();
        type = Type::Array;
        set_array_internal(arr);
    }
    else get_data<Array>() = arr;
}

void Compound::Value::set(const Array& arr) { set_array(arr); }

Compound::Value& Compound::Value::operator=(const Array& arr)
{
    set_array(arr);
    return *this;
}

bool Compound::Value::operator==(const Array& arr) const
{
    return type == Type::Array && get_data<Array>() == arr;
}

Compound::Value::operator Compound::Array&()
{
    if (type != Type::Array) throw TypeMismatchException(Type::Array, type);
    return get_data<Array>();
}

Compound::Value::operator const Compound::Array&() const
{
    if (type != Type::Array) throw TypeMismatchException(Type::Array, type);
    return get_data<Array>();
}

Compound::Array Compound::Value::get_array(const Array& default_value) const
{
    if (type != Type::Array) return default_value;
    return get_data<Array>();
}

Compound::Array* Compound::Value::access_array() const
{
    if (type == Type::Array) return (Array*)data;
    return nullptr;
}

void Compound::Value::convert_to(Array& to) const
{
    to = get_array();
}

void Compound::Value::convert_from(const Array& from)
{
    set_array(from);
}

// ----------------------------- Value Object

Compound::Value::Value(const Object& obj) { set_object_internal(obj); }

void Compound::Value::set_object(const Object& obj)
{
    if (type != Type::Object)
    {
        delete_data();
        type = Type::Object;
        set_object_internal(obj);
    }
    else get_data<Object>() = obj;
}

void Compound::Value::set(const Object& obj) { set_object(obj); }

Compound::Value& Compound::Value::operator=(const Object& obj)
{
    set_object(obj);
    return *this;
}

bool Compound::Value::operator==(const Object& obj) const
{
    return type == Type::Object && get_data<Object>() == obj;
}

Compound::Value::operator Compound::Object&()
{
    if (type != Type::Object) throw TypeMismatchException(Type::Object, type);
    return get_data<Object>();
}

Compound::Value::operator const Compound::Object&() const
{
    if (type != Type::Object) throw TypeMismatchException(Type::Object, type);
    return get_data<Object>();
}

Compound::Object Compound::Value::get_object(const Object& default_value) const
{
    if (type != Type::Object) return default_value;
    return get_data<Object>();
}

Compound::Object* Compound::Value::access_object() const
{
    if (type == Type::Object) return (Object*)data;
    return nullptr;
}

void Compound::Value::convert_to(Object& to) const
{
    to = get_object();
}

void Compound::Value::convert_from(const Object& from)
{
    set_object(from);
}

//------------------------------ Value Binary Data

Compound::Value::Value(const void* chars, uint64 size)
{
    set_binary_internal(chars, size);
}

void Compound::Value::set_binary_data(const void* chars, uint64 size)
{
    if (type != Type::BinaryData || size == binary_type_size)
    {
        delete_data();
        type = Type::BinaryData;
        set_binary_internal(chars, size);
    }
    else memcpy(data, chars, size);
}

void Compound::Value::set(const void*& chars, uint64 size)
{
    set_binary_data(chars, size);
}

void Compound::Value::read_binary_data(void*& bytes, uint64& size) const
{
    if (type == Type::BinaryData)
    {
        bytes = new char[binary_type_size];
        memcpy(bytes, data, binary_type_size);
        size = binary_type_size;
    }
    else
    {
        bytes = nullptr;
        size = 0;
    }
}

// ----------------------------- Value End

void Compound::Value::set_type(Type new_type)
{
    if (type == new_type) return;

    Value* old_number = is_number() && 1 << (int)new_type & numeric_type_mask ? new Value(*this) : nullptr;
    
    delete_data();
    
    type = new_type;

    if (old_number)
    {
        switch (type)
        {
        case Type::Char:
            data = new char(old_number->get_data_numeric<char>());
            break;
        case Type::Short:
            data = new short(old_number->get_data_numeric<short>());
            break;
        case Type::Int32:
            data = new int32(old_number->get_data_numeric<int32>());
            break;
        case Type::Int64:
            data = new int64(old_number->get_data_numeric<int64>());
            break;
        case Type::Bool:
            data = new bool(old_number->get_data_numeric<bool>());
            break;
        case Type::Float:
            data = new float(old_number->get_data_numeric<float>());
            break;
        case Type::Double:
            data = new double(old_number->get_data_numeric<double>());
            break;
        }
        delete old_number;
    }
    else
    {
        create_data();
    }
}

Compound::Value& Compound::Value::operator[](const String& key)
{
    set_type(Type::Object);

    return get_object().operator[](key);
}

Compound::Value& Compound::Value::operator[](uint index)
{
    set_type(Type::Array);

    return get_array().operator[](index);
}

Compound::Value& Compound::Value::operator=(const Value& rhs)
{
    if (&rhs == this) return *this;

    delete_data();

    copy_from(rhs);

    return *this;
}

bool Compound::Value::operator==(const Value& rhs) const
{
#ifdef COMPOUND_COMPARSION_EXACT
    return is_same_to(rhs);
#else
    return is_similar_to(rhs);
#endif
}

bool Compound::Value::is_similar_to(const Value& rhs) const
{
    if (is_number() && rhs.is_number())
    {
        switch (type)
        {
        case Type::Char:
            return get_data<char>() == rhs.get_data_numeric<char>();
        case Type::Short:
            return get_data<short>() == rhs.get_data_numeric<short>();
        case Type::Int32:
            return get_data<int32>() == rhs.get_data_numeric<int32>();
        case Type::Int64:
            return get_data<int64>() == rhs.get_data_numeric<int64>();
        case Type::Bool:
            return get_data<bool>() == rhs.get_data_numeric<bool>();
        case Type::Float:
            return get_data<float>() == rhs.get_data_numeric<float>();
        case Type::Double:
            return get_data<double>() == rhs.get_data_numeric<double>();
        }
    }
    /*else if (type == Type::String && rhs.is_number())
    {
        return get_data<String>() == rhs.to_string();
    }
    else if (is_number() && rhs.type == Type::String)
    {
        return to_string() == rhs.get_data<String>();
    }*/
    else if (type == rhs.type)
    {
        switch (type)
        {
        case Type::Null:
            return true;
        case Type::String:
            return get_data<String>() == rhs.get_data<String>();
        case Type::Array:
            return get_data<Array>() == rhs.get_data<Array>();
        case Type::Object:
            return get_data<Object>() == rhs.get_data<Object>();
        case Type::BinaryData:
            return binary_type_size == rhs.binary_type_size && memcmp(data, rhs.data, binary_type_size) == 0;
        }
    }

    return false;
}

bool Compound::Value::is_same_to(const Value& rhs) const
{
    if (type != rhs.type) return false;
    switch (type)
    {
    case Type::Null:
        return true;
    case Type::String:
        return get_data<String>() == rhs.get_data<String>(); 
    case Type::Char:
        return get_data<char>() == rhs.get_data<char>();
    case Type::Short:
        return get_data<short>() == rhs.get_data<short>();
    case Type::Int32:
        return get_data<int32>() == rhs.get_data<int32>();
    case Type::Int64:
        return get_data<int64>() == rhs.get_data<int64>();
    case Type::Bool:
        return get_data<bool>() == rhs.get_data<bool>();
    case Type::Float:
        return get_data<float>() == rhs.get_data<float>();
    case Type::Double:
        return get_data<double>() == rhs.get_data<double>(); 
    case Type::Array:
        return get_data<Array>() == rhs.get_data<Array>();
    case Type::Object:
        return get_data<Object>() == rhs.get_data<Object>();
    case Type::BinaryData:
        return binary_type_size == rhs.binary_type_size && memcmp(data, rhs.data, binary_type_size) == 0;
    }

    return false;
}

String Compound::Value::to_string() const
{
    switch (type)
    {
    case Type::Null:
        return "Null";
    case Type::String:
        return "\"" + get_data<String>().replace("\\", "\\\\").replace("\"", "\\\"") + "\"";
    case Type::Char:
        return String::make((int)get_data<char>());
    case Type::Short:
        return String::make(get_data<short>());
    case Type::Int32:
        return String::make(get_data<int32>());
    case Type::Int64:
        return String::make(get_data<int64>());
    case Type::Bool:
        return String::make(get_data<bool>());
    case Type::Float:
        return String::make(get_data<float>());
    case Type::Double:
        return String::make(get_data<double>());
    case Type::Array:
        return "[" + String::join(get_data<Array>().select<String>([](const Value& val) -> String{ return val.to_string(); }), ", ") + "]";
    case Type::Object:
        return "{" + String::join(get_data<Object>().entries.select<String>([](Pair<String, Value>* pair) -> String{ return "\"" + pair->key.replace("\\", "\\\\").replace("\"", "\\\"") + "\": " + pair->value.to_string(); }), ", ") + "}";
    case Type::BinaryData:
        return "<" + String::make(binary_type_size) + " bytes>";
    }

    return "";
}

void Compound::Value::create_data()
{
    switch (type)
    {
    case Type::Null:
        data = nullptr;
        break;
    case Type::String:
        data = new String();
        break;
    case Type::Char:
        data = new char(0);
        break;
    case Type::Short:
        data = new short(0);
        break;
    case Type::Int32:
        data = new int32(0);
        break;
    case Type::Int64:
        data = new int64(0);
        break;
    case Type::Bool:
        data = new bool(false);
        break;
    case Type::Float:
        data = new float(0);
        break;
    case Type::Double:
        data = new double(0);
        break;
    case Type::Array:
        data = new Array();
        break;
    case Type::Object:
        data = new Object();
        break;
    case Type::BinaryData:
        data = nullptr;
        binary_type_size = 0;
        break;
    }
}

void Compound::Value::delete_data()
{
    switch (type)
    {
    case Type::Null:
        break;
    case Type::String:
        delete (String*)data;
        break;
    case Type::Char:
        delete (char*)data;
        break;
    case Type::Short:
        delete (short*)data;
        break;
    case Type::Int32:
        delete (int32*)data;
        break;
    case Type::Int64:
        delete (int64*)data;
        break;
    case Type::Bool:
        delete (bool*)data;
        break;
    case Type::Float:
        delete (float*)data;
        break;
    case Type::Double:
        delete (double*)data;
        break;
    case Type::Array:
        delete (Array*)data;
        break;
    case Type::Object:
        delete (Object*)data;
        break;
    case Type::BinaryData:
        delete[] (char*)data;
        break;
    }

    data = nullptr;
}

void Compound::Value::copy_from(const Value& rhs)
{
    type = rhs.type;

    switch (type)
    {
    case Type::Null:
        data = nullptr;
        break;
    case Type::String:
        data = new String(rhs.get_data<String>());
        break;
    case Type::Char:
        data = new char(rhs.get_data<char>());
        break;
    case Type::Short:
        data = new short(rhs.get_data<short>());
        break;
    case Type::Int32:
        data = new int32(rhs.get_data<int32>());
        break;
    case Type::Int64:
        data = new int64(rhs.get_data<int64>());
        break;
    case Type::Bool:
        data = new bool(rhs.get_data<bool>());
        break;
    case Type::Float:
        data = new float(rhs.get_data<float>());
        break;
    case Type::Double:
        data = new double(rhs.get_data<double>());
        break;
    case Type::Array:
        data = new Array(rhs.get_data<Array>());
        break;
    case Type::Object:
        data = new Object(rhs.get_data<Object>());
        break;
    case Type::BinaryData:
        if (rhs.binary_type_size > 0)
        {
            data = new char[rhs.binary_type_size];
            memcpy(data, rhs.data, rhs.binary_type_size);
            binary_type_size = rhs.binary_type_size;
        }
        else
        {
            data = nullptr;
            binary_type_size = 0;
        }
        break;
    }
}


void Compound::Value::set_string_internal(const String& str)
{
    data = new String(str);
    type = Type::String;
}

void Compound::Value::set_char_internal(char b)
{
    data = new char(b);
    type = Type::Char;
}

void Compound::Value::set_short_internal(short s)
{
    data = new short(s);
    type = Type::Short;
}

void Compound::Value::set_int32_internal(int32 i)
{
    data = new int32(i);
    type = Type::Int32;
}

void Compound::Value::set_int64_internal(int64 i)
{
    data = new int64(i);
    type = Type::Int64;
}

void Compound::Value::set_bool_internal(bool b)
{
    data = new bool(b);
    type = Type::Bool;
}

void Compound::Value::set_float_internal(float f)
{
    data = new float(f);
    type = Type::Float;
}

void Compound::Value::set_double_internal(double d)
{
    data = new double(d);
    type = Type::Double;
}

void Compound::Value::set_array_internal(const Array& arr)
{
    data = new Array(arr);
    type = Type::Array;
}

void Compound::Value::set_object_internal(const Object& obj)
{
    data = new Object(obj);
    type = Type::Object;
}

void Compound::Value::set_binary_internal(const void* chars, uint64 size)
{
    data = new char[size];
    memcpy(data, chars, size);
    type = Type::BinaryData;
    binary_type_size = size;
}
