#pragma once

#include "IConvertible.h"
#include "List.h"
#include "Map.h"
#include "Name.h"
#include "Pointers.h"
#include "SimpleMap.h"
#include "String.h"
#include "TextReader.h"

class Name;

namespace Compound
{    
    class EXPORT Value;
    class EXPORT Array;
    class EXPORT Object;

    enum class Type : int
    {
        Null,
        String,
        Char,
        Short,
        Int32,
        Int64,
        Bool,
        Float,
        Double,
        Array,
        Object,
        BinaryData
    };

    const static int numeric_type_mask = 1 << (int)Type::Char | 1 << (int)Type::Short | 1 << (int)Type::Int32 | 1 << (int)Type::Int64 | 1 << (int)Type::Bool | 1 << (int)Type::Float | 1 << (int)Type::Double;

    template<typename T>
    concept CompoundBinaryData = Data<T> && !std::is_arithmetic<T>::value && !std::is_pointer<T>::value;

    template<typename T>
    concept CompoundValueType =
        std::same_as<T, nullptr_t> ||
        std::same_as<T, String> ||
        std::same_as<T, char> ||
        std::same_as<T, short> ||
        std::same_as<T, int32> ||
        std::same_as<T, int64> ||
        std::same_as<T, bool> ||
        std::same_as<T, float> ||
        std::same_as<T, double> ||
        std::same_as<T, Array> ||
        std::same_as<T, Object>;

    const static String type_names[12] = {
        "Null",
        "String",
        "Char",
        "Short",
        "Int32",
        "Int64",
        "Bool",
        "Float",
        "Double",
        "Array",
        "Object",
        "Binary Data"
    };

    class EXPORT CompoundException : public std::exception{};

    class EXPORT TypeMismatchException : public CompoundException
    {
    public:
        TypeMismatchException(Type desired_type, Type actual_type, uint64 desired_binary_size = 0, uint64 actual_binary_size = 0);
        
        char const* what() const override;

        const Type desired_type;
        const Type actual_type;
        const uint64 desired_binary_size;
        const uint64 actual_binary_size;
    };

    class EXPORT IDiff
    {
    public:
        virtual bool is_empty() const = 0;
        virtual Type get_target_type() const = 0;
    };

    class ArrayDiff : public IDiff
    {
    public:
        FORCEINLINE bool is_empty() const override { return set.size() == 0 && merge.size() == 0; }
        FORCEINLINE Type get_target_type() const override { return Type::Array; }
        
        uint newLength;
        SimpleMap<uint, Value> set;
        SimpleMap<uint, Shared<IDiff>> merge;
    };

    class ObjectDiff : public IDiff
    {
    public:
        FORCEINLINE bool is_empty() const override { return remove.length() == 0 && set.size() == 0 && merge.size() == 0; }
        FORCEINLINE Type get_target_type() const override { return Type::Object; }
        
        List<String> remove;
        SimpleMap<String, Value> set;
        SimpleMap<String, Shared<IDiff>> merge;
    };

    class EXPORT Array : public List<Value>
    {
    public:
        Array();
        Array(const Array& rhs);
        Array(const List<Value>& rhs);
        Array(const std::initializer_list<Value>& rhs);

        Array& operator=(const Array& rhs);
        Array& operator=(const List<Value>& rhs);

        bool operator==(const Array& rhs) const;
        bool operator==(const List<Value>& rhs) const;
        
        Shared<ArrayDiff> build_diff_against(const Array& base) const;
        void apply_diff(const Shared<ArrayDiff>& diff);
    };

    class EXPORT Object : public SimpleMap<String, Value>
    {
    public:
        Object();
        Object(const Object& rhs);
        Object(const SimpleMap<String, Value>& rhs);
        Object(const std::initializer_list<Pair<String, Value>>& rhs);

        Object& operator=(const Object& rhs);
        Object& operator=(const SimpleMap<String, Value>& rhs);

        bool operator==(const Object& rhs) const;
        bool operator==(const SimpleMap<String, Value>& rhs) const;

        bool contains_of_type(const String& key, Type type) const;
        bool contains_non_null(const String& key) const;
        bool contains_number(const String& key) const;

        Shared<ObjectDiff> build_diff_against(const Object& base) const;
        void apply_diff(const Shared<ObjectDiff>& diff);

#pragma region String
        String get_string(const String& key, const String& default_value = "") const;
        Object& chain_string(const String& key, const String& value);
        Object& chain(const String& key, const String& value);
#pragma endregion

#pragma region Char
        char get_char(const String& key, char default_value = 0) const;
        Object& chain_char(const String& key, char value);
        Object& chain(const String& key, char value);
#pragma endregion

#pragma region Short
        short get_short(const String& key, short default_value = 0) const;
        Object& chain_short(const String& key, short value);
        Object& chain(const String& key, short value);
#pragma endregion

#pragma region Int32
        int32 get_int32(const String& key, int32 default_value = 0) const;
        Object& chain_int32(const String& key, int32 value);
        Object& chain(const String& key, int32 value);
#pragma endregion

#pragma region Int64
        int64 get_int64(const String& key, int64 default_value = 0) const;
        Object& chain_int64(const String& key, int64 value);
        Object& chain(const String& key, int64 value);
#pragma endregion

#pragma region Bool
        bool get_bool(const String& key, bool default_value = false) const;
        Object& chain_bool(const String& key, bool value);
        Object& chain(const String& key, bool value);
#pragma endregion

#pragma region Float
        float get_float(const String& key, float default_value = 0) const;
        Object& chain_float(const String& key, float value);
        Object& chain(const String& key, float value);
#pragma endregion

#pragma region Double
        double get_double(const String& key, double default_value = 0) const;
        Object& chain_double(const String& key, double value);
        Object& chain(const String& key, double value);
#pragma endregion

#pragma region Array
        Array get_array(const String& key, const Array& default_value = Array()) const;
        Object& chain_array(const String& key, const Array& value);
        Object& chain(const String& key, const Array& value);
#pragma endregion

#pragma region Object
        Object get_object(const String& key, const Object& default_value = Object()) const;
        Object& chain_object(const String& key, const Object& value);
        Object& chain(const String& key, const Object& value);
#pragma endregion

#pragma region Binary data
        template<CompoundBinaryData T>
        T get_binary_data(const String& key, const T& default_value = T()) const;

        template<CompoundBinaryData T>
        Object& chain_binary_data(const String& key, const T& value);

        template<CompoundBinaryData T>
        Object& chain(const String& key, const T& value);
#pragma endregion

        
        template<Convertible<Object> T>
        void put_object(const String& key, const T& item);

        template<Convertible<Object> T>
        T extract_object(const String& key);

        template<Convertible<Array> T>
        void put_array(const String& key, const T& item);

        template<Convertible<Array> T>
        T extract_array(const String& key);
    };

    class EXPORT ParseException : CompoundException
    {
    public:
        explicit ParseException(uint line, uint column, const String& bad_token = "");

        char const* what() const override;

    private:
        uint line;
        uint column;
        String bad_token;
    };

    namespace Convert
    {
        static bool try_load_value_from_file(const Path& path, Value& result, Name format = Name());
        static bool try_load_object_from_file(const Path& path, Object& result, Name format = Name());
        static bool try_load_array_from_file(const Path& path, Array& result, Name format = Name());

        static void save_to_file(const Path& path, bool pretty, const Value& value, Name format = Name());
        
        class EXPORT IParser
        {
        public:
            virtual Value parse_value(const String& str) const = 0;
            Object parse_object(const String& str) const;
            Array parse_array(const String& str) const;

            bool try_parse_value(const String& str, Value& out_value) const;
            bool try_parse_object(const String& str, Object& out_value) const;
            bool try_parse_array(const String& str, Array& out_value) const;
        };

        class EXPORT IFormatter
        {
        public:
            virtual String format_value(const Value& val) const = 0;
            virtual bool write_to(std::ostream& stream, const Value& val) const = 0;

            bool pretty = false;
        };
        
        class EXPORT JSON : public IParser, public IFormatter
        {
        public:
            String format_value(const Value& val) const override;
            bool write_to(std::ostream& stream, const Value& val) const override;
            Value parse_value(const String& str) const override;

        private:
            void write(std::ostream& stream, const Value& val, uint depth) const;
            FORCEINLINE String tab_offset(uint depth) const { return pretty ? String(' ', depth * 2) : ""; }
            FORCEINLINE const String new_line() const { return pretty ? "\n" : ""; }

            Value read_value() const;
            void read_null() const;
            String read_string() const;
            Value read_number() const;
            bool read_bool() const;
            Object read_object() const;
            Array read_array() const;

            mutable TextReader reader;
            inline const static String space_chars = " \n\r\t";
        };

        class EXPORT XML; // TODO: Implement Compound XML converter
        
        class EXPORT YAML : public IFormatter
        {
        public:
            String format_value(const Value& val) const override;
            bool write_to(std::ostream& stream, const Value& val) const override;

        private:
            void write(std::ostream& stream, const Value& val, uint depth) const;
        };

        static Map<Name, Shared<IParser>(*)()> parsers = {
            {"json", []() -> Shared<IParser>{ return MakeShared<JSON>(); }}
        };
        
        static Map<Name, Shared<IFormatter>(*)()> formatters = {
            {"json", []() -> Shared<IFormatter>{ return MakeShared<JSON>(); }},
            {"yaml", []() -> Shared<IFormatter>{ return MakeShared<YAML>(); }}
        };
    }
    
    class EXPORT Value : public ISerializable, public IConvertible<String>, public IConvertible<char>, public IConvertible<short>, public IConvertible<int32>, public IConvertible<int64>, public IConvertible<bool>, public IConvertible<float>, public IConvertible<double>, public IConvertible<Array>, public IConvertible<Object>
    {
    public:
        void write_to_stream(std::ostream& stream) const override;
        void read_from_stream(std::istream& stream) override;
        
        Value(const Value& rhs);

        Value(Type type);

#pragma region Null
        Value();
        void set_null();
        void set(nullptr_t null);
        Value& operator=(nullptr_t null);
        bool operator==(nullptr_t null) const;
#pragma endregion 

#pragma region String
        Value(const String& str);
        void set_string(const String& str);
        void set(const String& str);
        Value& operator=(const String& str);
        bool operator==(const String& str) const;
        explicit operator String&();
        explicit operator const String&() const;
        String get_string(const String& default_value = "") const;

        void convert_to(String& to) const override;
        void convert_from(const String& from) override;
#pragma endregion 

#pragma region Char
        Value(char ch);
        void set_char(char ch);
        void set(char ch);
        Value& operator=(char ch);
        bool operator==(char ch) const;
        explicit operator char&();
        explicit operator const char&() const;
        char get_char(char default_value = 0) const;

        void convert_to(char& to) const override;
        void convert_from(const char& from) override;
#pragma endregion

#pragma region Short
        Value(short s);
        void set_short(short s);
        void set(short s);
        Value& operator=(short s);
        bool operator==(short s) const;
        explicit operator short&();
        explicit operator const short&() const;
        short get_short(short default_value = 0) const;

        void convert_to(short& to) const override;
        void convert_from(const short& from) override;
#pragma endregion

#pragma region Int32
        Value(int32 i);
        void set_int32(int32 i);
        void set(int32 i);
        Value& operator=(int32 i);
        bool operator==(int32 i) const;
        explicit operator int32&();
        explicit operator const int32&() const;
        int32 get_int32(int32 default_value = 0) const;

        void convert_to(int32& to) const override;
        void convert_from(const int32& from) override;
#pragma endregion

#pragma region Int64
        Value(int64 i);
        void set_int64(int64 i);
        void set(int64 i);
        Value& operator=(int64 i);
        bool operator==(int64 i) const;
        explicit operator int64&();
        explicit operator const int64&() const;
        int64 get_int64(int64 default_value = 0) const;

        void convert_to(int64& to) const override;
        void convert_from(const int64& from) override;
#pragma endregion
        
#pragma region Bool
        Value(bool b);
        void set_bool(bool b);
        void set(bool b);
        Value& operator=(bool b);
        bool operator==(bool b) const;
        explicit operator bool&();
        explicit operator const bool&() const;
        bool get_bool(bool default_value = false) const;

        void convert_to(bool& to) const override;
        void convert_from(const bool& from) override;
#pragma endregion 

#pragma region Float
        Value(float f);
        void set_float(float f);
        void set(float f);
        Value& operator=(float f);
        bool operator==(float f) const;
        explicit operator float&();
        explicit operator const float&() const;
        float get_float(float default_value = 0) const;

        void convert_to(float& to) const override;
        void convert_from(const float& from) override;
#pragma endregion

#pragma region Double
        Value(double d);
        void set_double(double d);
        void set(double d);
        Value& operator=(double d);
        bool operator==(double d) const;
        explicit operator double&();
        explicit operator const double&() const;
        double get_double(double default_value = 0) const;

        void convert_to(double& to) const override;
        void convert_from(const double& from) override;
#pragma endregion 

#pragma region Array
        Value(const Array& arr);
        void set_array(const Array& arr);
        void set(const Array& arr);
        Value& operator=(const Array& arr);
        bool operator==(const Array& arr) const;
        Array* access_array() const;
        explicit operator Array&();
        explicit operator const Array&() const;
        Array get_array(const Array& default_value = Array()) const;

        void convert_to(Array& to) const override;
        void convert_from(const Array& from) override;
#pragma endregion 

#pragma region Object
        Value(const Object& obj);
        void set_object(const Object& obj);
        void set(const Object& obj);
        Value& operator=(const Object& obj);
        bool operator==(const Object& obj) const;
        Object* access_object() const;
        explicit operator Object&();
        explicit operator const Object&() const;
        Object get_object(const Object& default_value = Object()) const;

        void convert_to(Object& to) const override;
        void convert_from(const Object& from) override;
#pragma endregion 

#pragma region Binary data
        template<CompoundBinaryData T>
        Value(const T& bin) : Value(&bin, sizeof(T)) {}
        Value(const void* chars, uint64 size);

        template<CompoundBinaryData T>
        void set_binary_data(const T& bin) { set_binary_data(&bin, sizeof(bin)); }
        void set_binary_data(const void* chars, uint64 size);

        template<CompoundBinaryData T>
        void set(const T& bin) { set(&bin, sizeof(bin)); }
        void set(const void*& chars, uint64 size);

        template<CompoundBinaryData T>
        Value& operator=(const T& bin) { set_binary_data(bin); return *this; }

        template<CompoundBinaryData T>
        bool operator==(const T& bin) { return type == Type::BinaryData && sizeof(T) == binary_type_size && memcmp(data, &bin, sizeof(T)) == 0; }

        template<CompoundBinaryData T> T* access_binary_data() const
        {
            if (type == Type::BinaryData && sizeof(T) == binary_type_size)
            {
                return (T*)data;
            }
            return nullptr;
        }

        template<CompoundBinaryData T>
        explicit operator T&()
        {
            if (type != Type::BinaryData || sizeof(T) != binary_type_size) throw TypeMismatchException(Type::BinaryData, type, sizeof(T), binary_type_size);
            return *(T*)data;
        }

        template<CompoundBinaryData T>
        explicit operator const T&() const
        {
            if (type != Type::BinaryData || sizeof(T) != binary_type_size) throw TypeMismatchException(Type::BinaryData, type, sizeof(T), binary_type_size);
            return *(T*)data;
        }

        template<CompoundBinaryData T>
        T get_binary_data(const String& default_value = T())
        {
            if (type != Type::BinaryData || sizeof(T) != binary_type_size) return default_value;
            return *(T*)data;
        }

        void read_binary_data(void*& bytes, uint64& size) const;
#pragma endregion

        void set_type(Type new_type);

        Value& operator[](const String& key);
        Value& operator[](uint index);

        Value& operator=(const Value& rhs);

        bool operator==(const Value& rhs) const;

        bool is_similar_to(const Value& rhs) const;
        bool is_same_to(const Value& rhs) const;

        String to_string() const;

        FORCEINLINE const void* get_data_block() const { return data; }
        FORCEINLINE Type get_type() const { return type; }
        FORCEINLINE bool is_number() const { return 1 << (int)type & numeric_type_mask; }
        FORCEINLINE uint64 get_binary_type_size() const { return binary_type_size; }

    private:
        void create_data();
        void delete_data();
        void copy_from(const Value& rhs);

        void set_string_internal(const String& str);
        void set_char_internal(char b);
        void set_short_internal(short s);
        void set_int32_internal(int32 i);
        void set_int64_internal(int64 i);
        void set_bool_internal(bool b);
        void set_float_internal(float f);
        void set_double_internal(double d);
        void set_array_internal(const Array& arr);
        void set_object_internal(const Object& obj);

        template<CompoundBinaryData T>
        void set_binary_internal(const T& bin)
        {
            set_binary_internal(&bin, sizeof(T));
        }
        void set_binary_internal(const void* chars, uint64 size);

        template<typename T>
        FORCEINLINE T& get_data()
        {
            return *(T*)data;
        }

        template<typename T>
        FORCEINLINE const T& get_data() const
        {
            return *(T*)data;
        }

        template<typename T>
        requires std::is_arithmetic<T>::value
        FORCEINLINE T get_data_numeric() const
        {
            switch (type)
            {
                case Type::Char:
                    return (T) get_data<char>();
                case Type::Short:
                    return (T) get_data<short>();
                case Type::Int32:
                    return (T) get_data<int32>();
                case Type::Int64:
                    return (T) get_data<int64>();
                case Type::Bool:
                    return (T) get_data<bool>();
                case Type::Float:
                    return (T) get_data<float>();
                case Type::Double:
                    return (T) get_data<double>();
                default:
                    return 0;
            }
        }

        void* data;
        Type type;
        uint64 binary_type_size;
    };
    
    template<CompoundBinaryData T>
    T Object::get_binary_data(const String& key, const T& default_value) const
    {
        if (const Value* value = find(key))
        {
            return value->get_binary_data<T>();
        }
                        
        return default_value;
    }

    template<CompoundBinaryData T>
    Object& Object::chain_binary_data(const String& key, const T& value)
    {
        operator[](key) = value;
            
        return *this;
    }

    template<CompoundBinaryData T>
    Object& Object::chain(const String& key, const T& value)
    {
        operator[](key) = value;
            
        return *this;
    }

    template <Convertible<Object> T>
    void Object::put_object(const String& key, const T& item)
    {
        Object converted;
        convert(item, converted);
        operator[](key) = converted;
    }

    template <Convertible<Object> T>
    T Object::extract_object(const String& key)
    {
        T object;
        convert(get_object(key), object);
        return object;
    }

    template <Convertible<Array> T>
    void Object::put_array(const String& key, const T& item)
    {
        Array converted;
        convert(item, converted);
        operator[](key) = converted;
    }

    template <Convertible<Array> T>
    T Object::extract_array(const String& key)
    {
        T array;
        convert(get_array(key), array);
        return array;
    }
}
