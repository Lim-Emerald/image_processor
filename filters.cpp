#include "filters.h"
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <stdexcept>

Filter::~Filter() {
    Clear();
}

void Filter::Clear() {
    filter_name.clear();
    filter_options.clear();
}

size_t Filter::GetFilterPosition(std::string& name) {
    size_t position = 0;
    while (position < filter_names_.size() && filter_names_[position] != name) {
        ++position;
    }
    // Если не нашли фильтр скажем, что такого фильтра нет
    if (position == filter_names_.size()) {
        throw std::invalid_argument("Filter recognition error: invalid name filter <" + name + ">");
    }
    return position;
}

void Filter::SetFilter(std::string& name, std::vector<std::string>& options) {
    try {
        // Задание имени фильтра
        SetFilterName(name);
        // Задание массива параметров
        SetFilterOptions(options);
    } catch (const std::invalid_argument& e) {
        Clear();
        std::cerr << e.what() << '\n';
    }
}

void Filter::SetFilterName(std::string& name) {
    filter_name = name;
}

void Filter::SetFilterOptions(std::vector<std::string>& options) {
    size_t filter_pos = GetFilterPosition(filter_name);
    if (options.size() != number_options_[filter_pos]) {
        throw std::invalid_argument("Filter recognition error: invalid number of arguments");
    }
    filter_options = options;
}

void Filter::Apply(RGBMatrix& rgb_matrix) {
    // Если фильтр не определен не будем его применять
    if (filter_name.empty()) {
        return;
    }

    // Создадим абстрактный фильтр и инциализируем его при помощи наследования
    Filter* abstract_filter = nullptr;
    switch (GetFilterPosition(filter_name)) {
        case FILTER_NAMES::CROP:
            abstract_filter = new CropFilter();
            break;
        case FILTER_NAMES::GRAYSCALE:
            abstract_filter = new GrayscaleFilter();
            break;
        case FILTER_NAMES::NEGATIVE:
            abstract_filter = new NegativeFilter();
            break;
        case FILTER_NAMES::SHARPENING:
            abstract_filter = new SharpeningFilter();
            break;
        case FILTER_NAMES::EDGEDETECTION:
            abstract_filter = new EdgeDetectionFilter();
            break;
        case FILTER_NAMES::GAUSSIANBLUR:
            abstract_filter = new GaussianBlurFilter();
            break;
    }
    abstract_filter->SetFilter(filter_name, filter_options);
    abstract_filter->Apply(rgb_matrix);
    delete abstract_filter;
}

void CropFilter::Apply(RGBMatrix& rgb_matrix) {
    int32_t new_height = stoi(filter_options[OPTIONS::HEIGHT]);
    int32_t new_width = stoi(filter_options[OPTIONS::WIDTH]);
    if (new_height <= 0 || new_width <= 0) {
        throw std::invalid_argument("Filter application error: image crop parameters must be positive numbers");
    }
    // создадим новую матрицу и запишем туда нужные пиксели
    new_height = std::min(new_height, static_cast<int32_t>(rgb_matrix.height));
    new_width = std::min(new_width, static_cast<int32_t>(rgb_matrix.width));
    RGBMatrix new_matrix(static_cast<size_t>(new_height), static_cast<size_t>(new_width));
    for (size_t i = 0; i < std::min(new_matrix.height, rgb_matrix.height); ++i) {
        for (size_t j = 0; j < std::min(new_matrix.width, rgb_matrix.width); ++j) {
            new_matrix.matrix[i][j] = rgb_matrix.matrix[i][j];
        }
    }
    rgb_matrix = new_matrix;
}

void GrayscaleFilter::Apply(RGBMatrix& rgb_matrix) {
    static const double TWO_HUNDRED_NINETY_NINE = 0.299;
    static const double FIVE_HUNDRED_EIGHTY_SEVEN = 0.587;
    static const double ONE_HUNDRED_AND_FOURTEEN = 0.114;
    for (auto& row : rgb_matrix.matrix) {
        for (auto& pixel : row) {
            uint8_t new_color = static_cast<uint8_t>(round(TWO_HUNDRED_NINETY_NINE * static_cast<double>(pixel.r) +
                                                           FIVE_HUNDRED_EIGHTY_SEVEN * static_cast<double>(pixel.g) +
                                                           ONE_HUNDRED_AND_FOURTEEN * static_cast<double>(pixel.b)));
            pixel.r = new_color;
            pixel.g = new_color;
            pixel.b = new_color;
        }
    }
}

void NegativeFilter::Apply(RGBMatrix& rgb_matrix) {
    static const uint8_t MAX_INT8_T = 255;
    for (auto& row : rgb_matrix.matrix) {
        for (auto& pixel : row) {
            pixel.r = MAX_INT8_T - pixel.r;
            pixel.g = MAX_INT8_T - pixel.g;
            pixel.b = MAX_INT8_T - pixel.b;
        }
    }
}

void SharpeningFilter::Apply(RGBMatrix& rgb_matrix) {
    static const int32_t MAX_INT8_T = 255;
    static const size_t STEPS = 5;
    // Зададим изменения i и j и коефициенты для цветов, при применении матрицы SharpeningFilter к rgb_matrix
    std::vector<int32_t> delta_x = {0, 0, 0, -1, 1};
    std::vector<int32_t> delta_y = {0, -1, 1, 0, 0};
    static const int32_t FIVE = 5;
    static const int32_t MINUS_ONE = -1;
    std::vector<int32_t> delta_color = {FIVE, MINUS_ONE, MINUS_ONE, MINUS_ONE, MINUS_ONE};
    // создадим новую матрицу и запишем туда результат применения фильтра
    RGBMatrix new_matrix(rgb_matrix.height, rgb_matrix.width);
    for (int32_t i = 0; i < rgb_matrix.height; ++i) {
        for (int32_t j = 0; j < rgb_matrix.width; ++j) {
            int32_t new_r = 0;
            int32_t new_g = 0;
            int32_t new_b = 0;
            for (size_t step = 0; step < STEPS; ++step) {
                int32_t new_i = i + delta_x[step];
                int32_t new_j = j + delta_y[step];
                new_i = std::clamp(new_i, 0, static_cast<int32_t>(rgb_matrix.height) - 1);
                new_j = std::clamp(new_j, 0, static_cast<int32_t>(rgb_matrix.width) - 1);
                new_r += delta_color[step] * static_cast<int32_t>(rgb_matrix.matrix[new_i][new_j].r);
                new_g += delta_color[step] * static_cast<int32_t>(rgb_matrix.matrix[new_i][new_j].g);
                new_b += delta_color[step] * static_cast<int32_t>(rgb_matrix.matrix[new_i][new_j].b);
            }
            new_r = std::clamp(new_r, 0, MAX_INT8_T);
            new_g = std::clamp(new_g, 0, MAX_INT8_T);
            new_b = std::clamp(new_b, 0, MAX_INT8_T);
            new_matrix.matrix[i][j] =
                RGBPixel(static_cast<int8_t>(new_r), static_cast<int8_t>(new_g), static_cast<int8_t>(new_b));
        }
    }
    rgb_matrix = new_matrix;
}

void EdgeDetectionFilter::Apply(RGBMatrix& rgb_matrix) {
    static const int32_t MAX_INT8_T = 255;
    GrayscaleFilter grayscale_filter;
    grayscale_filter.Apply(rgb_matrix);
    static const size_t STEPS = 5;
    static const int32_t THRESHOLD =
        static_cast<int32_t>(static_cast<double>(MAX_INT8_T) * stod(filter_options[OPTIONS::THRESHOLD]));
    // Зададим изменения i и j и коефициенты для цветов, при применении матрицы SharpeningFilter к rgb_matrix
    std::vector<int32_t> delta_x = {0, 0, 0, -1, 1};
    std::vector<int32_t> delta_y = {0, -1, 1, 0, 0};
    std::vector<int32_t> delta_color = {4, -1, -1, -1, -1};
    // создадим новую матрицу и запишем туда результат применения фильтра
    RGBMatrix new_matrix(rgb_matrix.height, rgb_matrix.width);
    for (int32_t i = 0; i < rgb_matrix.height; ++i) {
        for (int32_t j = 0; j < rgb_matrix.width; ++j) {
            int32_t new_color = 0;
            for (size_t step = 0; step < STEPS; ++step) {
                int32_t new_i = i + delta_x[step];
                int32_t new_j = j + delta_y[step];
                new_i = std::clamp(new_i, 0, static_cast<int32_t>(rgb_matrix.height) - 1);
                new_j = std::clamp(new_j, 0, static_cast<int32_t>(rgb_matrix.width) - 1);
                new_color += delta_color[step] * static_cast<int32_t>(rgb_matrix.matrix[new_i][new_j].r);
            }
            if (new_color > THRESHOLD) {
                new_color = MAX_INT8_T;
            } else {
                new_color = 0;
            }
            new_matrix.matrix[i][j] = RGBPixel(static_cast<int8_t>(new_color), static_cast<int8_t>(new_color),
                                               static_cast<int8_t>(new_color));
        }
    }
    rgb_matrix = new_matrix;
}

void GaussianBlurFilter::Apply(RGBMatrix& rgb_matrix) {
    static const double MIN_INT8_T = 0.0;
    static const double MAX_INT8_T = 255.0;
    static const double TWO = 2.0;
    static const double SIGMA = stod(filter_options[OPTIONS::SIGMA]);
    static const double SIGMA_POW_TWO = SIGMA * SIGMA;
    static const double GAUSS_COEFFICIENT = sqrt(TWO * M_PI * SIGMA_POW_TWO);
    static const double EXPONENT_COEFFICIENT = TWO * SIGMA_POW_TWO;
    static const int32_t SIGMA_NEIGHBORHOOD = static_cast<int32_t>(round(3.0 * SIGMA)) + 1;
    // создадим новую матрицу и запишем туда результат применения фильтра по строкам
    RGBMatrix new_matrix(rgb_matrix.height, rgb_matrix.width);
    for (size_t index = 0; index < new_matrix.height; ++index) {
        RGBLine new_row = new_matrix.GetRow(index);
        RGBLine row = rgb_matrix.GetRow(index);
        for (int32_t i = 0; i < static_cast<int32_t>(row.line.size()); ++i) {
            double new_r = 0;
            double new_g = 0;
            double new_b = 0;
            double sum_coef = 0;
            for (int32_t j = i - SIGMA_NEIGHBORHOOD; j <= i + SIGMA_NEIGHBORHOOD; ++j) {
                int32_t new_j = std::clamp(j, 0, static_cast<int32_t>(row.line.size() - 1));
                // Применим размытие Гаусса
                double square = static_cast<double>((j - i) * (j - i));
                double coefficient = exp(-square / EXPONENT_COEFFICIENT) / GAUSS_COEFFICIENT;
                sum_coef += coefficient;
                new_r += static_cast<double>(row.line[new_j]->r) * coefficient;
                new_g += static_cast<double>(row.line[new_j]->g) * coefficient;
                new_b += static_cast<double>(row.line[new_j]->b) * coefficient;
            }
            new_r /= sum_coef;
            new_g /= sum_coef;
            new_b /= sum_coef;
            int64_t new_red = llround(std::clamp(new_r, MIN_INT8_T, MAX_INT8_T));
            int64_t new_green = llround(std::clamp(new_g, MIN_INT8_T, MAX_INT8_T));
            int64_t new_blue = llround(std::clamp(new_b, MIN_INT8_T, MAX_INT8_T));
            new_row.line[i]->r = static_cast<uint8_t>(new_red);
            new_row.line[i]->g = static_cast<uint8_t>(new_green);
            new_row.line[i]->b = static_cast<uint8_t>(new_blue);
        }
    }
    // запишем результат применения фильтра по столбцам в нашу матрицу
    rgb_matrix = new_matrix;
    for (size_t index = 0; index < rgb_matrix.width; ++index) {
        RGBLine column = rgb_matrix.GetColumn(index);
        RGBLine new_column = new_matrix.GetColumn(index);
        for (int32_t i = 0; i < static_cast<int32_t>(column.line.size()); ++i) {
            double new_r = 0;
            double new_g = 0;
            double new_b = 0;
            double sum_coef = 0;
            for (int32_t j = i - SIGMA_NEIGHBORHOOD; j <= i + SIGMA_NEIGHBORHOOD; ++j) {
                int32_t new_j = std::clamp(j, 0, static_cast<int32_t>(column.line.size() - 1));
                // Применим размытие Гаусса
                double square = static_cast<double>((j - i) * (j - i));
                double coefficient = exp(-square / EXPONENT_COEFFICIENT) / GAUSS_COEFFICIENT;
                sum_coef += coefficient;
                new_r += static_cast<double>(column.line[new_j]->r) * coefficient;
                new_g += static_cast<double>(column.line[new_j]->g) * coefficient;
                new_b += static_cast<double>(column.line[new_j]->b) * coefficient;
            }
            new_r /= sum_coef;
            new_g /= sum_coef;
            new_b /= sum_coef;
            int64_t new_red = llround(std::clamp(new_r, MIN_INT8_T, MAX_INT8_T));
            int64_t new_green = llround(std::clamp(new_g, MIN_INT8_T, MAX_INT8_T));
            int64_t new_blue = llround(std::clamp(new_b, MIN_INT8_T, MAX_INT8_T));
            new_column.line[i]->r = static_cast<uint8_t>(new_red);
            new_column.line[i]->g = static_cast<uint8_t>(new_green);
            new_column.line[i]->b = static_cast<uint8_t>(new_blue);
        }
    }
    rgb_matrix = new_matrix;
}