#include <string>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include "GMArgProc.hpp"

GMArgProc::source::source() {
    this->color = "blue";
    this->color2 = "";
    this->F_with_line = false;
    this->F_with_error = false;
    this->F_with_linear = false;
}

GMArgProc::GMArgProc() {
    this->ofname = "";
    this->xlabel = "";
    this->ylabel = "";
    this->xmin = 0;
    this->xmax = 0;
    this->ymin = 0;
    this->ymax = 0;

    this->F_xmin = false;
    this->F_xmax = false;
    this->F_ymin = false;
    this->F_ymax = false;
    this->F_tex = false;
    this->F_logx = false;
    this->F_logy = false;

    this->texname = "";
    this->dviname = "";
    this->pdfname = "";
    this->auxname = "";
    this->logname = "";
}

void GMArgProc::setArguments(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        this->arguments.push_back(argv[i]);
    }
    return;
}

int GMArgProc::process() {
    // processing when displaying version and help
    if (this->arguments.size() == 1) {
        // version
        if (this->arguments.at(0) == "--version") {
            this->version();
            return 2;
        }
        // help
        if (this->arguments.at(0) == "--help") {
            this->help();
            return 2;
        }
    }

    // processing when making graph
    int error_code = 0;
    for (int i = 0; i < (this->arguments).size(); i++) {
        // get option
        error_code = this->getOption(i);
        if (error_code == 0) {
            continue;
        } else if (error_code == 1) {
            return 1;
        }

        // get source
        error_code = this->getSource(i);
        if (error_code == 1) {
            return 1;
        }
    }

    // if no input file, return 1
    if (this->sources.size() == 0) {
        std::cerr << "No input file" << std::endl;
        std::cerr << "Command aborted\n" << std::endl;
        return 1;
    }

    // set output file name
    error_code = setOutputFile();
    if (error_code == 1) {
        return 1;
    }

    return 0;
}

void GMArgProc::version() {
    std::cout << "Graph Maker" << std::endl;
    std::cout << "Copyright (C) 2023 tar2chicken\n" << std::endl;
    return;
}

void GMArgProc::help() {
    std::cout << "Usage: graphmaker [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --version     display version" << std::endl;
    std::cout << "  --help        display this help\n" << std::endl;
    return;
}

int GMArgProc::getSource(int& i) {
    GMArgProc::source src;

    // get file name, if no such file, return 1
    if (std::filesystem::exists(this->arguments.at(i))) {
        src.file_name = this->arguments.at(i);
        src.title = this->arguments.at(i);
        if (src.title.rfind(".") != std::string::npos) {
            src.title.erase(src.title.rfind("."));
        }
    } else {
        std::cerr << this->arguments.at(i) << ": No such file" << std::endl;
        std::cerr << "Command aborted\n" << std::endl;
        return 1;
    }

    // get optional properties of source
    // [file_name]  -c  [color]  -t  [title]  -l  -e  -c2  [color]  (arguments.size)
    //      i       i+1   i+2    i+3   i+4    i+5 i+6 i+7    i+8          i+9
    int j = i;
    while (j+1 < std::min(i+9, static_cast<int>(this->arguments.size()))) {
        // get color, if no color, return 1
        if (this->arguments.at(j+1) == "-c") {
            j++;
            if (j+1 < (this->arguments).size()) {
                src.color = this->arguments.at(j+1);
                j++;
                continue;
            } else {
                std::cerr << "No color" << std::endl;
                std::cerr << "Command aborted\n" << std::endl;
                return 1;
            }
        }

        // get second color, if no color, return 1
        if (this->arguments.at(j+1) == "-c2") {
            j++;
            if (j+1 < (this->arguments).size()) {
                src.color2 = this->arguments.at(j+1);
                j++;
                continue;
            } else {
                std::cerr << "No color" << std::endl;
                std::cerr << "Command aborted\n" << std::endl;
                return 1;
            }
        }

        // get title, if no title, return 1
        if (this->arguments.at(j+1) == "-t") {
            j++;
            if (j+1 < (this->arguments).size()) {
                src.title = this->arguments.at(j+1);
                j++;
                continue;
            } else {
                std::cerr << "No title" << std::endl;
                std::cerr << "Command aborted\n" << std::endl;
                return 1;
            }
        }

        // get with_line
        if (this->arguments.at(j+1) == "-l") {
            src.F_with_line = true;
            j++;
            continue;
        }

        // get with_error
        if (this->arguments.at(j+1) == "-e") {
            src.F_with_error = true;
            j++;
            continue;
        }

        // get with_linear
        if (this->arguments.at(j+1) == "-linear") {
            src.F_with_linear = true;
            j++;
            continue;
        }

        // if no optional properties, break
        break;
    }

    // add src to sources
    this->sources.push_back(src);

    //increment i
    i = j;
    return 0;
}

int GMArgProc::getOption(int& i) {
    // get output file name, if no file name, return 1
    if (this->arguments.at(i) == "-o") {
        if (i+1 < (this->arguments).size()) {
            this->ofname = this->arguments.at(i+1);
            i++;
            return 0;
        } else {
            std::cerr << "No output file name" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
    }

    // get x-axis label, if no label, return 1
    if (this->arguments.at(i) == "-x") {
        if (i+1 < (this->arguments).size()) {
            this->xlabel = this->arguments.at(i+1);
            i++;
            return 0;
        } else {
            std::cerr << "No x-axis label" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
    }

    // get y-axis label, if no label, return 1
    if (this->arguments.at(i) == "-y") {
        if (i+1 < (this->arguments).size()) {
            this->ylabel = this->arguments.at(i+1);
            i++;
            return 0;
        } else {
            std::cerr << "No y-axis label" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
    }

    // get minimum value of x-axis, if no value, return 1
    if (this->arguments.at(i) == "-xmin") {
        if (i+1 < (this->arguments).size()) {
            this->xmin = std::stod(this->arguments.at(i+1));
            this->F_xmin = true;
            i++;
            return 0;
        } else {
            std::cerr << "No minimum value of x-axis" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
    }

    // get maximum value of x-axis, if no value, return 1
    if (this->arguments.at(i) == "-xmax") {
        if (i+1 < (this->arguments).size()) {
            this->xmax = std::stod(this->arguments.at(i+1));
            this->F_xmax = true;
            i++;
            return 0;
        } else {
            std::cerr << "No maximum value of x-axis" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
    }

    // get minimum value of y-axis, if no value, return 1
    if (this->arguments.at(i) == "-ymin") {
        if (i+1 < (this->arguments).size()) {
            this->ymin = std::stod(this->arguments.at(i+1));
            this->F_ymin = true;
            i++;
            return 0;
        } else {
            std::cerr << "No minimum value of y-axis" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
    }

    // get maximum value of y-axis, if no value, return 1
    if (this->arguments.at(i) == "-ymax") {
        if (i+1 < (this->arguments).size()) {
            this->ymax = std::stod(this->arguments.at(i+1));
            this->F_ymax = true;
            i++;
            return 0;
        } else {
            std::cerr << "No maximum value of y-axis" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
    }

    // get flag of tex file output
    if (this->arguments.at(i) == "-tex") {
        this->F_tex = true;
        return 0;
    }

    // get flag of log plot of x axis
    if (this->arguments.at(i) == "-logx") {
        this->F_logx = true;
        return 0;
    }

    // get flag of log plot of y axis
    if (this->arguments.at(i) == "-logy") {
        this->F_logy = true;
        return 0;
    }

    return 2;
}

int GMArgProc::setOutputFile() {
    // if no input file, return 1
    if (this->sources.size() == 0) {
        std::cerr << "No input file" << std::endl;
        std::cerr << "Command aborted\n" << std::endl;
        return 1;
    }

    // if no output file name, use file name of first source
    if (this->ofname == "") {
        this->ofname = this->sources.at(0).file_name;
    }

    // if ofname has extension, erase extension
    if (this->ofname.rfind(".") != std::string::npos) {
        this->ofname.erase(this->ofname.rfind("."));
    }

    // set names of output files
    this->texname = this->ofname + ".tex";
    this->dviname = this->ofname + ".dvi";
    this->pdfname = this->ofname + ".pdf";
    this->auxname = this->ofname + ".aux";
    this->logname = this->ofname + ".log";

    return 0;
}
