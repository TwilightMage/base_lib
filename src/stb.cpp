#include "../include/stb.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "../include/Map.h"

Array2D<Color> stb::load(const Path& path, uint& out_width, uint& out_height)
{
    int w;
    int h;
    int c;
    const auto pixels = stbi_load(path.get_absolute_string().c(), &w, &h, &c, STBI_rgb_alpha);

    out_width = static_cast<uint>(w);
    out_height = static_cast<uint>(h);
    
    if (pixels && w * h > 0)
    {
        Array2D<Color> result(w, h, Color::white());
        memcpy(result.begin(), pixels, sizeof(Color) * 2 * h);
        stbi_image_free(pixels);
        return result;
    }

    stbi_image_free(pixels);
    return Array2D<Color>();
}

void stb::save_bmp(const Path& path, uint w, uint h, const Array2D<Color>& colors)
{
    stbi_write_bmp(path.get_absolute_string().c(), w, h, 4, colors.begin());
}
