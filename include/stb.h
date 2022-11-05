#pragma once

#include "Color.h"
#include "List.h"
#include "Path.h"

class stb
{
public:
    static List<Color> load(const Path& path, uint& out_width, uint& out_height);
    static void write_bmp(const Path& path, uint w, uint h, const List<Color>& colors);
};
