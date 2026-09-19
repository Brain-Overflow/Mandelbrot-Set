#include <fstream>
#include <vector>
#include <stdexcept>
#include <format>
#include <type_traits>
#include <cassert>
#include <cstddef>
#include <utility>
#include <iostream>

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
        Image(int width, int height, std::vector<Pixel> pixels) : width(width), height(height), pixels(std::move(pixels)) {
            if (this->width <= 0) {
                throw std::invalid_argument(std::format("Width must be a positive integer. Width is {}.", this->width));
            }
            if (this->height <= 0) {
                throw std::invalid_argument(std::format("Height must be a positive integer. Height is {}.", this->height));
            }

            std::size_t expected_size = static_cast<std::size_t>(this->width) * static_cast<std::size_t>(this->height);

            if (this->pixels.size() != expected_size) {
                throw std::invalid_argument(std::format("Buffer size must match width multiplied by height. Width is {}. Height is {}. Buffer size must be {}. Buffer size is {}.", this->width, this->height, expected_size, this->pixels.size()));
            }
        }

        Image(int width, int height) : Image(width, height, std::vector<Pixel>(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), Pixel{0, 0, 0})) {

        }

        int get_width() const {
            return width;
        }

        int get_height() const {
            return height;
        }

        const std::vector<Pixel>& get_pixels() const {
            return pixels;
        }

        const Pixel& operator()(int x, int y) const {
            assertm(x >= 0 && x < width, "X coordinate is out of range.");
            assertm(y >= 0 && y < height, "Y coordinate is out of range.");

            return pixels[static_cast<std::size_t>(y) * width + static_cast<std::size_t>(x)];
        }
            
        Pixel& operator()(int x, int y) {
            return const_cast<Pixel&>(std::as_const(*this)(x, y));
        }
};

std::ostream& write_ppm(std::ostream& out, const Image& image) {
    out << "P6" << '\n' << image.get_width() << ' ' << image.get_height() << '\n' << "255" << '\n';

    const auto& pixels = image.get_pixels();

    out.write(reinterpret_cast<const char*>(pixels.data()), static_cast<std::streamsize>(pixels.size() * sizeof(Pixel)));

    return out;
}

int main() {
    // Generate Checkerboard Image
    int image_size = 512;

    Image image(image_size, image_size);

    int cell_size = 64;

    for (int y = 0; y < image_size; ++y) {
        for (int x = 0; x < image_size; ++x) {
            int cell_x = x / cell_size;
            int cell_y = y / cell_size;

            bool is_light   = ((cell_x + cell_y) % 2) == 0;
            bool is_first_cell  = (cell_x == 0) && (cell_y == 0);

            if (is_first_cell) {
                image(x, y) = Pixel{255, 0, 0};
            }
            else {
                if (is_light) {
                    image(x, y) = Pixel{255, 255, 255};
                }
                else {
                    image(x, y) = Pixel{0, 0, 0};
                }
            }
        }
    }

    std::ofstream out("out.ppm", std::ios::binary);

    if (!out) {
        std::cerr << "Could not open out.ppm!" << '\n';
        return 1;
    }

    if(!write_ppm(out, image)) {
        std::cerr << "Could not write to out.ppm!" << '\n';
        return 1;
    }

    return 0;
}

