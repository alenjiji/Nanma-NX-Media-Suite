#include "nx/batch/ExecutionResultEnvelope.h"
#include <cassert>

using namespace nx::batch;

void test_output_descriptor_structure() {
    OutputDescriptor desc{
        .path = "/output/test.mkv",
        .type = "video"
    };
    
    assert(desc.path == "/output/test.mkv");
    assert(desc.type == "video");
    
    // Test equality
    OutputDescriptor desc2{"/output/test.mkv", "video"};
    assert(desc == desc2);
    
    OutputDescriptor desc3{"/output/test.mkv", "audio"};
    assert(!(desc == desc3));
}

void test_verification_artifact_structure() {
    VerificationArtifact artifact{
        .artifact_id = "checksum-001",
        .artifact_type = "checksum"
    };
    
    assert(artifact.artifact_id == "checksum-001");
    assert(artifact.artifact_type == "checksum");
    
    // Test equality
    VerificationArtifact artifact2{"checksum-001", "checksum"};
    assert(artifact == artifact2);
    
    VerificationArtifact artifact3{"checksum-001", "format"};
    assert(!(artifact == artifact3));
}

void test_execution_result_envelope_structure() {
    SessionId session_id{"test-session"};
    SessionJobId job_id{session_id, "job-001"};
    
    std::vector<OutputDescriptor> outputs = {
        {"/output/test.mkv", "video"},
        {"/output/test.wav", "audio"}
    };
    
    std::vector<VerificationArtifact> artifacts = {
        {"checksum-001", "checksum"},
        {"format-001", "format"}
    };
    
    ExecutionResultEnvelope envelope{
        .session_id = session_id,
        .job_id = job_id,
        .declared_outputs = outputs,
        .verification_placeholders = artifacts
    };
    
    assert(envelope.session_id == session_id);
    assert(envelope.job_id == job_id);
    assert(envelope.declared_outputs.size() == 2);
    assert(envelope.verification_placeholders.size() == 2);
    
    assert(envelope.declared_outputs[0].path == "/output/test.mkv");
    assert(envelope.declared_outputs[0].type == "video");
    assert(envelope.declared_outputs[1].path == "/output/test.wav");
    assert(envelope.declared_outputs[1].type == "audio");
    
    assert(envelope.verification_placeholders[0].artifact_id == "checksum-001");
    assert(envelope.verification_placeholders[0].artifact_type == "checksum");
}

void test_execution_result_envelope_equality() {
    SessionId session_id{"test-session"};
    SessionJobId job_id{session_id, "job-001"};
    
    std::vector<OutputDescriptor> outputs = {
        {"/output/test.mkv", "video"}
    };
    
    std::vector<VerificationArtifact> artifacts = {
        {"checksum-001", "checksum"}
    };
    
    ExecutionResultEnvelope envelope1{session_id, job_id, outputs, artifacts};
    ExecutionResultEnvelope envelope2{session_id, job_id, outputs, artifacts};
    
    assert(envelope1 == envelope2);
    
    // Different outputs should not be equal
    std::vector<OutputDescriptor> different_outputs = {
        {"/output/different.mkv", "video"}
    };
    
    ExecutionResultEnvelope envelope3{session_id, job_id, different_outputs, artifacts};
    assert(!(envelope1 == envelope3));
}

void test_empty_collections_allowed() {
    SessionId session_id{"test-session"};
    SessionJobId job_id{session_id, "job-001"};
    
    // Empty outputs and artifacts should be allowed
    ExecutionResultEnvelope envelope{
        .session_id = session_id,
        .job_id = job_id,
        .declared_outputs = {},
        .verification_placeholders = {}
    };
    
    assert(envelope.declared_outputs.empty());
    assert(envelope.verification_placeholders.empty());
    assert(envelope.session_id == session_id);
    assert(envelope.job_id == job_id);
}

int main() {
    test_output_descriptor_structure();
    test_verification_artifact_structure();
    test_execution_result_envelope_structure();
    test_execution_result_envelope_equality();
    test_empty_collections_allowed();
    
    return 0;
}