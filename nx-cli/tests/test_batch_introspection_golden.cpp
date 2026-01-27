#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <sstream>

// Test includes
#include "../include/batch_introspection_command.h"
#include "../include/cli_types.h"

using namespace nx::cli;

// Capture stdout for testing
class OutputCapture {
public:
    OutputCapture() : old_cout(std::cout.rdbuf()) {
        std::cout.rdbuf(buffer.rdbuf());
    }
    
    ~OutputCapture() {
        std::cout.rdbuf(old_cout);
    }
    
    std::string get_output() {
        return buffer.str();
    }
    
private:
    std::ostringstream buffer;
    std::streambuf* old_cout;
};

void test_inspect_plan_basic() {
    std::cout << "Testing inspect plan basic...\n";
    
    OutputCapture capture;
    std::vector<std::string> args = {"plan", "test_batch_001"};
    auto result = BatchIntrospectionCommand::execute(args);
    std::string output = capture.get_output();
    
    assert(result.success);
    assert(result.error_code == CliErrorCode::NONE);
    
    // Verify JSON structure
    assert(output.find("\"batch_id\": \"test_batch_001\"") != std::string::npos);
    assert(output.find("\"plan_hash\": \"sha256:abc123def456\"") != std::string::npos);
    assert(output.find("\"job_count\": 3") != std::string::npos);
    assert(output.find("\"jobs\":") != std::string::npos);
    assert(output.find("\"dag\":") != std::string::npos);
    
    std::cout << "✓ Basic plan output correct\n";
}

int main() {
    std::cout << "=== Phase 14A Golden Tests ===\n\n";
    
    try {
        test_inspect_plan_basic();
        
        std::cout << "\n✅ All golden tests passed\n";
        std::cout << "Phase 14A.2 implementation verified\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}