#pragma once

#include "BasicTypes.h"
#include "String.h"

class String;

class EXPORT TextReader
{
public:
    TextReader();
    explicit TextReader(const String& string);

    String peek(uint char_num) const; // get n chars without moving pointer
    char peek() const;                // get char without moving a pointer
    String get(uint char_num);                    // get n chars and move pointer
    char get();                                   // get char and move pointer
    void skip(uint char_num);                     // move pointer by n chars
    void skip(const String& chars);               // move pointer while char is present in provided string

    uint get_position() const { return position; }
    uint get_line() const { return line; }
    uint get_column() const { return column; }

    bool is_done() const { return position >= string.length(); }

private:
    String string;
    uint position;
    uint line;
    uint column;
};
