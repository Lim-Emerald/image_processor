#ifndef FILTERS_H
#define FILTERS_H

#include "rgb_matrix.h"
#include <string>
#include <vector>

class Filter {
public:
    std::string filter_name;                  // Имя филтра
    std::vector<std::string> filter_options;  // Массив параметров фильтра

public:
    virtual ~Filter();
    void Clear();
    void SetFilter(std::string& name, std::vector<std::string>& options);
    virtual void Apply(RGBMatrix& rgb_matrix);

private:
    // Поддерживаемые фильтры и количество параметров для каждого фильтра
    enum FILTER_NAMES { CROP, GRAYSCALE, NEGATIVE, SHARPENING, EDGEDETECTION, GAUSSIANBLUR };
    const std::vector<std::string> filter_names_ = {"crop", "gs", "neg", "sharp", "edge", "blur"};
    const std::vector<size_t> number_options_ = {2, 0, 0, 0, 1, 1};

private:
    size_t GetFilterPosition(std::string& name);
    void SetFilterName(std::string& name);
    void SetFilterOptions(std::vector<std::string>& options);
};

class CropFilter : public Filter {
public:
    void Apply(RGBMatrix& rgb_matrix) override;

private:
    enum OPTIONS { WIDTH, HEIGHT };
};

class GrayscaleFilter : public Filter {
public:
    void Apply(RGBMatrix& rgb_matrix) override;
};

class NegativeFilter : public Filter {
public:
    void Apply(RGBMatrix& rgb_matrix) override;
};

class SharpeningFilter : public Filter {
public:
    void Apply(RGBMatrix& rgb_matrix) override;
};

class EdgeDetectionFilter : public Filter {
public:
    void Apply(RGBMatrix& rgb_matrix) override;

private:
    enum OPTIONS { THRESHOLD };
};

class GaussianBlurFilter : public Filter {
public:
    void Apply(RGBMatrix& rgb_matrix) override;

private:
    enum OPTIONS { SIGMA };
};

#endif