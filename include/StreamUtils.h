#pragma once

#include <istream>
#include <ostream>

#include "ISerializable.h"

class StreamUtils
{
public:
    template<Serializable T>
    static void write(std::ostream& stream, const T& value)
    {
        if constexpr (ExplicitlySerializable<T> || ImplicitlySerializable<T>)
        {
            value.write_to_stream(stream);
        }
        else if constexpr (Data<T>)
        {
            stream.write((char*)&value, sizeof(T));
        }
    }

    template<Serializable T>
    static T read(std::istream& stream)
    {
        T value;

        read(stream, value);

        return value;
    }

    template<Serializable T>
    static void read(std::istream& stream, T& to)
    {
        if constexpr (ExplicitlySerializable<T> || ImplicitlySerializable<T>)
        {
            to.read_from_stream(stream);
        }
        else if constexpr (Data<T>)
        {
            stream.read((char*)&to, sizeof(T));
        }
    }

    template<typename T>
    requires Data<T>
    static void write_array(std::ostream& stream, T* value_arr, uint64 length)
    {
        stream.write((char*)value_arr, sizeof(T) * length);
    }
    static void write_array(std::ostream& stream, void* bytes, uint64 bytes_num)
    {
        stream.write((char*)bytes, sizeof(char) * bytes_num);
    }

    template<typename T>
    requires Data<T>
    static void read_array(std::istream& stream, T*& out_array, uint64 length)
    {
        out_array = new T[length];
        stream.read((char*)out_array, sizeof(T) * length);
    }
    static void read_array(std::istream& stream, void*& out_bytes, uint64 bytes_num)
    {
        out_bytes = new char[bytes_num];
        stream.read((char*)out_bytes, sizeof(char) * bytes_num);
    }

    // string length is determined by the first \0 character
    static void write_c_string(std::ostream& stream, const char* chars)
    {
        const char* ch = chars;
        do
        {
            if (*ch == '\\')
            {
                write(stream, '\\');
            }
            write(stream, *ch);
            ch++;
        }
        while (*ch != '\0');
    }

    // string length is determined by length parameter
    static void write_c_string(std::ostream& stream, const char* chars, uint length)
    {
        for (uint i = 0; i < length; i++)
        {
            if (chars[i] == '\\' || chars[i] == '\0')
            {
                write(stream, '\\');
            }
            write(stream, chars[i]);
        }
        write(stream, '\0');
    }

    static void read_c_string(std::istream& stream, char*& out_c_string, uint& length)
    {
        length = 0;
        const auto stream_pos = stream.tellg();
        bool protect = false;
        bool done = false;
        do
        {
            const char ch = read<char>(stream);
            if (protect)
            {
                length++;
                protect = false;
            }
            else if (ch == '\\')
            {
                protect = true;
            }
            else if (ch == '\0')
            {
                done = true;
                length++;
            }
            else
            {
                length++;
            }
        }
        while (!done);

        out_c_string = new char[length];

        stream.seekg(stream_pos);

        for (uint i = 0, j = 0; i < length; i++)
        {
            const char ch = read<char>(stream);
            if (protect)
            {
                out_c_string[j++] = ch;
                protect = false;
            }
            else if (ch == '\\')
            {
                protect = true;
            }
            else
            {
                out_c_string[j++] = ch;
            }
        }
    }
};
