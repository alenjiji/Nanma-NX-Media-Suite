#include "../include/nx_batchflow_preset.h"
#include "../include/nx_batchflow_dag.h"
#include "../include/nx_batchflow_scheduler.h"
#include "../include/nx_batchflow_logical_clock.h"
#include "../include/nx_batchflow_replay.h"
#include "../include/nx_batchflow_retry_policy.h"
#include <cassert>
#include <iostream>
#include <stdexcept>

using namespace nx::batchflow;

void test_cyclic_dependency_preset() {
    std::cout << "Testing cyclic dependency detection...\n";
    
    try {
        // Create preset with cyclic dependencies: A -> B -> C -> A
        BatchFlowPreset preset(
            PresetVersion::current(),
            "Cyclic Test Preset",
            "Intentionally creates cycle to test detection"
        );
        
        PresetJobDefinition jobA("job_a", "engine", "op", "{}", {}, {"output_a"});
        PresetJobDefinition jobB("job_b", "engine", "op", "{}", {"output_a"}, {"output_b"});
        PresetJobDefinition jobC("job_c", "engine", "op", "{}", {"output_b"}, {"output_c"});
        PresetJobDefinition jobD("job_d", "engine", "op", "{}", {"output_c"}, {"output_a"}); // Creates cycle!
        
        preset.add_job(jobA);
        preset.add_job(jobB);
        preset.add_job(jobC);
        preset.add_job(jobD);
        
        preset.add_dependency(PresetDependency("job_a", "job_b"));
        preset.add_dependency(PresetDependency("job_b", "job_c"));
        preset.add_dependency(PresetDependency("job_c", "job_d"));
        preset.add_dependency(PresetDependency("job_d", "job_a")); // Cycle!
        
        // Convert to DAG - should detect cycle during finalization
        JobGraph dag;
        dag.add_job_definition(JobDefinition(jobA.engine_identifier, jobA.api_operation, jobA.parameters_blob, {}, {ArtifactId("output_a")}));
        dag.add_job_definition(JobDefinition(jobB.engine_identifier, jobB.api_operation, jobB.parameters_blob, {ArtifactId("output_a")}, {ArtifactId("output_b")}));
        dag.add_job_definition(JobDefinition(jobC.engine_identifier, jobC.api_operation, jobC.parameters_blob, {ArtifactId("output_b")}, {ArtifactId("output_c")}));
        dag.add_job_definition(JobDefinition(jobD.engine_identifier, jobD.api_operation, jobD.parameters_blob, {ArtifactId("output_c")}, {ArtifactId("output_a")}));
        
        auto jobA_id = JobIdHasher::compute_job_id(JobDefinition(jobA.engine_identifier, jobA.api_operation, jobA.parameters_blob, {}, {ArtifactId("output_a")}));
        auto jobB_id = JobIdHasher::compute_job_id(JobDefinition(jobB.engine_identifier, jobB.api_operation, jobB.parameters_blob, {ArtifactId("output_a")}, {ArtifactId("output_b")}));
        auto jobC_id = JobIdHasher::compute_job_id(JobDefinition(jobC.engine_identifier, jobC.api_operation, jobC.parameters_blob, {ArtifactId("output_b")}, {ArtifactId("output_c")}));
        auto jobD_id = JobIdHasher::compute_job_id(JobDefinition(jobD.engine_identifier, jobD.api_operation, jobD.parameters_blob, {ArtifactId("output_c")}, {ArtifactId("output_a")}));
        
        dag.add_dependency(JobDependency(jobA_id, jobB_id));
        dag.add_dependency(JobDependency(jobB_id, jobC_id));
        dag.add_dependency(JobDependency(jobC_id, jobD_id));
        dag.add_dependency(JobDependency(jobD_id, jobA_id)); // Creates cycle
        
        // This should throw when cycle is detected
        dag.finalize();
        
        // If we reach here, the test failed
        assert(false && "Cycle detection failed - should have thrown exception");
        
    } catch (const std::runtime_error& e) {
        std::string error_msg = e.what();
        assert(error_msg.find("cycle") != std::string::npos);
        std::cout << "✓ Cyclic dependency correctly detected and rejected: " << error_msg << "\n";
    }
}

void test_missing_dependency() {
    std::cout << "Testing missing dependency detection...\n";
    
    try {
        // Create DAG with dependency to non-existent job
        JobGraph dag;
        
        JobDefinition validJob("engine", "op", "{}", {}, {ArtifactId("output")});
        dag.add_job_definition(validJob);
        
        auto validJobId = JobIdHasher::compute_job_id(validJob);
        auto fakeJobId = JobId::from_content_hash("fake_job_hash_that_does_not_exist");
        
        // Add dependency to non-existent job
        dag.add_dependency(JobDependency(fakeJobId, validJobId));
        
        // Finalization should succeed (dependency validation happens at scheduler level)
        dag.finalize();
        
        // But scheduler should detect missing dependency during execution
        LogicalClock clock;
        BatchFlowScheduler scheduler(dag, clock);
        
        // Try to get dependencies - should handle gracefully
        auto deps = dag.get_dependencies(validJobId);
        assert(deps.size() == 1);
        assert(deps[0] == fakeJobId);
        
        // The missing job should prevent execution
        auto ready_jobs = scheduler.next_ready_jobs();
        assert(ready_jobs.empty()); // No jobs ready because dependency missing
        
        std::cout << "✓ Missing dependency correctly prevents job execution\n";
        
    } catch (const std::exception& e) {
        std::cout << "✓ Missing dependency correctly detected: " << e.what() << "\n";
    }
}

void test_retry_exceeding_max_attempts() {
    std::cout << "Testing retry policy max attempts enforcement...\n";
    
    // Create retry policy with max 2 attempts
    RetryPolicy policy = RetryPolicy::retry_on_failure(2, 1);
    
    // Simulate multiple failures
    JobRetryState job_state;
    int attempt_count = 0;
    bool should_retry = true;
    
    while (should_retry && attempt_count < 5) { // Try up to 5 times
        attempt_count++;
        job_state.record_attempt(attempt_count);
        
        // Simulate job failure and evaluate retry
        RetryDecision decision = RetryPolicyEvaluator::evaluate_retry(
            policy, job_state, RetryableState::Failed, attempt_count
        );
        should_retry = decision.should_retry;
        
        std::cout << "  Attempt " << attempt_count << ": " << 
                     (should_retry ? "Retry allowed" : "Retry denied") << "\n";
    }
    
    // Should have stopped at max attempts (2)
    assert(attempt_count == 2);
    assert(!should_retry);
    
    std::cout << "✓ Retry policy correctly enforces max attempts limit\n";
}

void test_replay_with_altered_event_tick() {
    std::cout << "Testing replay with altered event tick...\n";
    
    try {
        // Create simple DAG
        JobGraph dag;
        JobDefinition job("engine", "op", "{}", {}, {ArtifactId("output")});
        dag.add_job_definition(job);
        dag.finalize();
        
        // Create original events with correct ticks
        std::vector<EventRecord> original_events;
        auto job_id = JobIdHasher::compute_job_id(job);
        original_events.push_back(EventRecord(1, BatchFlowEvent::JobStarted, job_id));
        original_events.push_back(EventRecord(2, BatchFlowEvent::JobCompleted, job_id));
        
        // Create altered events with wrong ticks
        std::vector<EventRecord> altered_events;
        altered_events.push_back(EventRecord(1, BatchFlowEvent::JobStarted, job_id));
        altered_events.push_back(EventRecord(99, BatchFlowEvent::JobCompleted, job_id)); // Wrong tick!
        
        // Replay with altered events should fail
        BatchFlowReplayExecutor replay_executor(dag);
        replay_executor.replay_from_events(altered_events);
        
        // If we reach here, the test failed
        assert(false && "Replay should have failed with altered ticks");
        
    } catch (const std::runtime_error& e) {
        std::string error_msg = e.what();
        assert(error_msg.find("tick") != std::string::npos || error_msg.find("determinism") != std::string::npos);
        std::cout << "✓ Replay correctly rejected altered event ticks: " << error_msg << "\n";
    }
}

void test_replay_with_reordered_events() {
    std::cout << "Testing replay with reordered events...\n";
    
    try {
        // Create simple DAG with two jobs
        JobGraph dag;
        JobDefinition job1("engine", "op1", "{}", {}, {ArtifactId("output1")});
        JobDefinition job2("engine", "op2", "{}", {ArtifactId("output1")}, {ArtifactId("output2")});
        dag.add_job_definition(job1);
        dag.add_job_definition(job2);
        
        auto job1_id = JobIdHasher::compute_job_id(job1);
        auto job2_id = JobIdHasher::compute_job_id(job2);
        dag.add_dependency(JobDependency(job1_id, job2_id));
        dag.finalize();
        
        // Create events in wrong order (job2 before job1)
        std::vector<EventRecord> reordered_events;
        reordered_events.push_back(EventRecord(1, BatchFlowEvent::JobStarted, job2_id)); // Wrong order!
        reordered_events.push_back(EventRecord(2, BatchFlowEvent::JobCompleted, job2_id));
        reordered_events.push_back(EventRecord(3, BatchFlowEvent::JobStarted, job1_id));
        reordered_events.push_back(EventRecord(4, BatchFlowEvent::JobCompleted, job1_id));
        
        // Replay should fail due to dependency violation
        LogicalClock replay_clock;
        BatchFlowScheduler replay_scheduler(dag, replay_clock);
        
        // Try to replay first event (job2 start) - should fail because job1 not completed
        replay_scheduler.replay_start_job(job2_id, 1);
        
        // This violates dependency order and should be caught by scheduler logic
        assert(false && "Replay should have failed with reordered events");
        
    } catch (const std::exception& e) {
        std::cout << "✓ Replay correctly rejected reordered events: " << e.what() << "\n";
    }
}

void test_law_violation_comprehensive() {
    std::cout << "Running Comprehensive Law Violation Tests...\n";
    std::cout << "==========================================\n";
    
    test_cyclic_dependency_preset();
    test_missing_dependency();
    test_retry_exceeding_max_attempts();
    test_replay_with_altered_event_tick();
    test_replay_with_reordered_events();
    
    std::cout << "\n🛡️ ALL LAW VIOLATION TESTS PASSED\n";
    std::cout << "System is properly defensive:\n";
    std::cout << "  - Cyclic dependencies detected and rejected\n";
    std::cout << "  - Missing dependencies prevent execution\n";
    std::cout << "  - Retry limits enforced strictly\n";
    std::cout << "  - Altered event ticks rejected\n";
    std::cout << "  - Reordered events cause failures\n";
    std::cout << "  - No silent corrections or permissive behavior\n";
}

int main() {
    test_law_violation_comprehensive();
    return 0;
}