#ifndef GMMakeCode_hpp
#define GMMakeCode_hpp

/// @brief make tex source code
class GMMakeCode {
    private:
        double xmin;
        double xmax;
        double ymin;
        double ymax;

        double dx;
        double dy;
        int xscale_count;
        int yscale_count;

        const double width = 10;
        const double height = 8;

        int legend_count;

    public:
        /// @brief constructor
        GMMakeCode();

        /// @brief get minimum and maximum values of x, y
        /// @param tables vector of table (call by reference)
        void getMinMax(const std::vector<std::vector<std::vector<double>>>& tables);

        /// @brief reset minimum and maximum values of x-axis
        /// @param F_xmin flag indicating that xmin is entered
        /// @param xmin_entered entered xmin
        /// @param F_xmax flag indicating that xmax is entered
        /// @param xmax_entered entered xmax
        /// @param extension extension ratio
        /// @param F_logx flag of log plot of x axis
        void resetXMinMax(const bool F_xmin, double xmin_entered, const bool F_xmax, double xmax_entered, const double extension, const bool F_logx);

        /// @brief reset minimum and maximum values of y-axis
        /// @param F_ymin flag indicating that ymin is entered
        /// @param ymin_entered entered ymin
        /// @param F_ymax flag indicating that ymax is entered
        /// @param ymax_entered entered ymax
        /// @param extension extension ratio
        /// @param F_logy flag of log plot of y axis
        void resetYMinMax(const bool F_ymin, double ymin_entered, const bool F_ymax, double ymax_entered, const double extension, const bool F_logy);

        /// @brief write preamble code to tex file
        /// @param texfile tex file (call by reference)
        void writePreamble(std::ofstream& texfile);

        /// @brief write code of scale to tex file
        /// @param texfile tex file (call by reference)
        /// @param F_logx flag of log plot of x axis
        /// @param F_logy flag of log plot of y axis
        /// @return 0:OK, 1:ERROR
        int writeScale(std::ofstream& texfile, const bool F_logx, const bool F_logy);

        /// @brief write code of plot to tex file
        /// @param texfile tex file (call by reference)
        /// @param table table (call by reference)
        /// @param column column
        /// @param color color
        /// @param F_with_line flag of option to connect data points with lines
        /// @param F_with_error flag of error bar
        /// @param F_with_linear flag of linear regression
        void writeData(std::ofstream& texfile, const std::vector<std::vector<double>>& table, const int column, const std::string color, const bool F_with_line, const bool F_with_error, const bool F_with_linear);

        /// @brief make list of titles
        /// @param title_list list of titles (call by reference)
        /// @param title title entered on command line
        /// @param data_number the number of data
        /// @param file_name file name
        void makeTitle(std::vector<std::string>& title_list, std::string title, const int data_number, const std::string file_name);

        /// @brief make list of colors
        /// @param color_list list of colors
        /// @param color first color
        /// @param color2 second color
        /// @param data_number the number of data
        void makeColor(std::vector<std::string>& color_list, const std::string color, const std::string color2, const int data_number);

        /// @brief output formula of regression line
        /// @param table data (call by reference)
        /// @param F_logx flag of log plot of x axis
        /// @param F_logy flag of log plot of y axis
        /// @return formula of regression line
        std::string makeFormula(std::vector<std::vector<double>>& table, const bool F_logx, const bool F_logy);

        /// @brief write code of graph legend to tex file
        /// @param texfile tex file (call by reference)
        /// @param title title of graph legend
        /// @param color color
        /// @param F_with_line flag of with_line
        /// @param F_left flag of left graph legend
        void writeLegend(std::ofstream& texfile, std::string title, const std::string color, const bool F_with_line, const bool F_left);

        /// @brief write code of frame and x, y label to tex file
        /// @param texfile tex file (call by reference)
        /// @param xlabel x-axis label
        /// @param ylabel y-axis label
        void writeFrame(std::ofstream& texfile, const std::string xlabel, const std::string ylabel);

    private:
        /// @brief get scale from minimum and maximum values
        /// @param min_value minimum value
        /// @param max_value maximum value
        /// @return scale, if failed, return -1
        double scale(const double min_value, const double max_value);

        /// @brief get x-coordinate of tikz from x value
        /// @param x value of x
        /// @return x-coordinate of tikz
        double xPosition(const double x);

        /// @brief get y-coordinate of tikz from y value
        /// @param y value of y
        /// @return y-coordinate of tikz
        double yPosition(const double y);

        /// @brief linear regression of data
        /// @param table data (call by reference)
        /// @param column column
        /// @param a slope (call by reference)
        /// @param b intercept (call by reference)
        void linearRegression(const std::vector<std::vector<double>>& table, const int column, double& a, double& b);
};

#endif
