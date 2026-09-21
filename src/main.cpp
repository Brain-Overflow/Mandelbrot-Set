#include <iostream>
#include <iomanip>
#include <cmath>
#include "image.hpp"
#include "ppm.hpp"

int main() {
    const int ITERATION_LIMIT = 1000;

    const double A_MIN = -2.0, A_MAX = 0.5; // Maximum range is -2.5 to 1.0.
    const double B_MIN = -1.25, B_MAX = 1.25;

    int image_size = 512;

    Image image(image_size, image_size);

    int image_height = image.get_height();
    int image_width = image.get_width();

    for (int pixel_y = 0; pixel_y < image_height; ++pixel_y) {
        for (int pixel_x = 0; pixel_x < image_width; ++pixel_x) {
            double a = A_MIN + (static_cast<double>(pixel_x) / static_cast<double>(image_width)) * (A_MAX - A_MIN);
            double b = B_MIN + (static_cast<double>(pixel_y) / static_cast<double>(image_height)) * (B_MAX - B_MIN);

            double x = 0;
            double y = 0;

            int escape_iteration_count = ITERATION_LIMIT;

            for (int iteration_count = 0; iteration_count < ITERATION_LIMIT; ++iteration_count) {
                double new_x = (x * x) - (y * y) + a;
                double new_y = 2 * x * y + b;

                if ((new_x * new_x) + (new_y * new_y) > (2 * 2)) {
                    escape_iteration_count = iteration_count;
                    break;
                }

                x = new_x;
                y = new_y;
            }

            //std::cout << std::setw(5) << escape_iteration_count;

            //double iteration_fraction = static_cast<double>(escape_iteration_count) / static_cast<double>(ITERATION_LIMIT);
            //unsigned char brightness = static_cast<unsigned char>(255.0 * (1.0 - iteration_fraction));

            unsigned char brightness = static_cast<unsigned char>(255.0 * (1.0 - std::log(static_cast<double>(escape_iteration_count) + 1) / std::log(static_cast<double>(ITERATION_LIMIT) + 1)));

            image(pixel_x, pixel_y) = from_grayscale(brightness);
        }

        //std::cout << '\n';
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

