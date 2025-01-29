#ifndef BMP_FILE_H
#define BMP_FILE_H

#include "rgb_matrix.h"
#include <cstdint>
#include <fstream>
#include <string>

#pragma pack(push, 1)
struct BMPFileHeader {
    uint16_t type{0};       // Тип файла
    uint32_t size{0};       // Размер файла в байтах
    uint16_t reserved1{0};  // Зарезервированные поля
    uint16_t reserved2{0};
    uint32_t offset_data{0};  // Смещение битового массива относительно начала файла
};
#pragma pack(pop)

#pragma pack(push, 1)
struct BMPInfoHeader {
    uint32_t size{0};               // Размер структуры
    int32_t width{0};               // Ширина изображения в пикселях
    int32_t height{0};              // Высота изображения в пикселях
    uint16_t planes{0};             // Количество плоскостей
    uint16_t bit_count{0};          // Глубина цвета в битах на пиксель
    uint32_t compression{0};        // Тип сжатия
    uint32_t size_image{0};         // Размер изображения в байтах
    int32_t x_pixels_per_meter{0};  // Горизонтальное разрешение
    int32_t y_pixels_per_meter{0};  // Вертикальное разрешение
    uint32_t colors_used{0};  // Количество используемых цветов кодовой таблицы
    uint32_t colors_important{0};  // Количество основных цветов
};
#pragma pack(pop)

class BMPFile {
public:
    BMPFileHeader bmp_file_header;
    BMPInfoHeader bmp_info_header;
    RGBMatrix rgb_matrix;

private:
    static const uint16_t BMP_TYPE = 0x4D42;  // BM - Type
    enum SUPPORTED_BIT_COUNT {
        TWENTY_FOUR  // 24-битные BMP файлы
    };
    static const uint16_t TWENTY_FOUR_BIT = 0x18;
    std::vector<uint16_t> supported_bit_count_ = {TWENTY_FOUR_BIT};  // поддерживаемые глубины цвета

public:
    void ReadBMP(std::string& file_name);
    void WriteBMP(std::string& file_name);
    void HeadersUpdate();
    void Clear();

private:
    void ReadFileHeader(std::ifstream& inp);
    void ReadInfoHeader(std::ifstream& inp);
    void ReadMatrix24bit(std::ifstream& inp);
    void WriteFileHeader(std::ofstream& outp);
    void WriteInfoHeader(std::ofstream& outp);
    void WriteMatrix24bit(std::ofstream& outp);
};

#endif