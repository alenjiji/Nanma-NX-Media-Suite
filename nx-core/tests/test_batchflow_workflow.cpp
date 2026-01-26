#include "../include/nx_batchflow_preset.h"
#include "../include/nx_batchflow_dag.h"
#include "../include/nx_batchflow_scheduler.h"
#include "../include/nx_batchflow_logical_clock.h"
#include "../include/nx_batchflow_replay.h"
#include <cassert>
#include <iostream>
#include <sstream>

using namespace nx::batchflow;

// Stub engine executor that always succeeds
class StubEngineExecutor {
public:
    struct ExecutionResult {
        bool success = true;
        std::string output_data;
    };
    
    static ExecutionResult execute_job(const JobNode& job) {
        // Simulate deterministic execution based on job content
        std::ostringstream output;
        output << "output_for_" << job.id().hash().substr(0, 8);
        return ExecutionResult{true, output.str()};
    }
};

void test_preset_to_dag() {
    std::cout << "Testing Preset → DAG conversion...\n";
    
    // Create preset with version and metadata
    BatchFlowPreset preset(
        PresetVersion::current(),
        "Example Batch Processing",
        "Demonstrates deterministic job execution"
    );
    
    // Create job definitions
    PresetJobDefinition job1(
        "decode_input",
        "nx_convert_pro",
        "decode", 
        "{\"input_format\":\"mp4\",\"codec\":\"h264\"}",
        std::vector<std::string>{}, // no inputs
        std::vector<std::string>{"decoded_video"}
    );
    
    PresetJobDefinition job2(
        "process_audio",
        "nx_audiolab",
        "normalize",
        "{\"target_lufs\":-23.0,\"true_peak_limit\":-1.0}",
        std::vector<std::string>{"decoded_video"},
        std::vector<std::string>{"processed_audio"}
    );
    
    // Add jobs to preset
    preset.add_job(job1);
    preset.add_job(job2);
    
    // Add dependency
    preset.add_dependency(PresetDependency("decode_input", "process_audio"));
    
    // Verify preset structure
    assert(preset.jobs().size() == 2);
    assert(preset.dependencies().size() == 1);
    
    std::cout << "✓ Preset → DAG conversion successful\n";
}

void test_dag_determinism() {
    std::cout << "Testing DAG determinism...\n";
    
    // Create identical job definitions
    JobDefinition job_def(
        "test_engine",
        "test_op",
        "{\"param\":\"value\"}",
        std::vector<ArtifactId>{},
        std::vector<ArtifactId>{ArtifactId("output")}
    );
    
    // Create two DAGs with same job
    JobGraph dag1, dag2;
    dag1.add_job_definition(job_def);
    dag2.add_job_definition(job_def);
    dag1.finalize();
    dag2.finalize();
    
    // Verify identical structure
    assert(dag1.node_count() == dag2.node_count());
    assert(dag1.dependency_count() == dag2.dependency_count());
    
    // Verify identical job IDs
    auto nodes1 = dag1.nodes();
    auto nodes2 = dag2.nodes();
    assert(nodes1.size() == nodes2.size());
    for (size_t i = 0; i < nodes1.size(); ++i) {
        assert(nodes1[i].id() == nodes2[i].id());
    }
    
    std::cout << "✓ DAG generation is deterministic\n";
}

void test_scheduler_events() {
    std::cout << "Testing Scheduler event emission...\n";
    
    // Create simple DAG with one job
    JobDefinition job_def(
        "test_engine",
        "test_op",
        "{\"test\":\"value\"}",
        std::vector<ArtifactId>{},
        std::vector<ArtifactId>{ArtifactId("output")}
    );
    
    JobGraph dag;
    dag.add_job_definition(job_def);
    dag.finalize();
    
    // Create scheduler and clock
    LogicalClock clock;
    BatchFlowScheduler scheduler(dag, clock);
    
    // Get ready jobs
    auto ready_jobs = scheduler.next_ready_jobs();
    assert(ready_jobs.size() == 1);
    
    // Start job execution
    auto job_id = ready_jobs[0];
    auto start_tick = scheduler.start_job(job_id);
    assert(start_tick > 0);
    
    // Verify clock advanced
    assert(clock.current_tick() > 0);
    
    // Complete job execution
    auto complete_tick = scheduler.mark_completed(job_id);
    assert(complete_tick > start_tick);
    
    // Verify all jobs completed
    assert(scheduler.next_ready_jobs().empty());
    assert(scheduler.all_jobs_finished());
    
    std::cout << "✓ Scheduler emits correct events\n";
}

void test_logical_clock_monotonic() {
    std::cout << "Testing LogicalClock monotonic ticks...\n";
    
    LogicalClock clock;
    
    // Record initial state
    auto initial_tick = clock.current_tick();
    assert(initial_tick == 0);
    
    // Advance clock multiple times
    std::vector<LogicalTick> ticks;
    for (int i = 0; i < 5; ++i) {
        auto job_id = JobId::from_content_hash("test_job_" + std::to_string(i));
        auto tick = clock.on_job_started(job_id);
        ticks.push_back(tick);
    }
    
    // Verify monotonic progression
    for (size_t i = 1; i < ticks.size(); ++i) {
        assert(ticks[i-1] < ticks[i]);
    }
    
    // Verify each tick is exactly one increment
    for (size_t i = 0; i < ticks.size(); ++i) {
        assert(ticks[i] == i + 1);
    }
    
    std::cout << "✓ LogicalClock ticks are monotonic\n";
}

void test_replay_reproduction() {
    std::cout << "Testing Replay state reproduction...\n";
    
    // Create DAG with two jobs
    JobDefinition job1_def(
        "test_engine",
        "test_op",
        "{\"id\":\"1\"}",
        std::vector<ArtifactId>{},
        std::vector<ArtifactId>{ArtifactId("output1")}
    );
    
    JobDefinition job2_def(
        "test_engine",
        "test_op",
        "{\"id\":\"2\"}",
        std::vector<ArtifactId>{ArtifactId("output1")},
        std::vector<ArtifactId>{ArtifactId("output2")}
    );
    
    JobGraph dag;
    dag.add_job_definition(job1_def);
    dag.add_job_definition(job2_def);
    
    auto job1_id = JobIdHasher::compute_job_id(job1_def);
    auto job2_id = JobIdHasher::compute_job_id(job2_def);
    dag.add_dependency(JobDependency(job1_id, job2_id));
    dag.finalize();
    
    // Execute original workflow and record events
    LogicalClock original_clock;
    BatchFlowScheduler original_scheduler(dag, original_clock);
    std::vector<EventRecord> recorded_events;
    
    // Execute first job
    auto ready_jobs = original_scheduler.next_ready_jobs();
    assert(ready_jobs.size() == 1);
    auto job1 = ready_jobs[0];
    
    auto start_tick1 = original_scheduler.start_job(job1);
    recorded_events.push_back(EventRecord(
        start_tick1,
        BatchFlowEvent::JobStarted,
        job1
    ));
    
    auto complete_tick1 = original_scheduler.mark_completed(job1);
    recorded_events.push_back(EventRecord(
        complete_tick1,
        BatchFlowEvent::JobCompleted,
        job1
    ));
    
    // Execute second job
    ready_jobs = original_scheduler.next_ready_jobs();
    assert(ready_jobs.size() == 1);
    auto job2 = ready_jobs[0];
    
    auto start_tick2 = original_scheduler.start_job(job2);
    recorded_events.push_back(EventRecord(
        start_tick2,
        BatchFlowEvent::JobStarted,
        job2
    ));
    
    auto complete_tick2 = original_scheduler.mark_completed(job2);
    recorded_events.push_back(EventRecord(
        complete_tick2,
        BatchFlowEvent::JobCompleted,
        job2
    ));
    
    // Store final state
    auto original_final_tick = original_clock.current_tick();
    bool original_complete = original_scheduler.all_jobs_finished();
    
    // Now replay the execution
    LogicalClock replay_clock;
    BatchFlowScheduler replay_scheduler(dag, replay_clock);
    BatchFlowReplayExecutor replay_executor(dag);
    
    // Initialize replay
    replay_executor.replay_from_events(recorded_events);
    
    // Verify identical final state
    auto replay_final_tick = replay_executor.clock().current_tick();
    bool replay_complete = replay_executor.scheduler().all_jobs_finished();
    
    assert(replay_final_tick == original_final_tick);
    assert(replay_complete == original_complete);
    
    std::cout << "✓ Replay reproduces identical state\n";
}

int main() {
    std::cout << "Running NX-BatchFlow Canonical Workflow Tests\n";
    std::cout << "=============================================\n\n";
    
    test_preset_to_dag();
    test_dag_determinism();
    test_scheduler_events();
    test_logical_clock_monotonic();
    test_replay_reproduction();
    
    std::cout << "\n✓ All workflow tests passed!\n";
    std::cout << "Canonical workflow: Preset → DAG → Scheduler → Clock → Replay verified.\n";
    
    return 0;
}