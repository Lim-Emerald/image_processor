#ifndef RGB_MATRIX_H
#define RGB_MATRIX_H

#include <cstdint>
#include <vector>

struct RGBPixel {
    uint8_t b;  // Интенсивность синего цвета
    uint8_t g;  // Интенсивность зеленого цвета
    uint8_t r;  // Интенсивность красного цвета

    RGBPixel();
    RGBPixel(uint8_t red, uint8_t green, uint8_t blue);
};

struct RGBLine {
    size_t index;  // индекс строки / столбца в матрице
    std::vector<RGBPixel*> line;

    RGBLine();
    RGBLine(size_t index, size_t size);
};

struct RGBMatrix {
    size_t height;  // высота матрицы
    size_t width;   // ширина матрицы
    std::vector<std::vector<RGBPixel>> matrix;

    RGBMatrix();
    RGBMatrix(size_t height, size_t width);

    RGBLine GetRow(size_t index);     // получить строку по индексу
    RGBLine GetColumn(size_t index);  // получить столбец по индексу
    void Clear();
};

#endif