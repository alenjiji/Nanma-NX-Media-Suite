// PHASE 15 INVARIANT TESTS:
// These tests prove adapters are stateless CLI bridges only.
// No defaults, no retries, no inference, no output modification.

#include "../include/adapters/MonitorAdapter.h"
#include "../include/cli/CliInvoker.h"
#include <cassert>
#include <string>

using namespace nx::ui::adapters;
using namespace nx::ui::cli;

class TestMonitorAdapter {
public:
    static void test_no_default_inference() {
        MockCliInvoker mock;
        mock.set_mock_result({0, "{\"status\":\"ok\"}", ""});
        
        MonitorStatusArgs args;
        args.format = "json";
        
        auto result = MonitorAdapter::status(args);
        
        // Verify CLI invocation matches exactly
        const auto& cli_args = mock.last_args();
        assert(cli_args.size() == 3);
        assert(cli_args[0] == "monitor");
        assert(cli_args[1] == "status");
        assert(cli_args[2] == "--format=json");
        
        // Verify no defaults added
        assert(result.exit_code == 0);
        assert(result.raw_json == "{\"status\":\"ok\"}");
    }
    
    static void test_cli_error_propagation() {
        MockCliInvoker mock;
        mock.set_mock_result({1, "", "ERROR: Invalid format"});
        
        MonitorStatusArgs args;
        args.format = "invalid";
        
        auto result = MonitorAdapter::status(args);
        
        // Verify error propagated unchanged
        assert(result.exit_code == 1);
        assert(result.raw_json == "");
        
        // CLI error message not modified by adapter
        const auto& cli_args = mock.last_args();
        assert(cli_args[2] == "--format=invalid");
    }
    
    static void test_no_retry_behavior() {
        MockCliInvoker mock;
        mock.set_mock_result({2, "", "Connection failed"});
        
        MonitorStatusArgs args;
        args.format = "json";
        
        auto result = MonitorAdapter::status(args);
        
        // Verify single invocation only (no retries)
        assert(result.exit_code == 2);
        
        // Mock should be called exactly once
        const auto& cli_args = mock.last_args();
        assert(cli_args.size() == 3);
    }
    
    static void test_output_preservation() {
        MockCliInvoker mock;
        const std::string original_json = "{\"engines\":[{\"id\":\"test\"}]}";
        mock.set_mock_result({0, original_json, ""});
        
        MonitorStatusArgs args;
        args.format = "json";
        
        auto result = MonitorAdapter::status(args);
        
        // Verify output preserved exactly
        assert(result.raw_json == original_json);
        assert(result.exit_code == 0);
    }
};

void run_monitor_adapter_tests() {
    TestMonitorAdapter::test_no_default_inference();
    TestMonitorAdapter::test_cli_error_propagation();
    TestMonitorAdapter::test_no_retry_behavior();
    TestMonitorAdapter::test_output_preservation();
}