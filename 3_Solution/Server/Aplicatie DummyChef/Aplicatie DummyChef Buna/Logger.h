#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>

class Logger {
public:
    Logger(const std::string& filename) : filename(filename) {
        std::ofstream ofs(filename, std::ios::app);
    };
    void add(const std::string& message) {
        std::string timestampedMessage = GetTimestamp() + " - " + message;
        logEntries.push_back(timestampedMessage);
        WriteToFile(timestampedMessage);
    };
    ~Logger() {};
private:
    std::vector<std::string> logEntries;
    std::string filename;

    std::string GetTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);

        std::tm timeInfo;
        localtime_s(&timeInfo, &now_c);  // versiune sigurã

        char buffer[100];
        if (std::strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeInfo)) {
            return std::string(buffer);
        }

        return "UNKNOWN_TIME";
    }


    void WriteToFile(const std::string& entry) {
        std::ofstream outFile(filename, std::ios::app);
        if (outFile.is_open()) {
            outFile << entry << std::endl;
            outFile.close();
        }
    }
};
