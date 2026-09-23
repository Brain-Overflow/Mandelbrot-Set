#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <thread>
#include "image.hpp"
#include "ppm.hpp"

Pixel iteration_count_to_pixel(int escape_iteration_count, int iteration_limit) {
    if (escape_iteration_count == iteration_limit) {
        return BLACK;
    }
    else {
        double logarithmic_fraction = std::log(static_cast<double>(escape_iteration_count) + 1) / std::log(static_cast<double>(iteration_limit) + 1);
        unsigned char brightness = static_cast<unsigned char>(255.0 * (1.0 - logarithmic_fraction));

        return to_grayscale_pixel(brightness);
    }
}

std::vector<Pixel> create_palette(int iteration_limit) {
    std::vector<Pixel> palette(iteration_limit + 1);

    for (int iteration = 0; iteration <= iteration_limit; ++iteration) {
        palette[iteration] = iteration_count_to_pixel(iteration, iteration_limit);
    }

    return palette;
}

template <typename Function>
double stopwatch_ms(Function&& function) {
    std::chrono::time_point starting_time = std::chrono::steady_clock::now();

    function();

    std::chrono::time_point ending_time = std::chrono::steady_clock::now();

    return std::chrono::duration<double, std::milli>(ending_time - starting_time).count();
}

double median(std::vector<double> elements) {
    std::sort(elements.begin(), elements.end());

    return elements[elements.size() / 2];
}

struct RenderParameters {
    int iteration_limit;
    double center_a;
    double center_b;
    double window_width;
};

void render_mandelbrot_set(Image& image, const RenderParameters& parameters) {
    const std::vector<Pixel> palette = create_palette(parameters.iteration_limit);

    int image_height = image.get_height();
    int image_width = image.get_width();

    double pixel_size = parameters.window_width / image_width;
    double window_height = pixel_size * image_height;
    double left = parameters.center_a - parameters.window_width / 2;
    double top = parameters.center_b + window_height / 2;

    for (int pixel_y = 0; pixel_y < image_height; ++pixel_y) {
        for (int pixel_x = 0; pixel_x < image_width; ++pixel_x) {
            double a = left + (static_cast<double>(pixel_x) + 0.5) * pixel_size;
            double b = top - (static_cast<double>(pixel_y) + 0.5) * pixel_size;

            double x = 0;
            double y = 0;

            int escape_iteration_count = parameters.iteration_limit;

            for (int iteration_count = 0; iteration_count < parameters.iteration_limit; ++iteration_count) {
                double new_x = (x * x) - (y * y) + a;
                double new_y = 2 * x * y + b;

                if ((new_x * new_x) + (new_y * new_y) > (2 * 2)) {
                    escape_iteration_count = iteration_count;
                    break;
                }

                x = new_x;
                y = new_y;
            }

            ASSERTM(escape_iteration_count >= 0 && escape_iteration_count < static_cast<int>(palette.size()), "Index is out of bounds for palette!");
            image(pixel_x, pixel_y) = palette[escape_iteration_count];
        }
    }
}

int main() {
    int image_size = 2048;

    Image image(image_size, image_size);

    const RenderParameters parameters{1000, -0.75, 0, 2.5};

    std::vector<double> ms_results;

    render_mandelbrot_set(image, parameters); // Render once before timing to warm the system up.

    for (int iteration = 0; iteration < 10; ++iteration) {
        double ms_result = stopwatch_ms([&] { render_mandelbrot_set(image, parameters); });

        ms_results.push_back(ms_result);
    }

    const double milliseconds_in_second = 1000.0;

    for (int result_index = 0; result_index < ms_results.size(); ++result_index) {
        std::cerr << std::format("Test {}: {:.2f} milliseconds | {:.2f} seconds\n", result_index + 1, ms_results[result_index], ms_results[result_index] / milliseconds_in_second);
    }

    std::cerr << std::format("Median: {:.2f} milliseconds | {:.2f} seconds\n", median(ms_results), median(ms_results) / milliseconds_in_second);

    auto [min_iterator, max_iterator] = std::minmax_element(ms_results.begin(), ms_results.end());

    std::cerr << std::format("Minimum: {:.2f} milliseconds | {:.2f} seconds\nMaximum: {:.2f} milliseconds| {:.2f} seconds\n", *min_iterator, *min_iterator / milliseconds_in_second, *max_iterator, *max_iterator / milliseconds_in_second);
  
    std::cerr << std::format("Supported Concurrent Threads: {}\n", std::thread::hardware_concurrency());

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