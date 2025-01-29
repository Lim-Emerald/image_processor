cmake_minimum_required(VERSION 3.8)

project(image-processor)

add_executable(
    image_processor
    image_processor.cpp
    image_processor.h
    filter_parser_argv.cpp
    filter_parser_argv.h
    filters.cpp
    filters.h
    bmp_file.cpp
    bmp_file.h
    rgb_matrix.cpp
    rgb_matrix.h
)
