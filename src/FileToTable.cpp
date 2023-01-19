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
        if (line_string.find(",") == std::string::npos) {
            if (line_string.find("\t") == std::string::npos) {
                this->delimiter = ' ';
            } else {
                this->delimiter = '\t';
            }
        } else {
            this->delimiter = ',';
        }

        // 1-line vector
        std::vector<double> line_vector;
        // 1-line stringstream
        std::istringstream line_stream(line_string);
        // 1-value string
        std::string value_string;
        // flag of exception
        bool F_exception = false;
        // get 1-value string from 1-line stream
        while (std::getline(line_stream, value_string, this->delimiter)) {
            if (this->delimiter==' ' || this->delimiter=='\t') {
                if (value_string == "") {
                    continue;
                }
            } else {
                if (value_string == "") {
                    F_exception = true;
                    break;
                }
            }
            double value;
            try {
                value = std::stod(value_string);
            }
            catch (...) {
                F_exception = true;
                break;
            }
            // cast 1-value string to double, then add it to 1-line vector
            line_vector.push_back(value);
        }

        // flag of first line
        bool F_first_line = true;
        // column number
        int column_number;
        // if 1-line vector has elements and the line has no string, add 1-line vector to table
        if (line_vector.size() && !F_exception) {
            if (F_first_line) {
                F_first_line = false;
                column_number = line_vector.size();
                table.push_back(line_vector);
            } else if (line_vector.size() == column_number) {
                // if the number of elements in 1-line vector is equal to the number of columns
                table.push_back(line_vector);
            }
        }
    }

    input_file.close();
    return;
}
