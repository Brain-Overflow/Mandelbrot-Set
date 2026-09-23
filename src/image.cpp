#include "image.hpp"

Image::Image(int width, int height, std::vector<Pixel> pixels) : width(width), height(height), pixels(std::move(pixels)) {
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

Image::Image(int width, int height) : Image(width, height, std::vector<Pixel>(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), Pixel{0, 0, 0})) {

}

int Image::get_width() const {
    return width;
}

int Image::get_height() const {
    return height;
}

const std::vector<Pixel>& Image::get_pixels() const {
    return pixels;
}

const Pixel& Image::operator()(int x, int y) const {
    ASSERTM(x >= 0 && x < width, "X coordinate is out of range.");
    ASSERTM(y >= 0 && y < height, "Y coordinate is out of range.");

    return pixels[static_cast<std::size_t>(y) * width + static_cast<std::size_t>(x)];
}
    
Pixel& Image::operator()(int x, int y) {
    return const_cast<Pixel&>(std::as_const(*this)(x, y));
}