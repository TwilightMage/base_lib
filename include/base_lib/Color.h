#pragma once

#include "BasicTypes.h"
#include "Compound.h"
#include "framework.h"
#include "Quaternion.h"
#include "Vector3.h"

class EXPORT Color : public IData
{
public:
    Color();
    Color(const Color& rhs);
    Color(byte r, byte g, byte b);
    Color(byte r, byte g, byte b, byte a);
    
    Vector3 to_vector3() const;
    Quaternion to_quaternion() const;
    String to_string() const;
    
    void convert_to(Compound::Object& to) const;
    void convert_from(const Compound::Object& from);
    
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
