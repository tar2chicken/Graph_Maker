#ifndef GMArgProc_hpp
#define GMArgProc_hpp

#include <string>
#include <vector>

/// @brief process command line arguments of Graph Maker
class GMArgProc {
    private:
        /// @brief vector of arguments
        std::vector<std::string> arguments;

    public:
        /// @brief source property
        class source {
            public:
                std::string file_name;
                std::string color;
                std::string color2;
                std::string title;
                bool F_with_line;
                bool F_with_error;

                /// @brief constructor
                source();
        };

        /// @brief vector of source
        std::vector<GMArgProc::source> sources;

        /// @brief output file name
        std::string ofname;
        /// @brief x-axis label
        std::string xlabel;
        /// @brief y-axis label
        std::string ylabel;
        /// @brief minimum value of x-axis
        double xmin;
        /// @brief maximum value of x-axis
        double xmax;
        /// @brief minimum value of y-axis
        double ymin;
        /// @brief maximum value of y-axis
        double ymax;

        /// @brief flag indicating that minimum value of x-axis is entered
        bool F_xmin;
        /// @brief flag indicating that maximum value of x-axis is entered
        bool F_xmax;
        /// @brief flag indicating that minimum value of y-axis is entered
        bool F_ymin;
        /// @brief flag indicating that maximum value of y-axis is entered
        bool F_ymax;
        /// @brief flag of tex file output
        bool F_tex;
        /// @brief flag of log plot of x axis
        bool F_logx;
        /// @brief flag of log plot of y axis
        bool F_logy;

        std::string texname;
        std::string dviname;
        std::string pdfname;
        std::string auxname;
        std::string logname;

    public:
        /// @brief constructor
        GMArgProc();

        /// @brief set arguments
        /// @param argc number of arguments
        /// @param argv array of arguments
        void setArguments(int argc, char** argv);

        /// @brief process command line arguments
        /// @return 0:OK, 1:ERROR, 2:version, help
        int process();

    private:
        /// @brief display version
        void version();

        /// @brief display help
        void help();

        /// @brief get source from arguments, add it to sources, increment i
        /// @param i index of vector arguments
        /// @return 0:OK, 1:ERROR
        int getSource(int& i);

        /// @brief get option from arguments, set flag, increment i
        /// @param i index of vector arguments
        /// @return 0:OK, 1:ERROR, 2:no option
        int getOption(int& i);

        /// @brief set output file name
        /// @return 0:OK, 1:ERROR
        int setOutputFile();
};

#endif
