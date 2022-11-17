#include "base_lib/Margins.h"

Margins::Margins()
    : left(0)
    , right(0)
    , top(0)
    , bottom(0)
{
}

Margins::Margins(int left, int right, int top, int bottom)
    : left(left)
    , right(right)
    , top(top)
    , bottom(bottom)
{
}

void Margins::convert_to(Compound::Object& to) const
{
    to = Compound::Object{
        { "left", left },
        { "right", right },
        { "top", top },
        { "bottom", bottom }
    };
}

void Margins::convert_from(const Compound::Object& from)
{
    left = from.get_int32("left", 0);
    right = from.get_int32("right", 0);
    top = from.get_int32("top", 0);
    bottom = from.get_int32("bottom", 0);
}
