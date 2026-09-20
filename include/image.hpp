#pragma once

#include <vector>
#include <stdexcept>
#include <format>
#include <type_traits>
#include <cassert>
#include <cstddef>
#include <utility>

#define assertm(exp, msg) assert((void(msg), exp))

struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

static_assert(sizeof(Pixel) == 3, "Pixel must only be three bytes.");
static_assert(std::is_trivially_copyable_v<Pixel>, "Pixel must be trivially copyable.");

class Image {
    private:
        int width;
        int height;
        std::vector<Pixel> pixels;

    public:
        Image(int width, int height, std::vector<Pixel> pixels);

        Image(int width, int height);

        int get_width() const;

        int get_height() const;

        const std::vector<Pixel>& get_pixels() const;

        const Pixel& operator()(int x, int y) const;
            
        Pixel& operator()(int x, int y);
};