#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <regex>
#include "GMMakeCode.hpp"

GMMakeCode::GMMakeCode() {
    this->xmin = 0;
    this->xmax = 0;
    this->ymin = 0;
    this->ymax = 0;

    this->dx = 1;
    this->dy = 1;
    this->xscale_count = 1;
    this->yscale_count = 1;

    this->legend_count = 0;
}

void GMMakeCode::getMinMax(const std::vector<std::vector<std::vector<double>>>& tables) {
    for (int i = 0; i < static_cast<int>(tables.size()); i++) {
        // get xmin and xmax
        std::vector<double> x;
        for (int j = 0; j < static_cast<int>(tables.at(i).size()); j++) {
            x.push_back(tables.at(i).at(j).at(0));
        }
        if (i == 0) {
            this->xmin = *(std::min_element(x.begin(), x.end()));
            this->xmax = *(std::max_element(x.begin(), x.end()));
        } else {
            this->xmin = std::min(this->xmin, *(std::min_element(x.begin(), x.end())));
            this->xmax = std::max(this->xmax, *(std::max_element(x.begin(), x.end())));
        }

        // get ymin and ymax
        for (int k = 1; k < static_cast<int>(tables.at(i).at(0).size()); k++) {
            std::vector<double> y;
            for (int j = 0; j < static_cast<int>(tables.at(i).size()); j++) {
                y.push_back(tables.at(i).at(j).at(k));
            }
            if (i==0 && k==1) {
                this->ymin = *(std::min_element(y.begin(), y.end()));
                this->ymax = *(std::max_element(y.begin(), y.end()));
            } else {
                this->ymin = std::min(this->ymin, *(std::min_element(y.begin(), y.end())));
                this->ymax = std::max(this->ymax, *(std::max_element(y.begin(), y.end())));
            }
        }
    }
    return;
}

void GMMakeCode::resetXMinMax(const bool F_xmin, double xmin_entered, const bool F_xmax, double xmax_entered, const double extension, const bool F_logx) {
    if (F_logx) {
        xmin_entered = std::log10(xmin_entered);
        xmax_entered = std::log10(xmax_entered);
    }
    if (F_xmin) {
        this->xmin = xmin_entered;
    }
    if (F_xmax) {
        this->xmax = xmax_entered;
    } else {
        this->xmax = this->xmin + (this->xmax - this->xmin)*extension;
    }
    return;
}

void GMMakeCode::resetYMinMax(const bool F_ymin, double ymin_entered, const bool F_ymax, double ymax_entered, const double extension, const bool F_logy) {
    if (F_logy) {
        ymin_entered = std::log10(ymin_entered);
        ymax_entered = std::log10(ymax_entered);
    }
    if (F_ymin) {
        this->ymin = ymin_entered;
    }
    if (F_ymax) {
        this->ymax = ymax_entered;
    } else {
        this->ymax = this->ymin + (this->ymax - this->ymin)*extension;
    }
    return;
}

void GMMakeCode::writePreamble(std::ofstream& texfile) {
    texfile << "\\documentclass[uplatex, dvipdfmx]{jsarticle}" << std::endl;
    texfile << "\\usepackage{amsmath, amsfonts, amssymb}" << std::endl;
    texfile << "\\usepackage{bm}" << std::endl;
    texfile << "\\usepackage[version=4]{mhchem}" << std::endl;
    texfile << "\\usepackage{tikz}" << std::endl;
    texfile << "\\usepackage{siunitx}" << std::endl;
    texfile << "\\usepackage{physics}" << std::endl;
    texfile << "\\pagestyle{empty}\n" << std::endl;
    texfile << "\\begin{document}" << std::endl;
    texfile << "\\begin{tikzpicture}" << std::endl;
    return;
}

int GMMakeCode::writeScale(std::ofstream& texfile, const bool F_logx, const bool F_logy) {
    // set dx
    this->dx = this->scale(this->xmin, this->xmax);
    if (this->dx < 0) {
        return 1;
    }
    // set xscale_count
    this->xscale_count = std::ceil(this->xmax / this->dx) - std::ceil(this->xmin / this->dx);
    // write x-axis scale
    for (int i = 0; i < xscale_count; i++) {
        double x = (std::ceil(this->xmin / this->dx) + i) * this->dx;
        if (!F_logx) {
            texfile << "    \\node[below] at (" << this->xPosition(x) << ", 0) {" << x << "};" << std::endl;
        } else {
            texfile << "    \\node[below] at (" << this->xPosition(x) << ", 0) {$10^{" << x << "}$};" << std::endl;
        }
        texfile << "    \\draw[thick] (" << this->xPosition(x) << ", 0) -- (" << this->xPosition(x) << ", 0.2);" << std::endl;
        texfile << "    \\draw[thick] (" << this->xPosition(x) << ", " << this->height << ") -- (" << this->xPosition(x) << ", " << this->height - 0.2 << ");" << std::endl;
    }
    texfile << std::endl;

    // set dy
    this->dy = this->scale(this->ymin, this->ymax);
    if (this->dy < 0) {
        return 1;
    }
    // set yscale_count
    this->yscale_count = std::ceil(this->ymax / this->dy) - std::ceil(this->ymin / this->dy);
    // write x-axis scale
    for (int i = 0; i < yscale_count; i++) {
        double y = (std::ceil(this->ymin / this->dy) + i) * this->dy;
        if (!F_logy) {
            texfile << "    \\node[left] at (0, " << this->yPosition(y) << ") {" << y << "};" << std::endl;
        } else {
            texfile << "    \\node[left] at (0, " << this->yPosition(y) << ") {$10^{" << y << "}$};" << std::endl;
        }
        texfile << "    \\draw[thick] (0, " << this->yPosition(y) << ") -- (0.2, " << this->yPosition(y) << ");" << std::endl;
        texfile << "    \\draw[thick] (" << this->width << ", " << this->yPosition(y) << ") -- (" << this->width - 0.2 << ", " << this->yPosition(y) << ");" << std::endl;
    }
    texfile << std::endl;

    return 0;
}

void GMMakeCode::writeData(std::ofstream& texfile, const std::vector<std::vector<double>>& table, const int column, const std::string color, const bool F_with_line, const bool F_with_error, const bool F_with_linear) {
    if (F_with_linear) {
        double a, b;
        this->linearRegression(table, column, a, b);
        if ((a*this->xmin+b)>=(this->ymin) && (a*this->xmin+b)<=(this->ymax)) {
            if ((a*this->xmax+b) < (this->ymin)) {
                texfile << "    \\draw[" << color << "!50!white, line width=1pt] (" << this->xPosition(this->xmin) << ", " << this->yPosition(a*this->xmin+b) << ") -- (" << this->xPosition((this->ymin-b)/a) << ", " << this->yPosition(this->ymin) << ");" << std::endl;
            } else if ((a*this->xmax+b) > (this->ymax)) {
                texfile << "    \\draw[" << color << "!50!white, line width=1pt] (" << this->xPosition(this->xmin) << ", " << this->yPosition(a*this->xmin+b) << ") -- (" << this->xPosition((this->ymax-b)/a) << ", " << this->yPosition(this->ymax) << ");" << std::endl;
            } else {
                texfile << "    \\draw[" << color << "!50!white, line width=1pt] (" << this->xPosition(this->xmin) << ", " << this->yPosition(a*this->xmin+b) << ") -- (" << this->xPosition(this->xmax) << ", " << this->yPosition(a*this->xmax+b) << ");" << std::endl;
            }
        } else if ((a*this->xmin+b)<(this->ymin) && (a*this->xmax+b)<=(this->ymax)) {
            texfile << "    \\draw[" << color << "!50!white, line width=1pt] (" << this->xPosition((this->ymin-b)/a) << ", " << this->yPosition(this->ymin) << ") -- (" << this->xPosition(this->xmax) << ", " << this->yPosition(a*this->xmax+b) << ");" << std::endl;
        } else if ((a*this->xmin+b)>(this->ymax) && (a*this->xmax+b)>=(this->ymin)) {
            texfile << "    \\draw[" << color << "!50!white, line width=1pt] (" << this->xPosition((this->ymax-b)/a) << ", " << this->yPosition(this->ymax) << ") -- (" << this->xPosition(this->xmax) << ", " << this->yPosition(a*this->xmax+b) << ");" << std::endl;
        } else {
            texfile << "    \\draw[" << color << "!50!white, line width=1pt] (" << this->xPosition((this->ymin-b)/a) << ", " << this->yPosition(this->ymin) << ") -- (" << this->xPosition((this->ymax-b)/a) << ", " << this->yPosition(this->ymax) << ");" << std::endl;
        }
    }
    if (F_with_error) {
        for (int i = 0; i < static_cast<int>(table.size()); i++) {
            if ((table.at(i).at(0)>(this->xmin)) && (table.at(i).at(0)<(this->xmax)) && (table.at(i).at(column)>(this->ymin)) && (table.at(i).at(column)<(this->ymax)) && (column+1 < static_cast<int>(table.at(i).size()))) {
                double x = table.at(i).at(0);
                double y = table.at(i).at(column);
                double ey = table.at(i).at(column+1);
                texfile << "    \\draw[thick] (" << this->xPosition(x) - 0.05 << ", " << this->yPosition(y+ey) << ") -- (" << this->xPosition(x) + 0.05 << ", " << this->yPosition(y+ey) << ");" << std::endl;
                texfile << "    \\draw[thick] (" << this->xPosition(x) - 0.05 << ", " << this->yPosition(y-ey) << ") -- (" << this->xPosition(x) + 0.05 << ", " << this->yPosition(y-ey) << ");" << std::endl;
                texfile << "    \\draw[thick] (" << this->xPosition(x) << ", " << this->yPosition(y+ey) << ") -- (" << this->xPosition(x) << ", " << this->yPosition(y-ey) << ");" << std::endl;
            }
        }
    }
    if (!F_with_line) {
        for (int i = 0; i < static_cast<int>(table.size()); i++) {
            if ((table.at(i).at(0)>(this->xmin)) && (table.at(i).at(0)<(this->xmax)) && (table.at(i).at(column)>(this->ymin)) && (table.at(i).at(column)<(this->ymax))) {
                texfile << "    \\fill[" << color << "] (" << this->xPosition(table.at(i).at(0)) << ", " << this->yPosition(table.at(i).at(column)) << ") circle [radius=1pt];" << std::endl;
            }
        }
    } else {
        texfile << "    \\draw[" << color << ", line width=1pt]";
        int line_count = 0;
        for (int i = 0; i < static_cast<int>(table.size()); i++) {
            if ((table.at(i).at(0)>=(this->xmin)) && (table.at(i).at(0)<(this->xmax)) && (table.at(i).at(column)>=(this->ymin)) && (table.at(i).at(column)<(this->ymax))) {
                if (line_count > 0) {
                    texfile << " --";
                }
                texfile << " (" << this->xPosition(table.at(i).at(0)) << ", " << this->yPosition(table.at(i).at(column)) << ")";
                line_count++;
                if ((line_count%5==4) && (i!=(static_cast<int>(table.size()))-1)) {
                    texfile << " %" << std::endl;
                    texfile << "       ";
                }
            }
        }
        texfile << ";" << std::endl;
    }
    texfile << std::endl;
    return;
}

void GMMakeCode::makeTitle(std::vector<std::string>& title_list, std::string title, const int data_number, const std::string file_name) {
    if (title != "-") {
        for (int i = 0; i < data_number; i++) {
            title_list.push_back(title + " (" + std::to_string(i+1) + ")");
        }
    } else {
        std::ifstream input_file(file_name);
        std::string line_string;
        while (std::getline(input_file, line_string)) {
            char delimiter;
            if (line_string.find(",") == std::string::npos) {
                if (line_string.find("\t") == std::string::npos) {
                    delimiter = ' ';
                } else {
                    delimiter = '\t';
                }
            } else {
                delimiter = ',';
            }

            std::istringstream line_stream(line_string);
            std::string field;
            std::getline(line_stream, field, delimiter);
            if (delimiter==' ' || delimiter=='\t') {
                while (field == "") {
                    std::getline(line_stream, field, delimiter);
                }
            } else if (field == "") {
                continue;
            }
            field = std::regex_replace(field, std::regex(" "), "");
            field = std::regex_replace(field, std::regex("\t"), "");
            if (field == "-t") {
                std::vector<std::string> line_vector;
                bool F_empty_field = false;
                while (std::getline(line_stream, field, delimiter)) {
                    if ((delimiter==' ' || delimiter=='\t') && field=="") {
                        continue;
                    } else if (field == "") {
                        F_empty_field = true;
                        break;
                    }

                    // Remove space and tab before and after field
                    std::regex s1("^ ");
                    std::regex s2(" $");
                    std::regex t1("^\t");
                    std::regex t2("\t$");
                    while (std::regex_search(field, s1) || std::regex_search(field, s2) || std::regex_search(field, t1) || std::regex_search(field, t2)) {
                        field = std::regex_replace(field, s1, "");
                        field = std::regex_replace(field, s2, "");
                        field = std::regex_replace(field, t1, "");
                        field = std::regex_replace(field, t2, "");
                    }

                    line_vector.push_back(field);
                }

                if (!F_empty_field && static_cast<int>(line_vector.size())==data_number) {
                    for (int i = 0; i < data_number; i++) {
                        title_list.push_back(line_vector.at(i));
                    }
                }
            } else {
                continue;
            }
        }

        if (title_list.size() == 0) {
            title = file_name;
            if (title.rfind(".") != std::string::npos) {
                title.erase(title.rfind("."));
            }
            for (int i = 0; i < data_number; i++) {
                title_list.push_back(title + " (" + std::to_string(i+1) + ")");
            }
        }
    }
    return;
}

void GMMakeCode::makeColor(std::vector<std::string>& color_list, const std::string color, const std::string color2, const int data_number) {
    if (data_number == 1) {
        color_list.push_back(color);
    } else if (color2 != "") {
        for (int i = 0; i < data_number; i++) {
            double ratio = i * 100 / (data_number-1);
            color_list.push_back(color2 + "!" + std::to_string(ratio) + "!" + color);
        }
    } else if (data_number == 2) {
        color_list.push_back("blue");
        color_list.push_back("red");
    } else if (data_number == 3) {
        color_list.push_back("blue");
        color_list.push_back("green!50!black");
        color_list.push_back("red");
    } else if (data_number == 4) {
        color_list.push_back("blue");
        color_list.push_back("green!50!black");
        color_list.push_back("orange");
        color_list.push_back("red");
    } else if (data_number == 5) {
        color_list.push_back("blue!50!red");
        color_list.push_back("blue");
        color_list.push_back("green!50!black");
        color_list.push_back("orange");
        color_list.push_back("red");
    } else if (data_number == 6) {
        color_list.push_back("blue!50!red");
        color_list.push_back("blue");
        color_list.push_back("green!50!black");
        color_list.push_back("yellow!50!orange");
        color_list.push_back("orange");
        color_list.push_back("red");
    } else if (data_number == 7) {
        color_list.push_back("blue!50!red");
        color_list.push_back("blue");
        color_list.push_back("blue!50!white");
        color_list.push_back("green!50!black");
        color_list.push_back("yellow!50!orange");
        color_list.push_back("orange");
        color_list.push_back("red");
    } else if (data_number == 8) {
        color_list.push_back("blue!50!red");
        color_list.push_back("blue");
        color_list.push_back("blue!50!white");
        color_list.push_back("green!50!black");
        color_list.push_back("green!50!orange");
        color_list.push_back("yellow!50!orange");
        color_list.push_back("orange");
        color_list.push_back("red");
    } else {
        color_list.push_back("blue!50!red");
        color_list.push_back("blue");
        color_list.push_back("blue!50!white");
        color_list.push_back("green!50!black");
        color_list.push_back("green!50!orange");
        color_list.push_back("yellow!50!orange");
        color_list.push_back("orange");
        color_list.push_back("red");
        for (int i = 0; i < data_number-8; i++) {
            color_list.push_back("black");
        }
    }
    return;
}

std::string GMMakeCode::makeFormula(std::vector<std::vector<double>>& table, const bool F_logx, const bool F_logy) {
    double a, b;
    this->linearRegression(table, 1, a, b);
    std::ostringstream title;
    title << std::setprecision(4);
    if (!F_logx && !F_logy) {
        if (b > 0) {
            title << "$ y = " << a << "x + " << b << " $";
        } else if (b == 0) {
            title << "$ y = " << a << "x $";
        } else {
            title << "$ y = " << a << "x " << b << " $";
        }
    } else if (!F_logx) {
        if (b > 0) {
            title << "$ y = \\exp (" << a * std::log(10) << "x + " << b * std::log(10) << ") $";
        } else if (b == 0) {
            title << "$ y = \\exp (" << a * std::log(10) << "x) $";
        } else {
            title << "$ y = \\exp (" << a * std::log(10) << "x " << b * std::log(10) << ") $";
        }
    } else if (!F_logy) {
        if (b > 0) {
            title << "$ y = " << a / std::log(10) << " \\log x + " << b << " $";
        } else if (b == 0) {
            title << "$ y = " << a / std::log(10) << " \\log x $";
        } else {
            title << "$ y = " << a / std::log(10) << " \\log x " << b << " $";
        }
    } else {
        if (b != 0) {
            title << "$ y = x^{" << a << "} 10^{" << b << "} $";
        } else {
            title << "$ y = x^{" << a << "} $";
        }
    }
    return title.str();
}

void GMMakeCode::writeLegend(std::ofstream& texfile, std::string title, const std::string color, const bool F_with_line, const bool F_left) {
    title = std::regex_replace(title, std::regex("_"), "\\_");
    double y_position = this->height - 0.8 - 0.6 * this->legend_count;
    if (!F_left) {
        texfile << "    \\node[left] at (" << this->width - 1.4 << ", " << y_position << ") {" << title << "};" << std::endl;
        if (!F_with_line) {
            texfile << "    \\fill[" << color << "] (" << this->width - 1 << ", " << y_position << ") circle [radius=1pt];" << std::endl;
        } else {
            texfile << "    \\draw[" << color << ", line width=1pt] (" << this->width - 1.2 << ", " << y_position << ") -- (" << this->width - 0.8 << ", " << y_position << ");" << std::endl;
        }
    } else {
        texfile << "    \\node[right] at (" << 1.4 << ", " << y_position << ") {" << title << "};" << std::endl;
        if (!F_with_line) {
            texfile << "    \\fill[" << color << "] (" << 1 << ", " << y_position << ") circle [radius=1pt];" << std::endl;
        } else {
            texfile << "    \\draw[" << color << ", line width=1pt] (" << 0.8 << ", " << y_position << ") -- (" << 1.2 << ", " << y_position << ");" << std::endl;
        }
    }
    this->legend_count++;
    return;
}

void GMMakeCode::writeFrame(std::ofstream& texfile, const std::string xlabel, const std::string ylabel) {
    texfile << "    \\draw[line width=1pt] (0, 0) -- (" << this->width << ", 0) -- (" << this->width << ", " << this->height << ") -- (0, " << this->height << ") -- cycle;" << std::endl;
    if (xlabel != "") {
        texfile << "    \\node at (" << this->width / 2 << ", -1) {" << xlabel << "};" << std::endl;
    }
    if (ylabel != "") {
        texfile << "    \\node[rotate=90] at (-1.5, " << this->height / 2 << ") {" << ylabel << "};" << std::endl;
    }
    texfile << "\\end{tikzpicture}" << std::endl;
    texfile << "\\end{document}" << std::endl;
    return;
}

double GMMakeCode::scale(const double min_value, const double max_value) {
    // define a, b as max-min = a * 10^b
    double a = max_value - min_value;
    if (a <= 0) {
        std::cerr << "scale: Invalid arguments" << std::endl;
        std::cerr << "Command aborted\n" << std::endl;
        return -1;
    }
    // define a, b as max-min = a * 10^b
    int b = 0;

    // set a, b
    while (a >= 10) {
        a /= 10;
        b++;
    }
    while (a < 1) {
        a *= 10;
        b--;
    }

    // set scale
    if (a == 1) {
        return (0.1 * std::pow(10, b));
    } else if (a>1 && a<=2) {
        return (0.2 * std::pow(10, b));
    } else if (a>2 && a<=4) {
        return (0.4 * std::pow(10, b));
    } else if (a>4 && a<=5) {
        return (0.5 * std::pow(10, b));
    } else if (a>5 && a<10) {
        return std::pow(10, b);
    }

    return -1;
}

double GMMakeCode::xPosition(const double x) {
    return (this->width * (x - this->xmin) / (this->xmax - this->xmin));
}

double GMMakeCode::yPosition(const double y) {
    return (this->height * (y - this->ymin) / (this->ymax - this->ymin));
}

void GMMakeCode::linearRegression(const std::vector<std::vector<double>>& table, const int column, double& a, double& b) {
    double X = 0;
    double Y = 0;
    double X2 = 0;
    double XY = 0;
    int N = table.size();
    for (int i = 0; i < static_cast<int>(table.size()); i++) {
        X += table.at(i).at(0);
        Y += table.at(i).at(column);
        X2 += std::pow(table.at(i).at(0), 2);
        XY += table.at(i).at(0)*table.at(i).at(column);
    }
    a = (N*XY - X*Y) / (N*X2 - std::pow(X, 2));
    b = (X2*Y - XY*X) / (N*X2 - std::pow(X, 2));
    return;
}
