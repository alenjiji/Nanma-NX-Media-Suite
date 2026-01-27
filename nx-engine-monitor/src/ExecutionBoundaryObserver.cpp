#include "nx/monitor/ExecutionBoundaryObserver.h"

namespace nx::monitor {

ExecutionBoundaryObserver::ExecutionBoundaryObserver(MonitorObserver* observer)
    : observer_(observer) {
}

void ExecutionBoundaryObserver::notify_session_created(const nx::batch::BatchPlanSession& session) {
    if (observer_) {
        SessionCreationEvent event{
            .session_id = session.id(),
            .job_count = session.job_count()
        };
        
        observer_->observe_session_creation(event);
    }
}

void ExecutionBoundaryObserver::notify_execution_graph_created(const nx::batch::ExecutionGraph& graph, 
                                                              const nx::batch::SessionId& session_id) {
    if (observer_) {
        ExecutionGraphEvent event{
            .session_id = session_id,
            .node_count = graph.node_count()
        };
        
        observer_->observe_execution_graph_creation(event);
    }
}

void ExecutionBoundaryObserver::setup_telemetry_correlation(const nx::batch::SessionId& session_id) {
    if (observer_) {
        observer_->prepare_telemetry_correlation(session_id);
    }
}

} // namespace nx::monitor