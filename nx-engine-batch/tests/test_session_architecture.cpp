#include "nx/batch/BatchEngineImpl.h"
#include <cassert>

using namespace nx::batch;

void test_session_creation() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true},
        {"nx audio --input test.wav --output test.flac", {"nx", "audio", "--input", "test.wav", "--output", "test.flac"}, true}
    };
    
    auto session = engine.create_session(commands);
    
    assert(session.job_count() == 2);
    assert(!session.id().value.empty());
    
    auto jobs = session.jobs();
    assert(jobs.size() == 2);
    assert(jobs[0].job_id.session == session.id());
    assert(jobs[1].job_id.session == session.id());
}

void test_deterministic_construction() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session1 = engine.create_session(commands);
    auto session2 = engine.create_session(commands);
    
    // Same input should produce identical sessions
    assert(session1 == session2);
    assert(session1.id() == session2.id());
}

void test_execution_input_preparation() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto execution_input = engine.prepare_execution(session);
    
    assert(execution_input.session_id() == session.id());
    assert(execution_input.job_count() == session.job_count());
    
    auto job_refs = execution_input.job_refs();
    auto session_jobs = session.jobs();
    
    assert(job_refs.size() == session_jobs.size());
    assert(job_refs[0].job_id == session_jobs[0].job_id);
}

void test_session_job_lookup() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto jobs = session.jobs();
    
    auto job_detail = session.job(jobs[0].job_id);
    assert(job_detail.has_value());
    assert(job_detail->job_id == jobs[0].job_id);
    
    SessionJobId invalid_id{session.id(), "invalid"};
    auto missing_job = session.job(invalid_id);
    assert(!missing_job.has_value());
}

void test_rejected_commands_excluded() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true},
        {"invalid command", {"invalid", "command"}, false}
    };
    
    auto session = engine.create_session(commands);
    
    // Only valid commands should be in session
    assert(session.job_count() == 1);
    
    auto jobs = session.jobs();
    assert(jobs.size() == 1);
    assert(jobs[0].command == "nx convert --input test.mp4 --output test.mkv");
}

void test_execution_graph_creation() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true},
        {"nx audio --input test.wav --output test.flac", {"nx", "audio", "--input", "test.wav", "--output", "test.flac"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto execution_graph = engine.create_execution_graph(session);
    
    assert(execution_graph.node_count() == session.job_count());
    
    auto nodes = execution_graph.nodes();
    auto session_jobs = session.jobs();
    
    // Verify structural mirror
    assert(nodes.size() == session_jobs.size());
    for (size_t i = 0; i < nodes.size(); ++i) {
        assert(nodes[i].job_id == session_jobs[i].job_id);
        assert(nodes[i].dependencies == session_jobs[i].dependencies);
    }
}

void test_execution_graph_node_lookup() {
    BatchEngineImpl engine;
    
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto execution_graph = engine.create_execution_graph(session);
    
    auto session_jobs = session.jobs();
    auto node = execution_graph.node(session_jobs[0].job_id);
    
    assert(node.has_value());
    assert(node->job_id == session_jobs[0].job_id);
    
    SessionJobId invalid_id{session.id(), "invalid"};
    auto missing_node = execution_graph.node(invalid_id);
    assert(!missing_node.has_value());
}

int main() {
    test_session_creation();
    test_deterministic_construction();
    test_execution_input_preparation();
    test_session_job_lookup();
    test_rejected_commands_excluded();
    test_execution_graph_creation();
    test_execution_graph_node_lookup();
    
    return 0;
}