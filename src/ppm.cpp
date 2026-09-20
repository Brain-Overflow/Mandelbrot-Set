#include "ppm.hpp"

std::ostream& write_ppm(std::ostream& out, const Image& image) {
    out << "P6" << '\n' << image.get_width() << ' ' << image.get_height() << '\n' << "255" << '\n';

    const auto& pixels = image.get_pixels();

    out.write(reinterpret_cast<const char*>(pixels.data()), static_cast<std::streamsize>(pixels.size() * sizeof(Pixel)));

    return out;
}