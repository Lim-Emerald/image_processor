#include "rgb_matrix.h"
#include <stdexcept>

RGBPixel::RGBPixel() {
    r = 0;
    g = 0;
    b = 0;
}

RGBPixel::RGBPixel(uint8_t red, uint8_t green, uint8_t blue) {
    r = red;
    g = green;
    b = blue;
}

RGBLine::RGBLine() {
    index = -1;
}

RGBLine::RGBLine(size_t index, size_t size) {
    RGBLine::index = index;
    line.resize(size, nullptr);
}

RGBMatrix::RGBMatrix() {
    height = 0;
    width = 0;
}

RGBMatrix::RGBMatrix(size_t height, size_t width) {
    if (height <= 0 || width <= 0) {
        throw std::runtime_error("Program error: matrix dimensions are not positive");
    }
    RGBMatrix::height = height;
    RGBMatrix::width = width;
    matrix.resize(height, std::vector<RGBPixel>(width, {0, 0, 0}));
}

void RGBMatrix::Clear() {
    height = 0;
    width = 0;
    matrix.clear();
}

RGBLine RGBMatrix::GetRow(size_t index) {
    if (index < 0 || index >= height) {
        throw std::runtime_error("Program error: accessing a non-existent row in a RGB matrix");
    }
    RGBLine row(index, width);
    for (size_t i = 0; i < width; ++i) {
        row.line[i] = &matrix[index][i];
    }
    return row;
}

RGBLine RGBMatrix::GetColumn(size_t index) {
    if (index < 0 || index >= width) {
        throw std::runtime_error("Program error: accessing a non-existent column in a RGB matrix");
    }
    RGBLine row(index, height);
    for (size_t i = 0; i < height; ++i) {
        row.line[i] = &matrix[i][index];
    }
    return row;
}