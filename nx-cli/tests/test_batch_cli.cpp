#include "batch_argument_parser.h"
#include "batch_command.h"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

namespace nx::cli {

void test_batch_run_parsing() {
    std::cout << "Testing batch run argument parsing...\n";
    
    // Valid run request
    {
        std::vector<std::string> args = {"--file", "test.batch"};
        BatchRunRequest request;
        auto result = BatchArgumentParser::parse_run_args(args, request);
        assert(result.success);
        assert(request.batch_file == "test.batch");
        assert(!request.flags.dry_run);
        assert(!request.flags.json_output);
    }
    
    // With dry-run flag
    {
        std::vector<std::string> args = {"--file", "test.batch", "--dry-run"};
        BatchRunRequest request;
        auto result = BatchArgumentParser::parse_run_args(args, request);
        assert(result.success);
        assert(request.flags.dry_run);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--file", "test.batch", "--json"};
        BatchRunRequest request;
        auto result = BatchArgumentParser::parse_run_args(args, request);
        assert(result.success);
        assert(request.flags.json_output);
    }
    
    // Missing file flag
    {
        std::vector<std::string> args = {"--dry-run"};
        BatchRunRequest request;
        auto result = BatchArgumentParser::parse_run_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Batch run parsing tests passed\n";
}

void test_batch_validate_parsing() {
    std::cout << "Testing batch validate argument parsing...\n";
    
    // Valid validate request
    {
        std::vector<std::string> args = {"--file", "test.batch"};
        BatchValidateRequest request;
        auto result = BatchArgumentParser::parse_validate_args(args, request);
        assert(result.success);
        assert(request.batch_file == "test.batch");
        assert(!request.flags.json_output);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--file", "test.batch", "--json"};
        BatchValidateRequest request;
        auto result = BatchArgumentParser::parse_validate_args(args, request);
        assert(result.success);
        assert(request.flags.json_output);
    }
    
    // Reject dry-run flag (not allowed for validate)
    {
        std::vector<std::string> args = {"--file", "test.batch", "--dry-run"};
        BatchValidateRequest request;
        auto result = BatchArgumentParser::parse_validate_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Batch validate parsing tests passed\n";
}

void test_batch_summarize_parsing() {
    std::cout << "Testing batch summarize argument parsing...\n";
    
    // Valid summarize request
    {
        std::vector<std::string> args = {"--file", "test.batch"};
        BatchSummaryRequest request;
        auto result = BatchArgumentParser::parse_summary_args(args, request);
        assert(result.success);
        assert(request.batch_file == "test.batch");
        assert(!request.flags.json_output);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--file", "test.batch", "--json"};
        BatchSummaryRequest request;
        auto result = BatchArgumentParser::parse_summary_args(args, request);
        assert(result.success);
        assert(request.flags.json_output);
    }
    
    std::cout << "✓ Batch summarize parsing tests passed\n";
}

void test_unknown_flag_rejection() {
    std::cout << "Testing unknown flag rejection...\n";
    
    // Unknown flag in run
    {
        std::vector<std::string> args = {"--file", "test.batch", "--parallel"};
        BatchRunRequest request;
        auto result = BatchArgumentParser::parse_run_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    // Unknown flag in validate
    {
        std::vector<std::string> args = {"--file", "test.batch", "--continue-on-error"};
        BatchValidateRequest request;
        auto result = BatchArgumentParser::parse_validate_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Unknown flag rejection tests passed\n";
}

void test_duplicate_flag_rejection() {
    std::cout << "Testing duplicate flag rejection...\n";
    
    // Duplicate file flag
    {
        std::vector<std::string> args = {"--file", "test1.batch", "--file", "test2.batch"};
        BatchRunRequest request;
        auto result = BatchArgumentParser::parse_run_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    // Duplicate JSON flag
    {
        std::vector<std::string> args = {"--file", "test.batch", "--json", "--json"};
        BatchValidateRequest request;
        auto result = BatchArgumentParser::parse_validate_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Duplicate flag rejection tests passed\n";
}

void test_batch_file_validation() {
    std::cout << "Testing batch file validation...\n";
    
    // Create test batch file with valid commands
    std::string test_file = "test_valid.batch";
    {
        std::ofstream file(test_file);
        file << "nx convert transcode --input a.mov --output a.mp4\n";
        file << "nx audio process --input b.wav --output b_norm.wav\n";
        file << "nx video process --input c.mp4 --output c_hd.mp4\n";
    }
    
    BatchValidateRequest request;
    request.batch_file = test_file;
    request.flags.json_output = false;
    
    auto result = BatchCommand::handle_validate(request);
    assert(result.success);
    
    std::filesystem::remove(test_file);
    std::cout << "✓ Batch file validation tests passed\n";
}

void test_invalid_command_rejection() {
    std::cout << "Testing invalid command rejection...\n";
    
    // Create test batch file with invalid commands
    std::string test_file = "test_invalid.batch";
    {
        std::ofstream file(test_file);
        file << "nx convert transcode --input a.mov --output a.mp4\n";
        file << "ffmpeg -i input.mp4 output.mp4\n";  // Invalid: doesn't start with nx
        file << "nx audio process --input b.wav --output b_norm.wav\n";
    }
    
    BatchValidateRequest request;
    request.batch_file = test_file;
    request.flags.json_output = false;
    
    auto result = BatchCommand::handle_validate(request);
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::filesystem::remove(test_file);
    std::cout << "✓ Invalid command rejection tests passed\n";
}

void test_forbidden_operators_rejection() {
    std::cout << "Testing forbidden operators rejection...\n";
    
    // Create test batch file with forbidden operators
    std::string test_file = "test_forbidden.batch";
    {
        std::ofstream file(test_file);
        file << "nx convert transcode --input a.mov --output a.mp4 && echo done\n";  // Forbidden &&
    }
    
    BatchValidateRequest request;
    request.batch_file = test_file;
    request.flags.json_output = false;
    
    auto result = BatchCommand::handle_validate(request);
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::filesystem::remove(test_file);
    std::cout << "✓ Forbidden operators rejection tests passed\n";
}

void test_unknown_component_rejection() {
    std::cout << "Testing unknown component rejection...\n";
    
    // Create test batch file with unknown component
    std::string test_file = "test_unknown.batch";
    {
        std::ofstream file(test_file);
        file << "nx convert transcode --input a.mov --output a.mp4\n";
        file << "nx unknown operation --flag value\n";  // Unknown component
    }
    
    BatchValidateRequest request;
    request.batch_file = test_file;
    request.flags.json_output = false;
    
    auto result = BatchCommand::handle_validate(request);
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::filesystem::remove(test_file);
    std::cout << "✓ Unknown component rejection tests passed\n";
}

void test_batch_summary_output() {
    std::cout << "Testing batch summary output...\n";
    
    // Create test batch file with mixed components
    std::string test_file = "test_summary.batch";
    {
        std::ofstream file(test_file);
        file << "nx convert transcode --input a.mov --output a.mp4\n";
        file << "nx convert analyze --input b.mov\n";
        file << "nx audio process --input c.wav --output c_norm.wav\n";
        file << "nx video process --input d.mp4 --output d_hd.mp4\n";
        file << "nx metafix repair --input e.mp4 --output e_fixed.mp4\n";
    }
    
    BatchSummaryRequest request;
    request.batch_file = test_file;
    request.flags.json_output = false;
    
    auto result = BatchCommand::handle_summarize(request);
    assert(result.success);
    
    std::filesystem::remove(test_file);
    std::cout << "✓ Batch summary output tests passed\n";
}

void test_nonexistent_file_handling() {
    std::cout << "Testing nonexistent file handling...\n";
    
    BatchValidateRequest request;
    request.batch_file = "nonexistent.batch";
    request.flags.json_output = false;
    
    auto result = BatchCommand::handle_validate(request);
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Nonexistent file handling tests passed\n";
}

void test_empty_batch_file() {
    std::cout << "Testing empty batch file handling...\n";
    
    // Create empty batch file
    std::string test_file = "test_empty.batch";
    {
        std::ofstream file(test_file);
        // Empty file
    }
    
    BatchSummaryRequest request;
    request.batch_file = test_file;
    request.flags.json_output = false;
    
    auto result = BatchCommand::handle_summarize(request);
    assert(result.success);  // Empty file is valid
    
    std::filesystem::remove(test_file);
    std::cout << "✓ Empty batch file handling tests passed\n";
}

void test_order_preservation() {
    std::cout << "Testing command order preservation...\n";
    
    // Create test batch file with specific order
    std::string test_file = "test_order.batch";
    {
        std::ofstream file(test_file);
        file << "nx convert transcode --input first.mov --output first.mp4\n";
        file << "nx audio process --input second.wav --output second_norm.wav\n";
        file << "nx video process --input third.mp4 --output third_hd.mp4\n";
    }
    
    BatchRunRequest request;
    request.batch_file = test_file;
    request.flags.dry_run = true;
    request.flags.json_output = false;
    
    // This should preserve order in output
    auto result = BatchCommand::handle_run(request);
    assert(result.success);
    
    std::filesystem::remove(test_file);
    std::cout << "✓ Order preservation tests passed\n";
}

void test_strict_rejection() {
    std::cout << "Testing strict rejection of intelligence features...\n";
    
    // Test that we reject any "smart" flags that would add intelligence
    std::vector<std::string> forbidden_flags = {
        "--parallel", "--continue-on-error", "--retry", "--filter", 
        "--only", "--skip", "--optimize", "--reorder"
    };
    
    for (const auto& flag : forbidden_flags) {
        std::vector<std::string> args = {"--file", "test.batch", flag};
        BatchRunRequest request;
        auto result = BatchArgumentParser::parse_run_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Strict rejection tests passed\n";
}

} // namespace nx::cli

int main() {
    std::cout << "Running nx batch CLI tests...\n\n";
    
    nx::cli::test_batch_run_parsing();
    nx::cli::test_batch_validate_parsing();
    nx::cli::test_batch_summarize_parsing();
    nx::cli::test_unknown_flag_rejection();
    nx::cli::test_duplicate_flag_rejection();
    nx::cli::test_batch_file_validation();
    nx::cli::test_invalid_command_rejection();
    nx::cli::test_forbidden_operators_rejection();
    nx::cli::test_unknown_component_rejection();
    nx::cli::test_batch_summary_output();
    nx::cli::test_nonexistent_file_handling();
    nx::cli::test_empty_batch_file();
    nx::cli::test_order_preservation();
    nx::cli::test_strict_rejection();
    
    std::cout << "\n✅ All nx batch CLI tests passed!\n";
    return 0;
}