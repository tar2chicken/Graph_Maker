#ifndef FileToTable_hpp
#define FileToTable_hpp

/// @brief create a table from a data file
class FileToTable {
    private:
        /// @brief delimiter of date file
        char delimiter;

    public:
        /// @brief constructor
        FileToTable();

        /// @brief available for numeric data
        /// @param ifname input file name
        /// @param table 2-dimensional vector (call by reference)
        void numeric(const std::string ifname, std::vector<std::vector<double>>& table);
};

#endif
