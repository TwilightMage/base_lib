#pragma once

#include "List.h"
#include "String.h"

class Base64
{
public:
    static String encode(const List<byte>& bytes);
    static List<byte> decode(const String& string);
};
