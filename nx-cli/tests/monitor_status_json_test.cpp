#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cassert>

std::string read_file(const std::string& path) {
    std::ifstream file(path);
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::string execute_command(const std::string& command) {
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) return "";
    
    std::ostringstream result;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result << buffer;
    }
    pclose(pipe);
    return result.str();
}

int main() {
    std::string golden_output = read_file("tests/golden/monitor_status.json");
    std::string cli_output = execute_command("./nx-cli monitor status --json");
    
    assert(cli_output == golden_output);
    
    return 0;
}