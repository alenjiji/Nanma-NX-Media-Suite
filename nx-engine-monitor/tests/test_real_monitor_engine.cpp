#include "nx/monitor/RealMonitorEngine.h"
#include <cassert>
#include <algorithm>

using namespace nx::monitor;

void test_deterministic_status() {
    RealMonitorEngine engine;
    
    auto status1 = engine.status();
    auto status2 = engine.status();
    
    assert(status1.healthy == status2.healthy);
    assert(status1.active_jobs == status2.active_jobs);
    assert(status1.completed_jobs == status2.completed_jobs);
}

void test_deterministic_engines() {
    RealMonitorEngine engine;
    
    auto engines1 = engine.engines();
    auto engines2 = engine.engines();
    
    assert(engines1.size() == engines2.size());
    
    for (size_t i = 0; i < engines1.size(); ++i) {
        assert(engines1[i].name == engines2[i].name);
        assert(engines1[i].version == engines2[i].version);
        assert(engines1[i].available == engines2[i].available);
    }
}

void test_stable_engine_ordering() {
    RealMonitorEngine engine;
    
    auto engines = engine.engines();
    
    // Verify expected engines in stable order
    assert(engines.size() == 4);
    assert(engines[0].name == "NX-Convert Pro");
    assert(engines[1].name == "NX-AudioLab");
    assert(engines[2].name == "NX-VideoTrans");
    assert(engines[3].name == "NX-MetaFix");
}

void test_empty_jobs() {
    RealMonitorEngine engine;
    
    auto jobs = engine.jobs();
    assert(jobs.empty());
    
    auto job_detail = engine.job("nonexistent");
    assert(!job_detail.has_value());
}

void test_version_info() {
    RealMonitorEngine engine;
    
    auto version = engine.version();
    assert(version.name == "NX-Monitor");
    assert(version.version == "1.0.0");
    assert(version.build_id == "real-engine");
}

void test_system_status_aggregation() {
    RealMonitorEngine engine;
    
    auto status = engine.status();
    
    // Phase 1.A engines have no failure modes, should be healthy
    assert(status.healthy == true);
    
    // Phase 1.A engines have no job APIs, counts should be 0
    assert(status.active_jobs == 0);
    assert(status.completed_jobs == 0);
}

int main() {
    test_deterministic_status();
    test_deterministic_engines();
    test_stable_engine_ordering();
    test_empty_jobs();
    test_version_info();
    test_system_status_aggregation();
    
    return 0;
}