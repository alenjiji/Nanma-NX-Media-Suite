#include "nx/batch/BatchEngineImpl.h"
#include <cassert>

using namespace nx::batch;

void test_deterministic_plan_batch() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true},
        {"nx audio --input test.wav --output test.flac", {"nx", "audio", "--input", "test.wav", "--output", "test.flac"}, true}
    };
    
    auto plan1 = engine.plan_batch(commands);
    auto plan2 = engine.plan_batch(commands);
    
    assert(plan1 == plan2);
    assert(plan1.jobs.size() == 2);
    
    // Verify job IDs are deterministic
    assert(plan1.jobs[0].job_id.value == "job-001");
    assert(plan1.jobs[1].job_id.value == "job-002");
}

void test_job_states() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true},
        {"invalid command", {"invalid", "command"}, false},
        {"nx audio --input test.wav --output test.flac", {"nx", "audio", "--input", "test.wav", "--output", "test.flac"}, true}
    };
    
    auto plan = engine.plan_batch(commands);
    
    assert(plan.jobs.size() == 3);
    assert(plan.jobs[0].state == JobState::Planned);
    assert(plan.jobs[1].state == JobState::Rejected);
    assert(plan.jobs[2].state == JobState::Planned);
}

void test_stable_ordering() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input a.mp4 --output a.mkv", {"nx", "convert", "--input", "a.mp4", "--output", "a.mkv"}, true},
        {"nx audio --input b.wav --output b.flac", {"nx", "audio", "--input", "b.wav", "--output", "b.flac"}, true},
        {"nx video --input c.mp4 --output c.mkv", {"nx", "video", "--input", "c.mp4", "--output", "c.mkv"}, true}
    };
    
    auto plan1 = engine.plan_batch(commands);
    auto plan2 = engine.plan_batch(commands);
    
    assert(plan1.jobs.size() == plan2.jobs.size());
    for (size_t i = 0; i < plan1.jobs.size(); ++i) {
        assert(plan1.jobs[i].job_id == plan2.jobs[i].job_id);
        assert(plan1.jobs[i].command == plan2.jobs[i].command);
        assert(plan1.jobs[i].state == plan2.jobs[i].state);
    }
}

void test_empty_commands() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands;
    auto plan = engine.plan_batch(commands);
    
    assert(plan.jobs.empty());
}

void test_stateless_behavior() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands1 = {
        {"nx convert --input test1.mp4 --output test1.mkv", {"nx", "convert", "--input", "test1.mp4", "--output", "test1.mkv"}, true}
    };
    
    std::vector<ParsedBatchCommand> commands2 = {
        {"nx audio --input test2.wav --output test2.flac", {"nx", "audio", "--input", "test2.wav", "--output", "test2.flac"}, true}
    };
    
    auto plan1 = engine.plan_batch(commands1);
    auto plan2 = engine.plan_batch(commands2);
    
    // Plans should be independent
    assert(plan1.jobs.size() == 1);
    assert(plan2.jobs.size() == 1);
    assert(plan1.jobs[0].command != plan2.jobs[0].command);
    
    // Re-running first plan should be identical
    auto plan1_again = engine.plan_batch(commands1);
    assert(plan1 == plan1_again);
}

int main() {
    test_deterministic_plan_batch();
    test_job_states();
    test_stable_ordering();
    test_empty_commands();
    test_stateless_behavior();
    
    return 0;
}