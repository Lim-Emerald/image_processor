#include "image_processor.h"
#include <string>
#include <tuple>
#include <vector>

int main(int argc, char** argv) {
    BMPFile bmp_file;  // Класс для хранения информации о BMP файле и его изображении
    std::string input_file_name;   // Расположение файла для чтения
    std::string output_file_name;  // Расположение файла для записи
    std::vector<Filter> filters;   // Массив фильтров
    // Преобразуем аргументы компиляции в более удобный вид
    std::tie(input_file_name, output_file_name, filters) = FilterParserArgv(argc, argv);
    // Считаем данные из BMP файла
    bmp_file.ReadBMP(input_file_name);
    // Применим фильтры
    for (auto filter : filters) {
        filter.Apply(bmp_file.rgb_matrix);
    }
    // Выведем полученное изображение в BMP файл
    bmp_file.WriteBMP(output_file_name);
}
