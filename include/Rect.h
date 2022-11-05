#pragma once

#include "Compound.h"
#include "IConvertible.h"
#include "Pointers.h"
#include "Vector2.h"

class EXPORT Rect : public IData
{
public:
    Rect();
    Rect(int x, int y, int w, int h);

    bool intersects(const Rect& rhs) const;
    Shared<Rect> intersection(const Rect& rhs) const;
    FORCEINLINE Vector2 get_position() const;
    FORCEINLINE Vector2 get_size() const;
    FORCEINLINE bool contains(int p_x, int p_y) const;
    FORCEINLINE bool contains(const Vector2& point) const;

    FORCEINLINE int left() const;
    FORCEINLINE int right() const;
    FORCEINLINE int top() const;
    FORCEINLINE int bottom() const;

    void convert_to(Compound::Object& to) const;
    void convert_from(const Compound::Object& from);

    int x;
    int y;
    int w;
    int h;
};
