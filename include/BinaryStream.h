#pragma once

#include <fstream>

#include "Path.h"

class BinaryReader
{
public:
    static Shared<BinaryReader> open(const Path& path)
    {
        Shared<BinaryReader> result = MakeShared<BinaryReader>();
        result->stream_ = std::ifstream(path.get_absolute_string().c(), std::ios::in | std::ios::binary);

        return result;
    }

    void close()
    {
        stream_.close();
    }
    
    template<typename T>
    T read()
    {
        T result;
        stream_.read((char*)&result, sizeof(T));
        return result;
    }
    
private:
    std::ifstream stream_;
};

template<>
String BinaryReader::read<String>()
{
    const uint len = read<uint>();
    char* str = new char[len];
    stream_.read(str, len);
    return String(str, len);
}

class BinaryWriter
{
public:
    static Shared<BinaryWriter> open(const Path& path)
    {
        Shared<BinaryWriter> result = MakeShared<BinaryWriter>();
        result->stream_ = std::ofstream(path.get_absolute_string().c(), std::ios::out | std::ios::binary);

        return result;
    }

    void close()
    {
        stream_.close();
    }
    
    template<typename T>
    void write(const T& val)
    {
        stream_.write((char*)&val, sizeof(T));
    }
    
private:
    std::ofstream stream_;
};

template<>
void BinaryWriter::write<String>(const String& val)
{
    write(val.length());
    stream_.write(val.c(), val.length());
}
