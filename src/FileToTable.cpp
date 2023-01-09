#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "FileToTable.hpp"

FileToTable::FileToTable() {
    this->delimiter = ',';
}

void FileToTable::numeric(const std::string ifname, std::vector<std::vector<double>>& table) {
    std::ifstream input_file(ifname);

    // 1-line string
    std::string line_string;
    // get 1-line string from input file
    while (std::getline(input_file, line_string)) {
        if (table.size() == 0) {
            if (line_string.find(",") == std::string::npos) {
                if (line_string.find("\t") == std::string::npos) {
                    this->delimiter = ' ';
                } else {
                    this->delimiter = '\t';
                }
            } else {
                this->delimiter = ',';
            }
        }

        // 1-line vector
        std::vector<double> line_vector;
        // 1-line stringstream
        std::istringstream line_stream(line_string);
        // 1-value string
        std::string value_string;
        // get 1-value string from 1-line stream
        while (std::getline(line_stream, value_string, this->delimiter)) {
            // cast 1-value string to double, then add it to 1-line vector
            line_vector.push_back(std::stod(value_string));
        }

        // if 1-line vector has elements
        if (line_vector.size()) {
            // add 1-line vector to table
            table.push_back(line_vector);
        }
    }

    input_file.close();
    return;
}
