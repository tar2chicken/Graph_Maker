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
        tables.push_back(table);
    }

    // make tex source code
    GMMakeCode gmmc;
    // get minimum and maximum values of each axis
    gmmc.getMinMax(tables);
    gmmc.resetXMinMax(gmap.F_xmin, gmap.xmin, gmap.F_xmax, gmap.xmax, 1.05);
    gmmc.resetYMinMax(gmap.F_ymin, gmap.ymin, gmap.F_ymax, gmap.ymax, 1.1);
    // output tex source code
    std::ofstream texfile(gmap.texname);
    gmmc.writePreamble(texfile);
    error_code = gmmc.writeScale(texfile);
    if (error_code == 1) {
        return 1;
    }
    for (int i = 0; i < gmap.sources.size(); i++) {
        gmmc.writeData(texfile, tables.at(i), gmap.sources.at(i).color, gmap.sources.at(i).F_with_line, gmap.sources.at(i).F_with_error);
    }
    if (gmap.sources.size() > 1) {
        for (int i = 0; i < gmap.sources.size(); i++) {
            gmmc.writeLegend(texfile, gmap.sources.at(i).title, gmap.sources.at(i).color, gmap.sources.at(i).F_with_line);
        }
        texfile << std::endl;
    }
    gmmc.writeFrame(texfile, gmap.xlabel, gmap.ylabel);
    texfile.close();

    // build
    std::string path = "/mnt/c/texlive/2021/bin/win32/";
    std::string uplatexcmd = path + "uplatex.exe " + gmap.texname + " > /dev/null 2>&1 ; ";
    std::string dvipdfmxcmd = path + "dvipdfmx.exe " + gmap.dviname + " > /dev/null 2>&1 ; ";
    std::string pdfcropcmd = path + "pdfcrop.exe " + gmap.pdfname + " " + gmap.pdfname + " > /dev/null 2>&1 ; ";
    std::string rmcmd = "rm " + gmap.dviname + " " + gmap.auxname + " " + gmap.logname;
    if (!gmap.F_tex) {
        rmcmd += " " + gmap.texname;
    }
    std::string cmd = uplatexcmd + dvipdfmxcmd + pdfcropcmd + rmcmd;
    std::system(cmd.c_str());

    return 0;
}

