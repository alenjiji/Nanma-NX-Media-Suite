#include "nx/monitor/NullMonitorEngine.h"
#include <iostream>
#include <cassert>

namespace nx::monitor {

void test_status_deterministic() {
    std::cout << "Testing status() deterministic behavior...\n";
    
    NullMonitorEngine engine;
    
    auto status1 = engine.status();
    auto status2 = engine.status();
    
    // Must be identical
    assert(status1.healthy == status2.healthy);
    assert(status1.active_jobs == status2.active_jobs);
    assert(status1.completed_jobs == status2.completed_jobs);
    
    // Must match specification
    assert(status1.healthy == true);
    assert(status1.active_jobs == 0);
    assert(status1.completed_jobs == 0);
    
    std::cout << "✓ Status deterministic tests passed\n";
}

void test_jobs_empty() {
    std::cout << "Testing jobs() returns empty...\n";
    
    NullMonitorEngine engine;
    
    auto jobs1 = engine.jobs();
    auto jobs2 = engine.jobs();
    
    // Must be empty
    assert(jobs1.empty());
    assert(jobs2.empty());
    
    // Must be identical
    assert(jobs1.size() == jobs2.size());
    
    std::cout << "✓ Jobs empty tests passed\n";
}

void test_job_always_nullopt() {
    std::cout << "Testing job() always returns nullopt...\n";
    
    NullMonitorEngine engine;
    
    // Test various job IDs
    std::vector<std::string> test_ids = {
        "job-123", "nonexistent", "", "test-job", "abc123"
    };
    
    for (const auto& id : test_ids) {
        auto result1 = engine.job(id);
        auto result2 = engine.job(id);
        
        // Must be nullopt
        assert(!result1.has_value());
        assert(!result2.has_value());
        
        // Must be identical (both nullopt)
        assert(result1.has_value() == result2.has_value());
    }
    
    std::cout << "✓ Job nullopt tests passed\n";
}

void test_engines_deterministic() {
    std::cout << "Testing engines() deterministic list...\n";
    
    NullMonitorEngine engine;
    
    auto engines1 = engine.engines();
    auto engines2 = engine.engines();
    
    // Must be identical
    assert(engines1.size() == engines2.size());
    assert(engines1.size() == 5);
    
    // Verify exact content and order
    std::vector<std::string> expected_names = {
        "NX-Convert Pro", "NX-AudioLab", "NX-VideoTrans", "NX-MetaFix", "NX-BatchFlow"
    };
    
    for (size_t i = 0; i < engines1.size(); ++i) {
        assert(engines1[i].name == engines2[i].name);
        assert(engines1[i].version == engines2[i].version);
        assert(engines1[i].available == engines2[i].available);
        
        assert(engines1[i].name == expected_names[i]);
        assert(engines1[i].version == "1.0.0");
        assert(engines1[i].available == true);
    }
    
    std::cout << "✓ Engines deterministic tests passed\n";
}

void test_version_static() {
    std::cout << "Testing version() static metadata...\n";
    
    NullMonitorEngine engine;
    
    auto version1 = engine.version();
    auto version2 = engine.version();
    
    // Must be identical
    assert(version1.name == version2.name);
    assert(version1.version == version2.version);
    assert(version1.build_id == version2.build_id);
    
    // Must match specification
    assert(version1.name == "NX-Monitor");
    assert(version1.version == "0.1.0");
    assert(version1.build_id == "null-engine");
    
    std::cout << "✓ Version static tests passed\n";
}

void test_contract_compliance() {
    std::cout << "Testing contract compliance...\n";
    
    NullMonitorEngine engine;
    
    // Verify all methods can be called without exceptions
    auto status = engine.status();
    auto jobs = engine.jobs();
    auto job = engine.job("test");
    auto engines = engine.engines();
    auto version = engine.version();
    
    // Verify types are correct
    static_assert(std::is_same_v<decltype(status), SystemStatus>);
    static_assert(std::is_same_v<decltype(jobs), std::vector<JobSummary>>);
    static_assert(std::is_same_v<decltype(job), std::optional<JobDetail>>);
    static_assert(std::is_same_v<decltype(engines), std::vector<EngineInfo>>);
    static_assert(std::is_same_v<decltype(version), EngineVersion>);
    
    std::cout << "✓ Contract compliance tests passed\n";
}

void test_multiple_instances() {
    std::cout << "Testing multiple instances produce identical results...\n";
    
    NullMonitorEngine engine1;
    NullMonitorEngine engine2;
    
    // Status
    auto status1 = engine1.status();
    auto status2 = engine2.status();
    assert(status1.healthy == status2.healthy);
    assert(status1.active_jobs == status2.active_jobs);
    assert(status1.completed_jobs == status2.completed_jobs);
    
    // Jobs
    auto jobs1 = engine1.jobs();
    auto jobs2 = engine2.jobs();
    assert(jobs1.size() == jobs2.size());
    
    // Engines
    auto engines1 = engine1.engines();
    auto engines2 = engine2.engines();
    assert(engines1.size() == engines2.size());
    for (size_t i = 0; i < engines1.size(); ++i) {
        assert(engines1[i].name == engines2[i].name);
        assert(engines1[i].version == engines2[i].version);
        assert(engines1[i].available == engines2[i].available);
    }
    
    // Version
    auto version1 = engine1.version();
    auto version2 = engine2.version();
    assert(version1.name == version2.name);
    assert(version1.version == version2.version);
    assert(version1.build_id == version2.build_id);
    
    std::cout << "✓ Multiple instances tests passed\n";
}

} // namespace nx::monitor

int main() {
    std::cout << "Running NX Monitor Engine tests...\n\n";
    
    nx::monitor::test_status_deterministic();
    nx::monitor::test_jobs_empty();
    nx::monitor::test_job_always_nullopt();
    nx::monitor::test_engines_deterministic();
    nx::monitor::test_version_static();
    nx::monitor::test_contract_compliance();
    nx::monitor::test_multiple_instances();
    
    std::cout << "\n✅ All NX Monitor Engine tests passed!\n";
    return 0;
}