#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <iomanip> // Include this for setw
#include <cstring>

// Function to run a command and capture its output
std::string runCommand(const char* command) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = _popen(command, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }

    _pclose(pipe);
    return result;
}

int main() {
    // Get the list of Wi-Fi profiles
    try {
        std::string meta_data = runCommand("netsh wlan show profiles");
        std::istringstream data_stream(meta_data);
        std::string line;
        std::vector<std::string> profiles;

        while (std::getline(data_stream, line)) {
            if (line.find("All User Profile") != std::string::npos) {
                size_t pos = line.find(":");
                if (pos != std::string::npos) {
                    std::string profile_name = line.substr(pos + 1);
                    // Remove leading and trailing whitespace
                    profile_name = profile_name.substr(profile_name.find_first_not_of(" \t"));
                    profile_name = profile_name.substr(0, profile_name.find_last_not_of(" \t") + 1);
                    profiles.push_back(profile_name);
                }
            }
        }

        // Print the table header
        std::cout << std::left << std::setw(30) << "Wi-Fi Name" << " | " << "Password" << std::endl;
        std::cout << std::string(45, '-') << std::endl;

        // Traverse the profiles
        for (const std::string& profile : profiles) {
            try {
                std::string command = "netsh wlan show profile \"" + profile + "\" key=clear";
                std::string profile_info = runCommand(command.c_str());
                std::istringstream info_stream(profile_info);
                std::string password_line;

                while (std::getline(info_stream, password_line)) {
                    if (password_line.find("Key Content") != std::string::npos) {
                        size_t pos = password_line.find(":");
                        if (pos != std::string::npos) {
                            std::string password = password_line.substr(pos + 1);
                            // Remove leading and trailing whitespace
                            password = password.substr(password.find_first_not_of(" \t"));
                            password = password.substr(0, password.find_last_not_of(" \t") + 1);
                            std::cout << std::left << std::setw(30) << profile << " | " << password << std::endl;
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error retrieving " << profile << " info: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
