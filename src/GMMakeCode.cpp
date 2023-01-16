#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
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
    for (int i = 0; i < tables.size(); i++) {
        // get xmin and xmax
        std::vector<double> x;
        for (int j = 0; j < tables.at(i).size(); j++) {
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
        for (int k = 1; k < tables.at(i).at(0).size(); k++) {
            std::vector<double> y;
            for (int j = 0; j < tables.at(i).size(); j++) {
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

void GMMakeCode::resetXMinMax(const bool F_xmin, const double xmin_entered, const bool F_xmax, const double xmax_entered, const double extension) {
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

void GMMakeCode::resetYMinMax(const bool F_ymin, const double ymin_entered, const bool F_ymax, const double ymax_entered, const double extension) {
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

int GMMakeCode::writeScale(std::ofstream& texfile) {
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
        texfile << "    \\node[below] at (" << this->xPosition(x) << ", 0) {" << x << "};" << std::endl;
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
        texfile << "    \\node[left] at (0, " << this->yPosition(y) << ") {" << y << "};" << std::endl;
        texfile << "    \\draw[thick] (0, " << this->yPosition(y) << ") -- (0.2, " << this->yPosition(y) << ");" << std::endl;
        texfile << "    \\draw[thick] (" << this->width << ", " << this->yPosition(y) << ") -- (" << this->width - 0.2 << ", " << this->yPosition(y) << ");" << std::endl;
    }
    texfile << std::endl;

    return 0;
}

void GMMakeCode::writeData(std::ofstream& texfile, const std::vector<std::vector<double>>& table, const std::string color, const bool F_with_line, const bool F_with_error) {
    if (!F_with_line) {
        for (int i = 0; i < table.size(); i++) {
            if ((table.at(i).at(0)>(this->xmin)) && (table.at(i).at(0)<(this->xmax)) && (table.at(i).at(1)>(this->ymin)) && (table.at(i).at(1)<(this->ymax))) {
                texfile << "    \\fill[" << color << "] (" << this->xPosition(table.at(i).at(0)) << ", " << this->yPosition(table.at(i).at(1)) << ") circle [radius=1pt];" << std::endl;
            }
        }
    } else {
        texfile << "    \\draw[" << color << ", line width=1pt]";
        int line_count = 0;
        for (int i = 0; i < table.size(); i++) {
            if ((table.at(i).at(0)>=(this->xmin)) && (table.at(i).at(0)<(this->xmax)) && (table.at(i).at(1)>=(this->ymin)) && (table.at(i).at(1)<(this->ymax))) {
                if (line_count > 0) {
                    texfile << " --";
                }
                texfile << " (" << this->xPosition(table.at(i).at(0)) << ", " << this->yPosition(table.at(i).at(1)) << ")";
                line_count++;
                if ((line_count%5==4) && (i!=table.size()-1)) {
                    texfile << " %" << std::endl;
                    texfile << "       ";
                }
            }
        }
        texfile << ";" << std::endl;
    }
    if (F_with_error) {
        for (int i = 0; i < table.size(); i++) {
            if ((table.at(i).at(0)>(this->xmin)) && (table.at(i).at(0)<(this->xmax)) && (table.at(i).at(1)>(this->ymin)) && (table.at(i).at(1)<(this->ymax)) && (table.at(i).size()>=3)) {
                double x = table.at(i).at(0);
                double y = table.at(i).at(1);
                double ey = table.at(i).at(2);
                texfile << "    \\draw[thick] (" << this->xPosition(x) - 0.1 << ", " << this->yPosition(y+ey) << ") -- (" << this->xPosition(x) + 0.1 << ", " << this->yPosition(y+ey) << ");" << std::endl;
                texfile << "    \\draw[thick] (" << this->xPosition(x) - 0.1 << ", " << this->yPosition(y-ey) << ") -- (" << this->xPosition(x) + 0.1 << ", " << this->yPosition(y-ey) << ");" << std::endl;
                texfile << "    \\draw[thick] (" << this->xPosition(x) << ", " << this->yPosition(y+ey) << ") -- (" << this->xPosition(x) << ", " << this->yPosition(y-ey) << ");" << std::endl;
            }
        }
    }
    texfile << std::endl;
    return;
}

void GMMakeCode::writeLegend(std::ofstream& texfile, const std::string title, const std::string color, const bool F_with_line) {
    double y_position = this->height - 0.8 - 0.6 * this->legend_count;
    texfile << "    \\node[left] at (" << this->width - 1.4 << ", " << y_position << ") {" << title << "};" << std::endl;
    if (!F_with_line) {
        texfile << "    \\fill[" << color << "] (" << this->width - 1 << ", " << y_position << ") circle [radius=1pt];" << std::endl;
    } else {
        texfile << "    \\draw[" << color << ", line width=1pt] (" << this->width - 1.2 << ", " << y_position << ") -- (" << this->width - 0.8 << ", " << y_position << ");" << std::endl;
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
