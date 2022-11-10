﻿#include "../include/Color.h"

Color::Color()
    : r(0)
    , g(0)
    , b(0)
    , a(255)
{
}

Color::Color(const Color& rhs)
    : r(rhs.r)
    , g(rhs.g)
    , b(rhs.b)
    , a(rhs.a)
{
}

Color::Color(byte r, byte g, byte b)
    : r(r)
    , g(g)
    , b(b)
    , a(255)
{
}

Color::Color(byte r, byte g, byte b, byte a)
    : r(r)
    , g(g)
    , b(b)
    , a(a)
{
}

Color::Color(int32 number)
    : r((number & 0b11111111000000000000000000000000) >> 24)
    , g((number & 0b00000000111111110000000000000000) >> 16)
    , b((number & 0b00000000000000001111111100000000) >> 8 )
    , a((number & 0b00000000000000000000000011111111)      )
{
}

Vector3 Color::to_vector3() const
{
    return Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
}

Quaternion Color::to_quaternion() const
{
    return Quaternion(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

String Color::to_string() const
{
    return String::format("{ r=%i, g=%i, b=%i, a=%i }", r, g, b, a);
}

int Color::to_number() const
{
    return (int)r << 24 | (int)g << 16 | (int)b << 8 | (int)a;
}

void Color::convert_to(Compound::Object& to) const
{
    to = Compound::Object{
        { "r", r },
        { "g", g },
        { "b", b },
        { "a", a }
    };
}

void Color::convert_from(const Compound::Object& from)
{
    r = from.get_char("r", '\255');
    g = from.get_char("g", '\255');
    b = from.get_char("b", '\255');
    a = from.get_char("a", '\255');
}