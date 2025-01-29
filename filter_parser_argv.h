#ifndef FILTER_PARSER_ARGV
#define FILTER_PARSER_ARGV

#include "filters.h"
#include <string>
#include <tuple>
#include <vector>

std::tuple<std::string, std::string, std::vector<Filter>> FilterParserArgv(int argc, char** argv);

#endif