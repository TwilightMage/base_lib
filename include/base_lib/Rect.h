#pragma once

#include "Pointers.h"
#include "Vector2.h"

class EXPORT Rect : public IData
{
public:
    Rect();
    Rect(int x, int y, int w, int h);

    bool intersects(const Rect& rhs) const;
    Shared<Rect> intersection(const Rect& rhs) const;
    Vector2 get_position() const;
    Vector2 get_size() const;
    bool contains(int p_x, int p_y) const;
    bool contains(const Vector2& point) const;

    int left() const;
    int right() const;
    int top() const;
    int bottom() const;

    void convert_to(Compound::Array& to) const;
    void convert_from(const Compound::Array& from);

    int x;
    int y;
    int w;
    int h;
};
