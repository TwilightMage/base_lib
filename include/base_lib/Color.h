#pragma once

#include "Quaternion.h"

class EXPORT Color : public IData
{
public:
    Color();
    Color(const Color& rhs);
    Color(byte r, byte g, byte b);
    Color(byte r, byte g, byte b, byte a);
    
    Vector3 to_vector3() const;
    Vector4 to_vector4() const;
    String to_string() const;
    
    void convert_to(Compound::Array& to) const;
    void convert_from(const Compound::Array& from);
    
    byte r;
    byte g;
    byte b;
    byte a;

    static Color white()       { return {255, 255, 255, 255}; }
    static Color grey()        { return {127, 127, 127, 255}; }
    static Color black()       { return {0,   0,   0,   255}; }
    static Color red()         { return {255, 0,   0,   255}; }
    static Color green()       { return {0,   255, 0,   255}; }
    static Color blue()        { return {0,   0,   255, 255}; }
    static Color cyan()        { return {0,   255, 255, 255}; }
    static Color purple()      { return {255, 0,   255, 255}; }
    static Color yellow()      { return {255, 255, 0,   255}; }
    static Color transparent() { return {255, 255, 255, 0  }; }
};
