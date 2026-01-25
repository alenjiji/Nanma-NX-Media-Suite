#include "../include/identity.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace nx::core;

void test_determinism() {
    std::cout << "Testing determinism...\n";
    
    // Same input must produce same ID
    JobID job1 = JobID::from_content("transcode|input.mp4|h264|1920x1080");
    JobID job2 = JobID::from_content("transcode|input.mp4|h264|1920x1080");
    assert(job1 == job2);
    assert(job1.to_string() == job2.to_string());
    
    RunID run1 = RunID::from_content("batch_v1.0|preset_broadcast|config_a");
    RunID run2 = RunID::from_content("batch_v1.0|preset_broadcast|config_a");
    assert(run1 == run2);
    assert(run1.to_string() == run2.to_string());
    
    NodeID node1 = NodeID::from_content("decode|h264|baseline_profile");
    NodeID node2 = NodeID::from_content("decode|h264|baseline_profile");
    assert(node1 == node2);
    assert(node1.to_string() == node2.to_string());
    
    ArtifactID artifact1 = ArtifactID::from_content("report|conversion|job123|success");
    ArtifactID artifact2 = ArtifactID::from_content("report|conversion|job123|success");
    assert(artifact1 == artifact2);
    assert(artifact1.to_string() == artifact2.to_string());
    
    std::cout << "✓ Same inputs produce identical IDs\n";
}

void test_uniqueness() {
    std::cout << "Testing uniqueness...\n";
    
    // Different inputs must produce different IDs
    JobID job1 = JobID::from_content("transcode|input.mp4|h264|1920x1080");
    JobID job2 = JobID::from_content("transcode|input.mp4|h264|1280x720");
    assert(job1 != job2);
    assert(job1.to_string() != job2.to_string());
    
    RunID run1 = RunID::from_content("batch_v1.0|preset_broadcast|config_a");
    RunID run2 = RunID::from_content("batch_v1.0|preset_broadcast|config_b");
    assert(run1 != run2);
    assert(run1.to_string() != run2.to_string());
    
    NodeID node1 = NodeID::from_content("decode|h264|baseline_profile");
    NodeID node2 = NodeID::from_content("encode|h264|baseline_profile");
    assert(node1 != node2);
    assert(node1.to_string() != node2.to_string());
    
    ArtifactID artifact1 = ArtifactID::from_content("report|conversion|job123|success");
    ArtifactID artifact2 = ArtifactID::from_content("report|conversion|job123|failure");
    assert(artifact1 != artifact2);
    assert(artifact1.to_string() != artifact2.to_string());
    
    std::cout << "✓ Different inputs produce different IDs\n";
}

void test_ordering() {
    std::cout << "Testing ordering...\n";
    
    JobID job1 = JobID::from_content("a");
    JobID job2 = JobID::from_content("b");
    JobID job3 = JobID::from_content("c");
    
    // Ordering must be consistent
    bool order1 = job1 < job2;
    bool order2 = job2 < job3;
    bool order3 = job1 < job3;
    
    // Verify transitivity
    if (order1 && order2) {
        assert(order3); // If a < b and b < c, then a < c
    }
    
    std::cout << "✓ Ordering is consistent\n";
}

void test_serialization() {
    std::cout << "Testing serialization...\n";
    
    JobID job = JobID::from_content("test_content");
    std::string serialized = job.to_string();
    
    // Verify hex string format (64 hex characters for 256-bit hash)
    assert(serialized.length() == 64);
    
    // Verify only hex characters
    for (char c : serialized) {
        assert((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'));
    }
    
    std::cout << "✓ Serialization produces valid hex strings\n";
}

void test_examples() {
    std::cout << "Testing documented examples...\n";
    
    // JobID example from header
    JobID id1 = JobID::from_content("transcode|input.mp4|h264|1920x1080");
    JobID id2 = JobID::from_content("transcode|input.mp4|h264|1920x1080");
    assert(id1 == id2); // Same content → same ID
    
    // RunID example from header
    RunID rid1 = RunID::from_content("batch_v1.0|preset_broadcast|2024-01-01");
    RunID rid2 = RunID::from_content("batch_v1.0|preset_broadcast|2024-01-01");
    assert(rid1 == rid2); // Same content → same ID
    
    // NodeID example from header
    NodeID nid1 = NodeID::from_content("decode|h264|baseline_profile");
    NodeID nid2 = NodeID::from_content("decode|h264|baseline_profile");
    assert(nid1 == nid2); // Same content → same ID
    
    // ArtifactID example from header
    ArtifactID aid1 = ArtifactID::from_content("report|conversion|job123|success");
    ArtifactID aid2 = ArtifactID::from_content("report|conversion|job123|success");
    assert(aid1 == aid2); // Same content → same ID
    
    std::cout << "✓ All documented examples work correctly\n";
}

int main() {
    std::cout << "Running NX-Core Identity Tests\n";
    std::cout << "==============================\n";
    
    test_determinism();
    test_uniqueness();
    test_ordering();
    test_serialization();
    test_examples();
    
    std::cout << "\n✓ All tests passed!\n";
    std::cout << "Deterministic identity system validated.\n";
    
    return 0;
}