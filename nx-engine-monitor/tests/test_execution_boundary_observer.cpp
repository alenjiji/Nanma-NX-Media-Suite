#include "nx/monitor/ExecutionBoundaryObserver.h"
#include "nx/batch/BatchEngineImpl.h"
#include <cassert>
#include <vector>

using namespace nx::monitor;
using namespace nx::batch;

// Test implementation of MonitorObserver
class TestMonitorObserver : public MonitorObserver {
public:
    std::vector<SessionCreationEvent> session_events;
    std::vector<ExecutionGraphEvent> graph_events;
    std::vector<SessionId> correlation_sessions;
    
    void observe_session_creation(const SessionCreationEvent& event) override {
        session_events.push_back(event);
    }
    
    void observe_execution_graph_creation(const ExecutionGraphEvent& event) override {
        graph_events.push_back(event);
    }
    
    void prepare_telemetry_correlation(const SessionId& session_id) override {
        correlation_sessions.push_back(session_id);
    }
};

void test_session_creation_observation() {
    TestMonitorObserver test_observer;
    ExecutionBoundaryObserver boundary_observer(&test_observer);
    
    BatchEngineImpl engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true},
        {"nx audio --input test.wav --output test.flac", {"nx", "audio", "--input", "test.wav", "--output", "test.flac"}, true}
    };
    
    auto session = engine.create_session(commands);
    
    // Notify observer of session creation
    boundary_observer.notify_session_created(session);
    
    // Verify observation event was recorded
    assert(test_observer.session_events.size() == 1);
    assert(test_observer.session_events[0].session_id == session.id());
    assert(test_observer.session_events[0].job_count == session.job_count());
    
    // Verify session remains unmodified
    assert(session.job_count() == 2);
    assert(!session.id().value.empty());
}

void test_execution_graph_observation() {
    TestMonitorObserver test_observer;
    ExecutionBoundaryObserver boundary_observer(&test_observer);
    
    BatchEngineImpl engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto execution_graph = engine.create_execution_graph(session);
    
    // Notify observer of execution graph creation
    boundary_observer.notify_execution_graph_created(execution_graph, session.id());
    
    // Verify observation event was recorded
    assert(test_observer.graph_events.size() == 1);
    assert(test_observer.graph_events[0].session_id == session.id());
    assert(test_observer.graph_events[0].node_count == execution_graph.node_count());
    
    // Verify execution graph remains unmodified
    assert(execution_graph.node_count() == 1);
}

void test_telemetry_correlation_setup() {
    TestMonitorObserver test_observer;
    ExecutionBoundaryObserver boundary_observer(&test_observer);
    
    SessionId session_id{"test-session-123"};
    
    // Setup telemetry correlation
    boundary_observer.setup_telemetry_correlation(session_id);
    
    // Verify correlation was prepared
    assert(test_observer.correlation_sessions.size() == 1);
    assert(test_observer.correlation_sessions[0] == session_id);
}

void test_null_observer_safety() {
    ExecutionBoundaryObserver boundary_observer(nullptr);
    
    BatchEngineImpl engine;
    std::vector<ParsedBatchCommand> commands = {
        {"nx convert --input test.mp4 --output test.mkv", {"nx", "convert", "--input", "test.mp4", "--output", "test.mkv"}, true}
    };
    
    auto session = engine.create_session(commands);
    auto execution_graph = engine.create_execution_graph(session);
    
    // Should not crash with null observer
    boundary_observer.notify_session_created(session);
    boundary_observer.notify_execution_graph_created(execution_graph, session.id());
    boundary_observer.setup_telemetry_correlation(session.id());
}

void test_observation_event_equality() {
    SessionId session_id{"test-session"};
    
    SessionCreationEvent event1{session_id, 2};
    SessionCreationEvent event2{session_id, 2};
    SessionCreationEvent event3{session_id, 3};
    
    assert(event1 == event2);
    assert(!(event1 == event3));
    
    ExecutionGraphEvent graph_event1{session_id, 5};
    ExecutionGraphEvent graph_event2{session_id, 5};
    ExecutionGraphEvent graph_event3{session_id, 6};
    
    assert(graph_event1 == graph_event2);
    assert(!(graph_event1 == graph_event3));
}

int main() {
    test_session_creation_observation();
    test_execution_graph_observation();
    test_telemetry_correlation_setup();
    test_null_observer_safety();
    test_observation_event_equality();
    
    return 0;
}