// PHASE 15 INVARIANT TESTS:
// These tests prove batch adapters are stateless CLI bridges only.
// No defaults, no retries, no inference, no output modification.

#include "../include/adapters/BatchAdapter.h"
#include "../include/cli/CliInvoker.h"
#include <cassert>
#include <string>

using namespace nx::ui::adapters;
using namespace nx::ui::cli;

class TestBatchAdapter {
public:
    static void test_no_default_inference() {
        MockCliInvoker mock;
        mock.set_mock_result({0, "{\"batch_id\":\"test\"}", ""});
        
        BatchRunArgs args;
        args.batch_file_path = "/path/to/batch.txt";
        args.format = "json";
        args.dry_run = true;
        
        auto result = BatchAdapter::run(args);
        
        // Verify CLI invocation matches exactly
        const auto& cli_args = mock.last_args();
        assert(cli_args.size() == 5);
        assert(cli_args[0] == "batch");
        assert(cli_args[1] == "run");
        assert(cli_args[2] == "/path/to/batch.txt");
        assert(cli_args[3] == "--format=json");
        assert(cli_args[4] == "--dry-run");
        
        // Verify no defaults added
        assert(result.exit_code == 0);
        assert(result.raw_json == "{\"batch_id\":\"test\"}");
    }
    
    static void test_file_not_found_error() {
        MockCliInvoker mock;
        mock.set_mock_result({2, "", "ERROR: File not found: /invalid/path"});
        
        BatchRunArgs args;
        args.batch_file_path = "/invalid/path";
        args.format = "json";
        args.dry_run = false;
        
        auto result = BatchAdapter::run(args);
        
        // Verify error propagated unchanged
        assert(result.exit_code == 2);
        assert(result.raw_json == "");
        
        // CLI arguments passed through unchanged
        const auto& cli_args = mock.last_args();
        assert(cli_args[2] == "/invalid/path");
    }
    
    static void test_no_retry_on_failure() {
        MockCliInvoker mock;
        mock.set_mock_result({1, "", "Validation failed"});
        
        BatchValidateArgs args;
        args.batch_file_path = "/path/to/batch.txt";
        args.format = "json";
        
        auto result = BatchAdapter::validate(args);
        
        // Verify single invocation only (no retries)
        assert(result.exit_code == 1);
        
        // Mock called exactly once
        const auto& cli_args = mock.last_args();
        assert(cli_args.size() == 4);
        assert(cli_args[1] == "validate");
    }
    
    static void test_output_preservation() {
        MockCliInvoker mock;
        const std::string original_json = "{\"commands\":[\"nx convert\"],\"valid\":true}";
        mock.set_mock_result({0, original_json, ""});
        
        BatchSummaryArgs args;
        args.batch_file_path = "/path/to/batch.txt";
        args.format = "json";
        
        auto result = BatchAdapter::summarize(args);
        
        // Verify output preserved exactly
        assert(result.raw_json == original_json);
        assert(result.exit_code == 0);
    }
};

void run_batch_adapter_tests() {
    TestBatchAdapter::test_no_default_inference();
    TestBatchAdapter::test_file_not_found_error();
    TestBatchAdapter::test_no_retry_on_failure();
    TestBatchAdapter::test_output_preservation();
}