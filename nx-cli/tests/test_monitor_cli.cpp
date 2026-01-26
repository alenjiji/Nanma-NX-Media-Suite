#include "monitor_argument_parser.h"
#include "monitor_command.h"
#include <iostream>
#include <cassert>

namespace nx::cli {

void test_monitor_status_parsing() {
    std::cout << "Testing monitor status argument parsing...\n";
    
    // Valid status request (no flags)
    {
        std::vector<std::string> args = {};
        MonitorStatusRequest request;
        auto result = MonitorArgumentParser::parse_status_args(args, request);
        assert(result.success);
        assert(!request.flags.json_output);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--json"};
        MonitorStatusRequest request;
        auto result = MonitorArgumentParser::parse_status_args(args, request);
        assert(result.success);
        assert(request.flags.json_output);
    }
    
    // Unknown flag rejection
    {
        std::vector<std::string> args = {"--watch"};
        MonitorStatusRequest request;
        auto result = MonitorArgumentParser::parse_status_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Monitor status parsing tests passed\n";
}

void test_monitor_jobs_parsing() {
    std::cout << "Testing monitor jobs argument parsing...\n";
    
    // Valid jobs request
    {
        std::vector<std::string> args = {};
        MonitorJobsRequest request;
        auto result = MonitorArgumentParser::parse_jobs_args(args, request);
        assert(result.success);
        assert(!request.flags.json_output);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--json"};
        MonitorJobsRequest request;
        auto result = MonitorArgumentParser::parse_jobs_args(args, request);
        assert(result.success);
        assert(request.flags.json_output);
    }
    
    // Forbidden filtering flags
    {
        std::vector<std::string> args = {"--filter", "active"};
        MonitorJobsRequest request;
        auto result = MonitorArgumentParser::parse_jobs_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Monitor jobs parsing tests passed\n";
}

void test_monitor_job_parsing() {
    std::cout << "Testing monitor job argument parsing...\n";
    
    // Valid job request with ID
    {
        std::vector<std::string> args = {"--id", "job-123"};
        MonitorJobRequest request;
        auto result = MonitorArgumentParser::parse_job_args(args, request);
        assert(result.success);
        assert(request.job_id == "job-123");
        assert(!request.flags.json_output);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--id", "job-456", "--json"};
        MonitorJobRequest request;
        auto result = MonitorArgumentParser::parse_job_args(args, request);
        assert(result.success);
        assert(request.job_id == "job-456");
        assert(request.flags.json_output);
    }
    
    // Missing required ID flag
    {
        std::vector<std::string> args = {"--json"};
        MonitorJobRequest request;
        auto result = MonitorArgumentParser::parse_job_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Monitor job parsing tests passed\n";
}

void test_monitor_engines_parsing() {
    std::cout << "Testing monitor engines argument parsing...\n";
    
    // Valid engines request
    {
        std::vector<std::string> args = {};
        MonitorEnginesRequest request;
        auto result = MonitorArgumentParser::parse_engines_args(args, request);
        assert(result.success);
        assert(!request.flags.json_output);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--json"};
        MonitorEnginesRequest request;
        auto result = MonitorArgumentParser::parse_engines_args(args, request);
        assert(result.success);
        assert(request.flags.json_output);
    }
    
    std::cout << "✓ Monitor engines parsing tests passed\n";
}

void test_monitor_version_parsing() {
    std::cout << "Testing monitor version argument parsing...\n";
    
    // Valid version request
    {
        std::vector<std::string> args = {};
        MonitorVersionRequest request;
        auto result = MonitorArgumentParser::parse_version_args(args, request);
        assert(result.success);
        assert(!request.flags.json_output);
    }
    
    // With JSON flag
    {
        std::vector<std::string> args = {"--json"};
        MonitorVersionRequest request;
        auto result = MonitorArgumentParser::parse_version_args(args, request);
        assert(result.success);
        assert(request.flags.json_output);
    }
    
    std::cout << "✓ Monitor version parsing tests passed\n";
}

void test_forbidden_control_operations() {
    std::cout << "Testing forbidden control operations rejection...\n";
    
    // Test that control verbs are rejected at operation level
    std::vector<std::string> forbidden_operations = {
        "pause", "resume", "cancel", "watch", "tail", "follow", 
        "subscribe", "filter", "since", "live"
    };
    
    for (const auto& operation : forbidden_operations) {
        std::vector<std::string> args = {operation};
        auto result = MonitorCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Forbidden control operations rejection tests passed\n";
}

void test_forbidden_flags_rejection() {
    std::cout << "Testing forbidden flags rejection...\n";
    
    // Test forbidden flags across all operations
    std::vector<std::string> forbidden_flags = {
        "--watch", "--tail", "--follow", "--subscribe", "--live", 
        "--since", "--filter", "--pause", "--resume", "--cancel"
    };
    
    for (const auto& flag : forbidden_flags) {
        // Test with status operation
        {
            std::vector<std::string> args = {flag};
            MonitorStatusRequest request;
            auto result = MonitorArgumentParser::parse_status_args(args, request);
            assert(!result.success);
            assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
        }
        
        // Test with jobs operation
        {
            std::vector<std::string> args = {flag};
            MonitorJobsRequest request;
            auto result = MonitorArgumentParser::parse_jobs_args(args, request);
            assert(!result.success);
            assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
        }
    }
    
    std::cout << "✓ Forbidden flags rejection tests passed\n";
}

void test_duplicate_flag_rejection() {
    std::cout << "Testing duplicate flag rejection...\n";
    
    // Duplicate JSON flag
    {
        std::vector<std::string> args = {"--json", "--json"};
        MonitorStatusRequest request;
        auto result = MonitorArgumentParser::parse_status_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    // Duplicate ID flag
    {
        std::vector<std::string> args = {"--id", "job1", "--id", "job2"};
        MonitorJobRequest request;
        auto result = MonitorArgumentParser::parse_job_args(args, request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Duplicate flag rejection tests passed\n";
}

void test_engine_not_implemented_responses() {
    std::cout << "Testing engine not implemented responses...\n";
    
    // All operations should return NX_ENGINE_REJECTED in Phase 4
    {
        MonitorStatusRequest request;
        auto result = MonitorCommand::handle_status(request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    {
        MonitorJobsRequest request;
        auto result = MonitorCommand::handle_jobs(request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    {
        MonitorJobRequest request;
        request.job_id = "test-job";
        auto result = MonitorCommand::handle_job(request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    {
        MonitorEnginesRequest request;
        auto result = MonitorCommand::handle_engines(request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    {
        MonitorVersionRequest request;
        auto result = MonitorCommand::handle_version(request);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    std::cout << "✓ Engine not implemented responses tests passed\n";
}

void test_read_only_enforcement() {
    std::cout << "Testing read-only enforcement...\n";
    
    // Verify no operation can mutate state (all return engine rejected)
    // This ensures Phase 4 compliance - no execution, only observation contracts
    
    MonitorStatusRequest status_req;
    auto status_result = MonitorCommand::handle_status(status_req);
    assert(status_result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    
    MonitorJobsRequest jobs_req;
    auto jobs_result = MonitorCommand::handle_jobs(jobs_req);
    assert(jobs_result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    
    MonitorJobRequest job_req;
    job_req.job_id = "test";
    auto job_result = MonitorCommand::handle_job(job_req);
    assert(job_result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    
    MonitorEnginesRequest engines_req;
    auto engines_result = MonitorCommand::handle_engines(engines_req);
    assert(engines_result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    
    MonitorVersionRequest version_req;
    auto version_result = MonitorCommand::handle_version(version_req);
    assert(version_result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    
    std::cout << "✓ Read-only enforcement tests passed\n";
}

void test_deterministic_output_order() {
    std::cout << "Testing deterministic output order...\n";
    
    // Test that operations produce consistent output (no randomization)
    // Multiple calls should produce identical results
    
    MonitorStatusRequest request1, request2;
    request1.flags.json_output = true;
    request2.flags.json_output = true;
    
    // Both should fail identically (Phase 4 behavior)
    auto result1 = MonitorCommand::handle_status(request1);
    auto result2 = MonitorCommand::handle_status(request2);
    
    assert(result1.success == result2.success);
    assert(result1.error_code == result2.error_code);
    
    std::cout << "✓ Deterministic output order tests passed\n";
}

void test_operation_routing() {
    std::cout << "Testing operation routing...\n";
    
    // Valid operations should route correctly
    {
        std::vector<std::string> args = {"status"};
        auto result = MonitorCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    {
        std::vector<std::string> args = {"jobs"};
        auto result = MonitorCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    {
        std::vector<std::string> args = {"job", "--id", "test"};
        auto result = MonitorCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_ENGINE_REJECTED);
    }
    
    // Invalid operation should be rejected
    {
        std::vector<std::string> args = {"invalid"};
        auto result = MonitorCommand::execute(args);
        assert(!result.success);
        assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    }
    
    std::cout << "✓ Operation routing tests passed\n";
}

void test_no_placeholder_output() {
    MonitorStatusRequest request;
    auto result = MonitorCommand::handle_status(request);
    assert(!result.success);
}

} // namespace nx::cli

int main() {
    std::cout << "Running nx monitor CLI tests...\n\n";
    
    nx::cli::test_monitor_status_parsing();
    nx::cli::test_monitor_jobs_parsing();
    nx::cli::test_monitor_job_parsing();
    nx::cli::test_monitor_engines_parsing();
    nx::cli::test_monitor_version_parsing();
    nx::cli::test_forbidden_control_operations();
    nx::cli::test_forbidden_flags_rejection();
    nx::cli::test_duplicate_flag_rejection();
    nx::cli::test_engine_not_implemented_responses();
    nx::cli::test_read_only_enforcement();
    nx::cli::test_deterministic_output_order();
    nx::cli::test_operation_routing();
    nx::cli::test_no_placeholder_output();
    
    std::cout << "\n✅ All nx monitor CLI tests passed!\n";
    return 0;
}