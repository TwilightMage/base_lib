#pragma once

#include <fstream>

#include "Pointers.h"

class Path;

class BinaryReader {
public:
    static Shared<BinaryReader> open(const Path& path);

    void close();

    template<Serializable T>
    inline T read() {
        return StreamUtils::read<T>(stream_);
    }

    template<Data T>
    inline void read_array(T* out_array, uint64 length) {
        StreamUtils::read_array(stream_, out_array, length);
    }

    void move(int offset);
    void reset();

private:
    std::ifstream stream_;
};

class BinaryWriter {
public:
    static Shared<BinaryWriter> open(const Path& path);

    void close();

    template<Serializable T>
    inline void write(const T& val) {
        StreamUtils::write(stream_, val);
    }

    template<Data T>
    inline void write_array(const T* array, uint size) {
        StreamUtils::write_array<T>(stream_, array, size);
    }

private:
    std::ofstream stream_;
};
