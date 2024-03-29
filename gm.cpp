#include <cstdlib>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include "src/FileToTable.hpp"
#include "src/GMArgProc.hpp"
#include "src/GMMakeCode.hpp"

/// @brief make graph from date file
/// @param argc number of arguments
/// @param argv array of arguments
/// @return 0:OK, 1:ERROR
int main(int argc, char** argv) {
    // process command line arguments
    GMArgProc gmap;
    gmap.setArguments(argc, argv);
    int error_code = gmap.process();
    if (error_code == 1) {
        return 1;
    } else if (error_code == 2) {
        return 0;
    }

    // get data from input file
    FileToTable ftt;
    // vector of table
    std::vector<std::vector<std::vector<double>>> tables;
    for (int i = 0; i < gmap.sources.size(); i++) {
        std::vector<std::vector<double>> table;
        ftt.numeric(gmap.sources.at(i).file_name, table);

        if (gmap.F_logx) {
            for (int j = 0; j < table.size(); j++) {
                while (table.at(j).at(0) <= 0) {
                    table.erase(table.begin()+j);
                    if (j == table.size()) {
                        break;
                    }
                }
                if (j < table.size()){
                    table.at(j).at(0) = std::log10(table.at(j).at(0));
                }
            }
        }

        if (gmap.F_logy) {
            for (int j = 0; j < table.size(); j++) {
                for (int column = 1; column < table.at(j).size(); column++) {
                    while (table.at(j).at(column) <= 0) {
                        table.erase(table.begin()+j);
                        if (j == table.size()) {
                            break;
                        }
                    }
                    if (j < table.size()){
                        table.at(j).at(column) = std::log10(table.at(j).at(column));
                    } else {
                        break;
                    }
                }
            }
        }

        if (table.size()<2) {
            std::cerr << "Data cannot be graphed" << std::endl;
            std::cerr << "Command aborted\n" << std::endl;
            return 1;
        }
        tables.push_back(table);
    }

    // make tex source code
    GMMakeCode gmmc;
    // get minimum and maximum values of each axis
    gmmc.getMinMax(tables);
    gmmc.resetXMinMax(gmap.F_xmin, gmap.xmin, gmap.F_xmax, gmap.xmax, 1.05, gmap.F_logx);
    gmmc.resetYMinMax(gmap.F_ymin, gmap.ymin, gmap.F_ymax, gmap.ymax, 1.1, gmap.F_logy);
    // output tex source code
    std::ofstream texfile(gmap.texname);
    gmmc.writePreamble(texfile);
    error_code = gmmc.writeScale(texfile, gmap.F_logx, gmap.F_logy);
    if (error_code == 1) {
        return 1;
    }

    // output data
    for (int i = 0; i < gmap.sources.size(); i++) {
        if (!gmap.sources.at(i).F_with_error && !gmap.sources.at(i).F_with_linear) {
            std::vector<std::string> color_list;
            gmmc.makeColor(color_list, gmap.sources.at(i).color, gmap.sources.at(i).color2, tables.at(i).at(0).size()-1);
            for (int column = 1; column < tables.at(i).at(0).size(); column++) {
                gmmc.writeData(texfile, tables.at(i), column, color_list.at(column-1), gmap.sources.at(i).F_with_line, gmap.sources.at(i).F_with_error, gmap.sources.at(i).F_with_linear);
            }
        } else {
            gmmc.writeData(texfile, tables.at(i), 1, gmap.sources.at(i).color, gmap.sources.at(i).F_with_line, gmap.sources.at(i).F_with_error, gmap.sources.at(i).F_with_linear);
        }
    }

    // output graph legend
    if (gmap.sources.size()>1 || (!gmap.sources.at(0).F_with_error && tables.at(0).at(0).size()>2)) {
        for (int i = 0; i < gmap.sources.size(); i++) {
            if (tables.at(i).at(0).size()==2 || gmap.sources.at(i).F_with_error) {
                gmmc.writeLegend(texfile, gmap.sources.at(i).title, gmap.sources.at(i).color, gmap.sources.at(i).F_with_line, gmap.F_left);
            } else {
                std::vector<std::string> title_list;
                gmmc.makeTitle(title_list, gmap.sources.at(i).title, tables.at(i).at(0).size()-1, gmap.sources.at(i).file_name);
                std::vector<std::string> color_list;
                gmmc.makeColor(color_list, gmap.sources.at(i).color, gmap.sources.at(i).color2, tables.at(i).at(0).size()-1);
                for (int column = 1; column < tables.at(i).at(0).size(); column++) {
                    gmmc.writeLegend(texfile, title_list.at(column-1), color_list.at(column-1), gmap.sources.at(i).F_with_line, gmap.F_left);
                }
            }
        }
    }
    for (int i = 0; i < gmap.sources.size(); i++) {
        if (gmap.sources.at(i).F_with_linear) {
            std::string title = gmmc.makeFormula(tables.at(i), gmap.F_logx, gmap.F_logy);
            std::string color = gmap.sources.at(i).color + "!50!white";
            gmmc.writeLegend(texfile, title, color, true, gmap.F_left);
        }
    }
    texfile << std::endl;
    gmmc.writeFrame(texfile, gmap.xlabel, gmap.ylabel);
    texfile.close();

    // build
    std::string path = "/mnt/c/texlive/2021/bin/win32/";
    std::string uplatexcmd = path + "uplatex.exe " + gmap.texname + " > /dev/null 2>&1 ; ";
    std::string dvipdfmxcmd = path + "dvipdfmx.exe " + gmap.dviname + " > /dev/null 2>&1 ; ";
    std::string pdfcropcmd = path + "pdfcrop.exe --margins \"5\" " + gmap.pdfname + " " + gmap.pdfname + " > /dev/null 2>&1 ; ";
    std::string rmcmd = "rm " + gmap.dviname + " " + gmap.auxname + " " + gmap.logname;
    if (!gmap.F_tex) {
        rmcmd += " " + gmap.texname;
    }
    std::string cmd = uplatexcmd + dvipdfmxcmd + pdfcropcmd + rmcmd;
    std::system(cmd.c_str());

    return 0;
}

