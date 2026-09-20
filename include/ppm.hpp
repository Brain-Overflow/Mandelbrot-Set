#pragma once

#include <fstream>
#include "image.hpp"

std::ostream& write_ppm(std::ostream& out, const Image& image);