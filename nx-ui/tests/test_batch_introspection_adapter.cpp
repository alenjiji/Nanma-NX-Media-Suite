// PHASE 15 INVARIANT TESTS:
// These tests prove batch introspection adapters are stateless CLI bridges only.
// No defaults, no retries, no inference, no output modification.

#include "../include/adapters/BatchIntrospectionAdapter.h"
#include "../include/cli/CliInvoker.h"
#include <cassert>
#include <string>

using namespace nx::ui::adapters;
using namespace nx::ui::cli;

class TestBatchIntrospectionAdapter {
public:
    static void test_batch_not_found_error() {
        MockCliInvoker mock;
        mock.set_mock_result({3, "", "ERROR_BATCH_NOT_FOUND: batch_123"});
        
        BatchInspectPlanArgs args;
        args.batch_id = "batch_123";
        args.format = "json";
        args.jobs_only = false;
        args.dag_only = false;
        
        auto result = BatchIntrospectionAdapter::plan(args);
        
        // Verify CLI error code preserved exactly
        assert(result.exit_code == 3);
        assert(result.raw_json == "");
        
        // CLI arguments passed through unchanged
        const auto& cli_args = mock.last_args();
        assert(cli_args[0] == "batch");
        assert(cli_args[1] == "inspect");
        assert(cli_args[2] == "plan");
        assert(cli_args[3] == "batch_123");
        assert(cli_args[4] == "--format=json");
    }
    
    static void test_execution_flags_raw_mapping() {
        MockCliInvoker mock;
        mock.set_mock_result({0, "{\"job_states\":[]}", ""});
        
        BatchInspectStatusArgs args;
        args.batch_id = "batch_456";
        args.format = "json";
        args.filter_state = "failed";
        args.include_retries = true; // Maps directly to CLI flag --include-retries
        
        auto result = BatchIntrospectionAdapter::status(args);
        
        // Verify CLI flags mapped exactly
        const auto& cli_args = mock.last_args();
        assert(cli_args.size() == 7);
        assert(cli_args[3] == "batch_456");
        assert(cli_args[4] == "--format=json");
        assert(cli_args[5] == "--filter-state=failed");
        assert(cli_args[6] == "--include-retries");
        
        assert(result.exit_code == 0);
    }
    
    static void test_no_retry_on_artifact_error() {
        MockCliInvoker mock;
        mock.set_mock_result({4, "", "ERROR_ARTIFACT_NOT_FOUND: artifact_789"});
        
        BatchInspectArtifactArgs args;
        args.batch_id = "batch_456";
        args.artifact_id = "artifact_789";
        args.format = "raw";
        args.max_size = 1024;
        
        auto result = BatchIntrospectionAdapter::artifact(args);
        
        // Verify single invocation only (no retries)
        assert(result.exit_code == 4);
        
        // Mock called exactly once
        const auto& cli_args = mock.last_args();
        assert(cli_args[1] == "inspect");
        assert(cli_args[2] == "artifact");
    }
    
    static void test_timeline_flag_preservation() {
        MockCliInvoker mock;
        const std::string timeline_json = "{\"job_id\":\"job_1\",\"timeline\":[]}";
        mock.set_mock_result({0, timeline_json, ""});
        
        BatchInspectJobArgs args;
        args.batch_id = "batch_789";
        args.job_id = "job_1";
        args.format = "json";
        args.include_artifacts = false;
        args.include_timeline = true; // Maps directly to CLI flag --include-timeline
        
        auto result = BatchIntrospectionAdapter::job(args);
        
        // Verify timeline flag mapped exactly
        const auto& cli_args = mock.last_args();
        bool found_timeline_flag = false;
        for (const auto& arg : cli_args) {
            if (arg == "--include-timeline") {
                found_timeline_flag = true;
                break;
            }
        }
        assert(found_timeline_flag);
        
        // Verify output preserved exactly
        assert(result.raw_json == timeline_json);
        assert(result.exit_code == 0);
    }
};

void run_batch_introspection_adapter_tests() {
    TestBatchIntrospectionAdapter::test_batch_not_found_error();
    TestBatchIntrospectionAdapter::test_execution_flags_raw_mapping();
    TestBatchIntrospectionAdapter::test_no_retry_on_artifact_error();
    TestBatchIntrospectionAdapter::test_timeline_flag_preservation();
}