#pragma once

#include "Array2D.h"
#include "Color.h"
#include "Path.h"

class stb
{
public:
    static Array2D<Color> load(const Path& path, uint& out_width, uint& out_height);
    static void save_bmp(const Path& path, uint w, uint h, const Array2D<Color>& colors);
};
