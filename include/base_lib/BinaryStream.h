#pragma once

#include <fstream>

#include "Path.h"

class BinaryReader {
public:
    static Shared<BinaryReader> open(const Path& path) {
        Shared<BinaryReader> result = MakeShared<BinaryReader>();
        result->stream_ = std::ifstream(path.get_absolute_string().c(), std::ios::in | std::ios::binary);

        return result;
    }

    void close() {
        stream_.close();
    }

    template<Serializable T>
    T read() {
        return StreamUtils::read<T>(stream_);
    }

    void move(int offset) {
        StreamUtils::move(stream_, offset);
    }

    void reset() {
        StreamUtils::reset(stream_);
    }

    template<Data T>
    void read_array(T* out_array, uint64 length)
    {
        StreamUtils::read_array(stream_, out_array, length);
    }

private:
    std::ifstream stream_;
};

template<>
String BinaryReader::read<String>() {
    const uint len = read<uint>();
    char* str = new char[len];
    stream_.read(str, len);
    return String(str, len);
}

class BinaryWriter {
public:
    static Shared<BinaryWriter> open(const Path& path) {
        Shared<BinaryWriter> result = MakeShared<BinaryWriter>();
        result->stream_ = std::ofstream(path.get_absolute_string().c(), std::ios::out | std::ios::binary);

        return result;
    }

    void close() {
        stream_.close();
    }

    template<Serializable T>
    void write(const T& val) {
        StreamUtils::write(stream_, val);
    }

    template<Data T>
    void write_array(const T* array, uint size) {
        StreamUtils::write_array<T>(stream_, array, size);
    }

private:
    std::ofstream stream_;
};

template<>
void BinaryWriter::write<String>(const String& val) {
    write(val.length());
    stream_.write(val.c(), val.length());
}
