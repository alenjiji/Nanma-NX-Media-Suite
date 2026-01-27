#include "nx/batch/ExecutionCoordinator.h"
#include "nx/batch/BatchEngineImpl.h"
#include <cassert>

using namespace nx::batch;

void test_job_spec_preparation() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true},
        {"nx audio --input test.wav --output test.flac", {"nx", "audio", "--input", "test.wav", "--output", "test.flac"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto job_specs = ExecutionCoordinator::prepare_job_specs(session);
    
    assert(job_specs.size() == session.job_count());
    
    auto session_jobs = session.jobs();
    for (size_t i = 0; i < job_specs.size(); ++i) {
        assert(job_specs[i].job_id == session_jobs[i].job_id);
        assert(job_specs[i].command == session_jobs[i].command);
        assert(job_specs[i].arguments == session_jobs[i].arguments);
    }
}

void test_single_job_spec_preparation() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto session_jobs = session.jobs();
    
    auto job_spec = ExecutionCoordinator::prepare_job_spec(session, session_jobs[0].job_id);
    
    assert(job_spec.has_value());
    assert(job_spec->job_id == session_jobs[0].job_id);
    assert(job_spec->command == session_jobs[0].command);
    assert(job_spec->arguments == session_jobs[0].arguments);
    
    SessionJobId invalid_id{session.id(), "invalid"};
    auto missing_spec = ExecutionCoordinator::prepare_job_spec(session, invalid_id);
    assert(!missing_spec.has_value());
}

void test_session_immutability_during_spec_preparation() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto original_job_count = session.job_count();
    auto original_jobs = session.jobs();
    
    // Prepare specs - session must remain unchanged
    auto job_specs = ExecutionCoordinator::prepare_job_specs(session);
    
    assert(session.job_count() == original_job_count);
    assert(session.jobs() == original_jobs);
    
    // Specs should be independent copies
    assert(job_specs.size() == original_jobs.size());
    for (size_t i = 0; i < job_specs.size(); ++i) {
        assert(job_specs[i].job_id == original_jobs[i].job_id);
        assert(job_specs[i].command == original_jobs[i].command);
    }
}

void test_job_execution_result_structure() {
    // Test that JobExecutionResult is a simple value type
    SessionId session_id{"test-session"};
    SessionJobId job_id{session_id, "job-001"};
    
    JobExecutionResult result{
        .success = true,
        .message = "Test execution",
        .result_token = "execution-token-123"
    };
    
    assert(result.success == true);
    assert(result.message == "Test execution");
    assert(result.result_token == "execution-token-123");
    
    // Test equality
    JobExecutionResult result2{true, "Test execution", "execution-token-123"};
    assert(result == result2);
    
    JobExecutionResult result3{true, "Test execution", "different-token"};
    assert(!(result == result3));
}

int main() {
    test_job_spec_preparation();
    test_single_job_spec_preparation();
    test_session_immutability_during_spec_preparation();
    test_job_execution_result_structure();
    
    return 0;
}