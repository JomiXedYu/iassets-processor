#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "jimg.h"
#include <filesystem>

namespace jimg
{
    image load_img_from_file(const char* name)
    {
        int w, h, n;

        uint8_t* data = stbi_load(name, &w, &h, &n, 0);

        float* linear = new float[w * h * n];

        bit8_to_linear(data, size_t(w) * h * n, linear);

        stbi_image_free(data);

        return image(linear, n, w, h, [](float* p) { delete[] p; });
    }

    void save_img(const char* name, const image_io_linearf& img)
    {
        using namespace std::filesystem;
        auto ext = path(name).extension();
        if (ext == ".jpg")
        {

            buffer<uint8_t> buf = buffer<uint8_t>(img.size());
            linear_to_bit8(img.data, img.size(), buf);
            stbi_write_jpg(name, img.weight, img.height, img.channel_num, buf, 0);

        }
        else if (ext == ".png")
        {
            buffer<uint8_t> buf = buffer<uint8_t>(img.size());
            linear_to_bit8(img.data, img.size(), buf);
            stbi_write_png(name, img.weight, img.height, img.channel_num, buf, 0);

        }
        else if (ext == ".tga")
        {
            buffer<uint8_t> buf = buffer<uint8_t>(img.size());
            linear_to_bit8(img.data, img.size(), buf);
            stbi_write_tga(name, img.weight, img.height, img.channel_num, buf);
        }
        else if (ext == ".bmp")
        {
            buffer<uint8_t> buf = buffer<uint8_t>(img.size());
            linear_to_bit8(img.data, img.size(), buf);
            stbi_write_bmp(name, img.weight, img.height, img.channel_num, buf);
        }

    }


    image create_img(int32_t channel_num, int32_t weight, int32_t height)
    {
        return image(channel_num, weight, height);
    }
}