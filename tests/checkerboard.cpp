#include <iostream>
#include "image.hpp"
#include "ppm.hpp"

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

