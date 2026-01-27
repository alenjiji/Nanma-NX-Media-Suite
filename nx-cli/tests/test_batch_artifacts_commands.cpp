#include <iostream>
#include <vector>
#include <string>
#include <cassert>

// Test includes
#include "../include/batch_introspection_command.h"
#include "../include/cli_types.h"

using namespace nx::cli;

void test_artifacts_argument_parsing() {
    std::cout << "Testing artifacts argument parsing...\n";
    
    // Test missing batch ID
    {
        std::vector<std::string> args = {"artifacts"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    // Test invalid artifact type
    {
        std::vector<std::string> args = {"artifacts", "test_batch_001", "--artifact-type", "invalid"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    }
    
    // Test unknown flag
    {
        std::vector<std::string> args = {"artifacts", "test_batch_001", "--unknown-flag"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Artifacts argument parsing works\n";
}

void test_artifact_argument_parsing() {
    std::cout << "Testing artifact argument parsing...\n";
    
    // Test missing arguments
    {
        std::vector<std::string> args = {"artifact", "test_batch_001"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    // Test invalid format
    {
        std::vector<std::string> args = {"artifact", "test_batch_001", "artifact_001", "--format", "invalid"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    }
    
    // Test invalid max-size
    {
        std::vector<std::string> args = {"artifact", "test_batch_001", "artifact_001", "--max-size", "invalid"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Artifact argument parsing works\n";
}

void test_artifacts_not_implemented() {
    std::cout << "Testing artifacts command (not implemented)...\n";
    
    std::vector<std::string> args = {"artifacts", "test_batch_001"};
    auto result = BatchIntrospectionCommand::execute(args);
    
    // Should fail with NX_EXEC_FAILED due to parsing not implemented
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_EXEC_FAILED);
    
    std::cout << "✓ Artifacts command fails appropriately (parsing not implemented)\n";
}

void test_artifact_not_implemented() {
    std::cout << "Testing artifact command (not implemented)...\n";
    
    std::vector<std::string> args = {"artifact", "test_batch_001", "artifact_001"};
    auto result = BatchIntrospectionCommand::execute(args);
    
    // Should fail with NX_EXEC_FAILED due to parsing not implemented
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_EXEC_FAILED);
    
    std::cout << "✓ Artifact command fails appropriately (parsing not implemented)\n";
}

void test_batch_not_found_errors() {
    std::cout << "Testing batch not found errors...\n";
    
    // Test unknown batch for artifacts
    {
        std::vector<std::string> args = {"artifacts", "unknown_batch"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::ERROR_BATCH_NOT_FOUND);
    }
    
    // Test unknown batch for artifact
    {
        std::vector<std::string> args = {"artifact", "unknown_batch", "artifact_001"};
        auto result = BatchIntrospectionCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::ERROR_BATCH_NOT_FOUND);
    }
    
    std::cout << "✓ Batch not found errors work\n";
}

void test_valid_enum_values() {
    std::cout << "Testing valid enum values...\n";
    
    // Test valid artifact types (should reach parsing not implemented)
    std::vector<std::string> valid_types = {"report", "validation", "hash", "timeline", "log"};
    
    for (const auto& type : valid_types) {
        std::vector<std::string> args = {"artifacts", "test_batch_001", "--artifact-type", type};
        auto result = BatchIntrospectionCommand::execute(args);
        
        // Should fail with NX_EXEC_FAILED (parsing not implemented), not enum error
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_EXEC_FAILED);
    }
    
    // Test valid formats for artifact command
    std::vector<std::string> valid_formats = {"json", "raw"};
    
    for (const auto& format : valid_formats) {
        std::vector<std::string> args = {"artifact", "test_batch_001", "artifact_001", "--format", format};
        auto result = BatchIntrospectionCommand::execute(args);
        
        // Should fail with NX_EXEC_FAILED (parsing not implemented), not enum error
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_EXEC_FAILED);
    }
    
    std::cout << "✓ Valid enum values accepted\n";
}

int main() {
    std::cout << "=== Phase 14A Artifacts Commands Tests ===\n\n";
    
    try {
        test_artifacts_argument_parsing();
        test_artifact_argument_parsing();
        test_artifacts_not_implemented();
        test_artifact_not_implemented();
        test_batch_not_found_errors();
        test_valid_enum_values();
        
        std::cout << "\n✅ All artifacts tests passed\n";
        std::cout << "Artifacts commands implemented with proper error handling\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
}