#pragma once
#include "BasicTypes.h"
#include "String.h"

class EXPORT Utils
{
public:
    static void init_base_lib(const String& executablePath);
    static List<String> get_callstack(uint offset = 0);
    static String get_callstack_string(uint offset = 0);
    static void print_callstack(String category, uint offset = 0);
    static bool ask_yes_no(const String& message);
};
