#include "../include/nx_batchflow_preset.h"
#include "../include/nx_batchflow_dag.h"
#include "../include/nx_batchflow_scheduler.h"
#include "../include/nx_batchflow_logical_clock.h"
#include "../include/nx_batchflow_replay.h"
#include <cassert>
#include <iostream>

using namespace nx::batchflow;

// Stub engine executor for deterministic testing
class DeterministicStubExecutor {
public:
    static bool execute_job(const JobNode& job) {
        // Always succeeds with deterministic behavior
        return true;
    }
};

// Execute complete workflow and return results
struct WorkflowResults {
    std::vector<JobId> dag_job_ids;
    std::vector<EventRecord> event_records;
    std::map<JobId, JobStatus> final_job_statuses;
    LogicalTick final_tick;
    bool replay_success;
};

WorkflowResults execute_workflow() {
    // Create identical preset
    BatchFlowPreset preset(
        PresetVersion::current(),
        "Determinism Test Preset",
        "Test preset for determinism validation"
    );
    
    // Add identical jobs
    PresetJobDefinition job1(
        "job_alpha",
        "test_engine",
        "test_operation",
        "{\"param\":\"value_alpha\"}",
        std::vector<std::string>{},
        std::vector<std::string>{"output_alpha"}
    );
    
    PresetJobDefinition job2(
        "job_beta", 
        "test_engine",
        "test_operation",
        "{\"param\":\"value_beta\"}",
        std::vector<std::string>{"output_alpha"},
        std::vector<std::string>{"output_beta"}
    );
    
    preset.add_job(job1);
    preset.add_job(job2);
    preset.add_dependency(PresetDependency("job_alpha", "job_beta"));
    
    // Convert to DAG
    JobGraph dag;
    dag.add_job_definition(JobDefinition(
        job1.engine_identifier,
        job1.api_operation,
        job1.parameters_blob,
        std::vector<ArtifactId>{},
        std::vector<ArtifactId>{ArtifactId("output_alpha")}
    ));
    dag.add_job_definition(JobDefinition(
        job2.engine_identifier,
        job2.api_operation,
        job2.parameters_blob,
        std::vector<ArtifactId>{ArtifactId("output_alpha")},
        std::vector<ArtifactId>{ArtifactId("output_beta")}
    ));
    
    // Add dependency
    auto job1_id = JobIdHasher::compute_job_id(JobDefinition(
        job1.engine_identifier, job1.api_operation, job1.parameters_blob,
        std::vector<ArtifactId>{}, std::vector<ArtifactId>{ArtifactId("output_alpha")}
    ));
    auto job2_id = JobIdHasher::compute_job_id(JobDefinition(
        job2.engine_identifier, job2.api_operation, job2.parameters_blob,
        std::vector<ArtifactId>{ArtifactId("output_alpha")}, std::vector<ArtifactId>{ArtifactId("output_beta")}
    ));
    
    dag.add_dependency(JobDependency(job1_id, job2_id));
    dag.finalize();
    
    // Execute with scheduler and clock
    LogicalClock clock;
    BatchFlowScheduler scheduler(dag, clock);
    std::vector<EventRecord> events;
    
    // Execute job1
    auto ready_jobs = scheduler.next_ready_jobs();
    assert(ready_jobs.size() == 1);
    auto job1_actual = ready_jobs[0];
    
    auto start_tick1 = scheduler.start_job(job1_actual);
    events.push_back(EventRecord(start_tick1, BatchFlowEvent::JobStarted, job1_actual));
    
    bool success1 = DeterministicStubExecutor::execute_job(*dag.get_node(job1_actual));
    auto complete_tick1 = scheduler.mark_completed(job1_actual);
    events.push_back(EventRecord(complete_tick1, BatchFlowEvent::JobCompleted, job1_actual));
    
    // Execute job2
    ready_jobs = scheduler.next_ready_jobs();
    assert(ready_jobs.size() == 1);
    auto job2_actual = ready_jobs[0];
    
    auto start_tick2 = scheduler.start_job(job2_actual);
    events.push_back(EventRecord(start_tick2, BatchFlowEvent::JobStarted, job2_actual));
    
    bool success2 = DeterministicStubExecutor::execute_job(*dag.get_node(job2_actual));
    auto complete_tick2 = scheduler.mark_completed(job2_actual);
    events.push_back(EventRecord(complete_tick2, BatchFlowEvent::JobCompleted, job2_actual));
    
    // Test replay
    BatchFlowReplayExecutor replay_executor(dag);
    replay_executor.replay_from_events(events);
    
    // Collect results
    WorkflowResults results;
    results.dag_job_ids = {job1_id, job2_id};
    results.event_records = events;
    results.final_job_statuses = scheduler.get_all_statuses();
    results.final_tick = clock.current_tick();
    results.replay_success = replay_executor.scheduler().all_jobs_finished() && 
                            replay_executor.clock().current_tick() == results.final_tick;
    
    return results;
}

void test_determinism_critical() {
    std::cout << "Running Critical Determinism Test...\n";
    std::cout << "===================================\n";
    
    // Execute workflow twice with fresh state
    std::cout << "Executing workflow - Run 1...\n";
    WorkflowResults run1 = execute_workflow();
    
    std::cout << "Executing workflow - Run 2...\n";
    WorkflowResults run2 = execute_workflow();
    
    // Assert: DAG node IDs identical
    std::cout << "Verifying DAG node IDs identical...\n";
    assert(run1.dag_job_ids.size() == run2.dag_job_ids.size());
    for (size_t i = 0; i < run1.dag_job_ids.size(); ++i) {
        assert(run1.dag_job_ids[i] == run2.dag_job_ids[i]);
    }
    std::cout << "✓ DAG node IDs are identical\n";
    
    // Assert: EventRecord streams identical
    std::cout << "Verifying EventRecord streams identical...\n";
    assert(run1.event_records.size() == run2.event_records.size());
    for (size_t i = 0; i < run1.event_records.size(); ++i) {
        assert(run1.event_records[i] == run2.event_records[i]);
    }
    std::cout << "✓ EventRecord streams are identical\n";
    
    // Assert: Final JobStatus identical
    std::cout << "Verifying final JobStatus identical...\n";
    assert(run1.final_job_statuses == run2.final_job_statuses);
    std::cout << "✓ Final JobStatus are identical\n";
    
    // Assert: Replay succeeds without divergence
    std::cout << "Verifying replay success...\n";
    assert(run1.replay_success);
    assert(run2.replay_success);
    assert(run1.final_tick == run2.final_tick);
    std::cout << "✓ Replay succeeds without divergence\n";
    
    std::cout << "\n🎉 CRITICAL DETERMINISM TEST PASSED\n";
    std::cout << "All assertions verified:\n";
    std::cout << "  - DAG node IDs identical across runs\n";
    std::cout << "  - EventRecord streams identical across runs\n";
    std::cout << "  - Final JobStatus identical across runs\n";
    std::cout << "  - Replay reproduces identical state\n";
    std::cout << "  - No divergence detected\n";
}

int main() {
    test_determinism_critical();
    return 0;
}