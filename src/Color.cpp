#include "base_lib/Color.h"

#include "base_lib/Compound.h"

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

Vector3 Color::to_vector3() const
{
    return Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
}

Quaternion Color::to_vector4() const
{
    return Quaternion(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

String Color::to_string() const
{
    return String::format("{ r=%i, g=%i, b=%i, a=%i }", r, g, b, a);
}

void Color::convert_to(Compound::Array& to) const
{
    to = Compound::Array{r, g, b, a};
}

void Color::convert_from(const Compound::Array& from)
{
    r = from.at_or_default(0, '\0').get_char();
    g = from.at_or_default(1, '\0').get_char();
    b = from.at_or_default(2, '\0').get_char();
    a = from.at_or_default(3, '\255').get_char();
}
