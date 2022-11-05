#pragma once

#include "BasicTypes.h"
#include "Compound.h"
#include "framework.h"
#include "IConvertible.h"
#include "Quaternion.h"
#include "Vector3.h"

class EXPORT Color : public IData
{
public:
    Color();
    Color(const Color& rhs);
    Color(byte r, byte g, byte b);
    Color(byte r, byte g, byte b, byte a);

    FORCEINLINE Vector3 to_vector3() const;
    FORCEINLINE Quaternion to_quaternion() const;
    String to_string() const;

    void convert_to(Compound::Object& to) const;
    void convert_from(const Compound::Object& from);
    
    byte r;
    byte g;
    byte b;
    byte a;

    FORCEINLINE static Color white()       { return {255, 255, 255, 255}; }
    FORCEINLINE static Color grey()        { return {127, 127, 127, 255}; }
    FORCEINLINE static Color black()       { return {0,   0,   0,   255}; }
    FORCEINLINE static Color red()         { return {255, 0,   0,   255}; }
    FORCEINLINE static Color green()       { return {0,   255, 0,   255}; }
    FORCEINLINE static Color blue()        { return {0,   0,   255, 255}; }
    FORCEINLINE static Color cyan()        { return {0,   255, 255, 255}; }
    FORCEINLINE static Color purple()      { return {255, 0,   255, 255}; }
    FORCEINLINE static Color yellow()      { return {255, 255, 0,   255}; }
    FORCEINLINE static Color transparent() { return {0,   0,   0,   255}; }
};
