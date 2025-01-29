#include "filter_parser_argv.h"
#include <stdexcept>

std::tuple<std::string, std::string, std::vector<Filter>> FilterParserArgv(int argc, char** argv) {
    if (argc < 3) {
        throw std::runtime_error("Parsing error: insufficient number of arguments");
    }
    // Преобразуем аргументы, отвечающие за input и output файл
    std::string input_file_name(argv[1]);
    std::string output_file_name(argv[2]);
    // Преобразуем аргументы в соответствующие им фильтры
    std::vector<Filter> filters;
    for (int i = 3; i < argc; ++i) {
        if (argv[i][0] != '-') {
            throw std::runtime_error("Parsing error: invalid filter format");
        }
        // Зададим имя фильтра
        std::string filter_name(argv[i]);
        filter_name = filter_name.substr(1);
        // Преобразуем аргументы в параметры фильтра
        ++i;
        std::vector<std::string> filter_options;
        while (i < argc && argv[i][0] != '-') {
            std::string option(argv[i]);
            filter_options.push_back(option);
            ++i;
        }
        if (i < argc && argv[i][0] == '-') {
            --i;
        }
        // Создадим фильтр и присвоим ему имя и параметры
        Filter filter;
        filter.SetFilter(filter_name, filter_options);
        filters.push_back(filter);
    }
    return {input_file_name, output_file_name, filters};
}