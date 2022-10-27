#pragma once
#include <cstdint>
#include <type_traits>
#include <string>
#include <map>
#include <any>

namespace jimg
{
    template<typename T>
    struct color_io_rgba
    {
    public:
        T* data;
        color_io_rgba(T* data) : data(data), r(data[0]), g(data[1]), b(data[2]), a(data[3]) { }
    public:
        T& r, & g, & b, & a;
    };

    template<typename T>
    struct image_io
    {
        T* data;
        int32_t channel_num;
        int32_t weight;
        int32_t height;
        using color = color_io_rgba<T>;
        image_io(T* buffer, int32_t channel_num, int32_t weight, int32_t height)
            : data(buffer), channel_num(channel_num), weight(weight), height(height)
        {}

        T* at(int x, int y) { return data + (y * weight * channel_num + x * channel_num); }
        color at_rgba(int x, int y) { return color{ at(x, y) }; }
        size_t size() const { return channel_num * weight * height; }

        //lambda : [](int x, int y, color_io_rgba<T>::color&& c)
        template<typename FUNC>
        void foreach(FUNC&& func)
        {
            for (int32_t y = 0; y < height; y++)
                for (int32_t x = 0; x < weight; x++)
                    func(x, y, color(at(x, y)));
        }

        void draw(image_io<T> painter,
            int me_posx = 0, int me_posy = 0,
            int painter_offx = 0, int painter_offy = 0,
            int painter_width = -1, int painter_height = -1)
        {
            if (me_posx > weight || me_posy > height) return;

            painter_width = painter_width != -1 ? painter_width : painter.height;
            painter_height = painter_height != -1 ? painter_height : painter.height;

            int size_width = painter.weight - painter_offx;
            int size_height = painter.height - painter_offy;
            {
                for (size_t y = 0; y < size_height; y++)
                {
                    for (size_t x = 0; x < size_width; x++)
                    {
                        if (me_posx + x >= weight || me_posy + y >= height) continue;
                        ::memcpy(at(me_posx + x, me_posy + y), painter.at(x + painter_offx, y + painter_offy), sizeof(T) * channel_num);
                    }
                }
            }

        }
    };
    using image_io_8b = image_io<uint8_t>;
    using image_io_linearf = image_io<float>;


    inline void bit8_to_linear(uint8_t* data, size_t length, float* out_data)
    {
        for (size_t i = 0; i < length; i++)
            out_data[i] = data[i] / 255.f;
    }
    inline void linear_to_bit8(float* data, size_t length, uint8_t* out_data)
    {
        for (size_t i = 0; i < length; i++)
            out_data[i] = (uint8_t)(data[i] * 255);
    }

    inline double lerp(double a, double b, double t) { return a + (b - a) * t; }
    inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

    inline double inv_lerp(double a, double b, double v) { return (v - a) / (b - a); }
    inline float inv_lerp(float a, float b, float v) { return (v - a) / (b - a); }

    template<typename T>
    struct buffer
    {
        int32_t size_;
        void(*deleter)(T*);
        T* data;

        buffer(int32_t size) : size_(size), deleter(nullptr)
        {
            data = new T[size];
        }
        buffer(T* managed_data, int32_t size_, void(*deleter)(T*))
            : data(managed_data), size_(size_), deleter(deleter)
        {
        }

        buffer(const buffer& r)
        {
            size_ = r.size_;
            data = new T[size_];
            deleter = nullptr;
            ::memcpy(data, r.data, size_);
        }

        buffer(buffer&& r) noexcept
        {
            data = r.data;
            size_ = r.size_;
            deleter = r.deleter;
            r.data = nullptr;
        }
        ~buffer()
        {
            if (deleter && data)
                deleter(data);
            else if (data)
                delete[] data;
        }
        int32_t size() const { return size_; }
        int32_t byte_size() const { return size_ * sizeof(T); }
        bool is_managed() const { return deleter != nullptr; }
        operator T* () { return this->data; }
    };

    struct image
    {
        using attr_t = std::map<std::string, std::any>;

        int32_t channel_num;
        int32_t weight;
        int32_t height;
        buffer<float> buf;
        std::map<std::string, std::any> attr;

        image(int32_t channel_num, int32_t weight, int32_t height, const attr_t& attr = {})
            :channel_num(channel_num), weight(weight), height(height),
            buf(channel_num* weight* height), attr(attr)
        {}

        image(float* managed_data, int32_t channel_num, int32_t weight, int32_t height, void(*deleter)(float*), const attr_t& attr = {})
            : channel_num(channel_num), weight(weight), height(height),
            buf(managed_data, channel_num* weight* height, deleter), attr(attr)
        {
        }

        image(const image& img) = default;

        image(image&& r) noexcept = default;

        image_io<float> operate() { return image_io<float>(buf.data, channel_num, weight, height); }

        template<typename T>
        void set_attr(const std::string& name, const T& value)
        {
            attr[name] = value;
        }

        template<typename T>
        bool get_attr(const std::string& name, T* out)
        {
            auto it = attr.find(name);
            if (it == attr.end())
            {
                return false;
            }
            if (it->second.type() != typeid(T))
            {
                return false;
            }
            *out = std::any_cast<T>(it->second);
            return true;
        }
    };

    image load_img_from_file(const char* name);
    void save_img(const char* name, const image_io_linearf& img);
    image create_img(int32_t channel_num, int32_t weight, int32_t height);




    struct seqimg_name_sort
    {
        int last_get_num_pos(const std::string& a, int* out_pos)
        {
            std::string number;
            bool enabled = false;
            for (int i = a.length() - 1; i >= 0; i--)
            {
                if (a[i] >= '0' && a[i] <= '9')
                {
                    enabled = true;
                    number.insert(number.begin(), a[i]);
                }
                else if (enabled)
                {
                    *out_pos = i;
                    break;
                }
            }
            if (number.length() == 0) return -1;
            return atoi(number.c_str());
        }
        bool operator()(const std::string& a, const std::string& b)
        {
            int aindex = 0, bindex = 0;
            int anum = last_get_num_pos(a, &aindex);
            int bnum = last_get_num_pos(b, &bindex);
            if (aindex == bindex && memcmp(a.c_str(), b.c_str(), aindex) == 0)
            {
                return anum < bnum;
            }
            return a < b;
        }
    };
}