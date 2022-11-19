#pragma once

#include "Pointers.h"
#include "SimpleMap.h"

class EXPORT Regex
{
public:
    Regex(const String& expression);

    bool check(const String& subject) const;
    Shared<List<String>> capture(const String& subject) const;

private:
    const char* expression_;
};
