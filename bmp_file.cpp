#include "bmp_file.h"
#include <iostream>

void BMPFile::ReadBMP(std::string& file_name) {
    std::ifstream inp;
    try {
        // Открытие файла
        inp.open(file_name, std::ios::binary);
        if (inp.fail()) {
            throw std::runtime_error("Error open graphic file for reading");
        }
        // Чтение структуры bmp_file_header
        ReadFileHeader(inp);
        // Проверка типа файла
        if (bmp_file_header.type != BMP_TYPE) {
            throw std::runtime_error("Invalid source file type to read: not BMP");
        }

        // Чтение структуры bmp_info_header
        ReadInfoHeader(inp);

        // Проверка глубины цвета
        bool is_find = false;
        for (auto & supported_bit : supported_bit_count_) {
            if (supported_bit == bmp_info_header.bit_count) {
                is_find = true;
                break;
            }
        }
        if (!is_find) {
            throw std::runtime_error("Invalid source file type to write: unsupported color depth");
        }

        // Переход к месту матрицы пикселей
        inp.seekg(bmp_file_header.offset_data, inp.beg);

        // Чтение матрицы пикселей
        if (bmp_info_header.bit_count == supported_bit_count_[SUPPORTED_BIT_COUNT::TWENTY_FOUR]) {
            ReadMatrix24bit(inp);
        }

        // Закрытие файла
        inp.close();
    } catch (const std::runtime_error& e) {
        Clear();
        inp.close();
        std::cerr << e.what() << '\n';
        exit(0);
    }
}

void BMPFile::ReadFileHeader(std::ifstream& inp) {
    inp.read(reinterpret_cast<char*>(&bmp_file_header), sizeof(bmp_file_header));
    if (inp.fail()) {
        throw std::runtime_error("File reading error: cannot read file header");
    }
}

void BMPFile::ReadInfoHeader(std::ifstream& inp) {
    inp.read(reinterpret_cast<char*>(&bmp_info_header), sizeof(bmp_info_header));
    if (inp.fail()) {
        throw std::runtime_error("File reading error: cannot read info header");
    }
}

void BMPFile::ReadMatrix24bit(std::ifstream& inp) {
    if (bmp_info_header.height <= 0 || bmp_info_header.width <= 0) {
        throw std::runtime_error("Invalid source file type to read: matrix dimensions are not positive");
    }
    rgb_matrix = RGBMatrix(bmp_info_header.height, bmp_info_header.width);
    // Число байт для пикселей в строке
    static const uint32_t EIGHT = 8;
    uint32_t number_of_row_bytes = static_cast<uint32_t>(bmp_info_header.width) * bmp_info_header.bit_count / EIGHT;
    // Число байт нужное для дополнения длины битовой строки до кратной 32
    static const uint32_t FOUR = 4;
    uint32_t number_of_extra_bytes = (FOUR - number_of_row_bytes % FOUR) % FOUR;
    for (size_t i = bmp_info_header.height - 1;; --i) {
        inp.read(reinterpret_cast<char*>(rgb_matrix.matrix[i].data()), number_of_row_bytes);
        if (inp.fail()) {
            throw std::runtime_error("File reading error: cannot read pixel matrix");
        }
        std::vector<uint8_t> temporary(number_of_extra_bytes);
        inp.read(reinterpret_cast<char*>(temporary.data()), number_of_extra_bytes);
        if (inp.fail()) {
            throw std::runtime_error("File reading error: cannot read pixel matrix");
        }
        if (i == 0) {
            break;
        }
    }
    bmp_file_header.size += bmp_info_header.height * (number_of_row_bytes + number_of_extra_bytes);
}

void BMPFile::WriteBMP(std::string& file_name) {
    std::ofstream outp;
    try {
        // Проверка типа файла
        if (bmp_file_header.type != BMP_TYPE) {
            throw std::runtime_error("Invalid source file type to write: not BMP");
        }
        // Проверка глубины кодирования
        bool is_find = false;
        for (auto & supported_bit : supported_bit_count_) {
            if (supported_bit == bmp_info_header.bit_count) {
                is_find = true;
                break;
            }
        }
        if (!is_find) {
            throw std::runtime_error("Invalid source file type to write: unsupported color depth");
        }
        // Обновление данных о файле если они были изменены
        HeadersUpdate();
        // Открытие файла
        outp.open(file_name, std::ios::binary);
        if (outp.fail()) {
            throw std::runtime_error("Error open graphic file for writing");
        }
        // Запись структуры bmp_file_header
        WriteFileHeader(outp);
        // Запись структуры bmp_info_header
        WriteInfoHeader(outp);
        // Запись матрицы пикселей
        WriteMatrix24bit(outp);
        // Закрытие файла
        outp.close();
    } catch (const std::runtime_error& e) {
        outp.clear();
        outp.close();
        std::cerr << e.what() << '\n';
        exit(0);
    }
}

void BMPFile::HeadersUpdate() {
    // Перезапись данных файла
    if (bmp_info_header.bit_count == supported_bit_count_[SUPPORTED_BIT_COUNT::TWENTY_FOUR]) {
        bmp_info_header.size = sizeof(BMPInfoHeader);
        bmp_file_header.offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        bmp_file_header.size = bmp_file_header.offset_data;
        bmp_info_header.height = static_cast<int32_t>(rgb_matrix.height);
        bmp_info_header.width = static_cast<int32_t>(rgb_matrix.width);
        // Число байт для пикселей в строке
        static const uint32_t EIGHT = 8;
        uint32_t number_of_row_bytes = static_cast<uint32_t>(bmp_info_header.width) * bmp_info_header.bit_count / EIGHT;
        // Число байт нужное для дополнения длины битовой строки до кратной 32
        static const uint32_t FOUR = 4;
        uint32_t number_of_extra_bytes = (FOUR - number_of_row_bytes % FOUR) % FOUR;
        bmp_file_header.size += bmp_info_header.height * (number_of_row_bytes + number_of_extra_bytes);
    }
}

void BMPFile::WriteFileHeader(std::ofstream& outp) {
    outp.write(reinterpret_cast<const char*>(&bmp_file_header), sizeof(bmp_file_header));
    if (outp.fail()) {
        throw std::runtime_error("Error writing file header");
    }
}

void BMPFile::WriteInfoHeader(std::ofstream& outp) {
    outp.write(reinterpret_cast<const char*>(&bmp_info_header), sizeof(bmp_info_header));
    if (outp.fail()) {
        throw std::runtime_error("Error writing info header");
    }
}

void BMPFile::WriteMatrix24bit(std::ofstream& outp) {
    // Число байт для пикселей в строке
    static const uint32_t EIGHT = 8;
    uint32_t number_of_row_bytes = static_cast<uint32_t>(bmp_info_header.width) * bmp_info_header.bit_count / EIGHT;
    // Число байт нужное для дополнения длины битовой строки до кратной 32
    static const uint32_t FOUR = 4;
    uint32_t number_of_extra_bytes = (FOUR - number_of_row_bytes % FOUR) % FOUR;
    for (size_t i = bmp_info_header.height - 1;; --i) {
        outp.write(reinterpret_cast<char*>(rgb_matrix.matrix[i].data()), number_of_row_bytes);
        if (outp.fail()) {
            throw std::runtime_error("Error writing pixel matrix");
        }
        std::vector<uint8_t> temporary(number_of_extra_bytes);
        outp.write(reinterpret_cast<char*>(temporary.data()), number_of_extra_bytes);
        if (outp.fail()) {
            throw std::runtime_error("Error writing pixel matrix");
        }
        if (i == 0) {
            break;
        }
    }
}

void BMPFile::Clear() {
    rgb_matrix.Clear();
}