#include "nx/batch/BatchEngineImpl.h"
#include <cassert>

using namespace nx::batch;

void test_deterministic_plan_batch() {
    BatchEngineImpl engine;
    
    auto plan1 = engine.plan_batch("test.batch");
    auto plan2 = engine.plan_batch("test.batch");
    
    assert(plan1 == plan2);
    assert(plan1.jobs.empty());
}

void test_deterministic_jobs() {
    BatchEngineImpl engine;
    
    auto jobs1 = engine.jobs();
    auto jobs2 = engine.jobs();
    
    assert(jobs1 == jobs2);
    assert(jobs1.empty());
}

void test_deterministic_job_lookup() {
    BatchEngineImpl engine;
    
    JobId test_id{"test"};
    auto job1 = engine.job(test_id);
    auto job2 = engine.job(test_id);
    
    assert(job1 == job2);
    assert(!job1.has_value());
}

void test_no_side_effects() {
    BatchEngineImpl engine;
    
    // Multiple calls should not change behavior
    engine.plan_batch("file1.batch");
    engine.plan_batch("file2.batch");
    engine.jobs();
    engine.job(JobId{"any"});
    
    // Verify still empty
    assert(engine.jobs().empty());
    assert(!engine.job(JobId{"test"}).has_value());
}

void test_job_id_equality() {
    JobId id1{"test"};
    JobId id2{"test"};
    JobId id3{"other"};
    
    assert(id1 == id2);
    assert(!(id1 == id3));
}

int main() {
    test_deterministic_plan_batch();
    test_deterministic_jobs();
    test_deterministic_job_lookup();
    test_no_side_effects();
    test_job_id_equality();
    
    return 0;
}