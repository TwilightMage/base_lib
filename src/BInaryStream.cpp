#include "base_lib/BinaryStream.h"

#include "base_lib/Path.h"

Shared<BinaryReader> BinaryReader::open(const Path& path) {
    Shared<BinaryReader> result = MakeShared<BinaryReader>();
    result->stream_ = std::ifstream(path.get_absolute_string().c(), std::ios::in | std::ios::binary);

    return result;
}

void BinaryReader::close() {
    stream_.close();
}

void BinaryReader::move(int offset) {
    StreamUtils::move(stream_, offset);
}

void BinaryReader::reset() {
    StreamUtils::reset(stream_);
}

Shared<BinaryWriter> BinaryWriter::open(const Path& path) {
    Shared<BinaryWriter> result = MakeShared<BinaryWriter>();
    result->stream_ = std::ofstream(path.get_absolute_string().c(), std::ios::out | std::ios::binary);

    return result;
}

void BinaryWriter::close() {
    stream_.close();
}
