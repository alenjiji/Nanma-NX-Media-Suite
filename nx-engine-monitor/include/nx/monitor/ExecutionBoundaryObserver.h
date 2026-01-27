#pragma once

#include "MonitorObservationContract.h"
#include "nx/batch/BatchPlanSession.h"
#include "nx/batch/ExecutionGraph.h"

namespace nx::monitor {

/**
 * Execution boundary observer for monitoring
 * 
 * Provides read-only observation of execution boundary events.
 * Maintains strict separation between monitoring and execution domains.
 * 
 * ARCHITECTURAL GUARANTEES:
 * - No execution state mutation
 * - No execution flow influence
 * - No mutable reference caching
 * - Pure observation for telemetry correlation
 */
class ExecutionBoundaryObserver {
public:
    explicit ExecutionBoundaryObserver(MonitorObserver* observer);
    
    /**
     * Notify monitor of session creation
     * 
     * @param session Immutable session for observation
     * 
     * GUARANTEES:
     * - Session remains unmodified
     * - Only structural data extracted for monitoring
     * - No session lifecycle influence
     */
    void notify_session_created(const nx::batch::BatchPlanSession& session);
    
    /**
     * Notify monitor of execution graph creation
     * 
     * @param graph Immutable execution graph for observation
     * @param session_id Associated session identity
     * 
     * GUARANTEES:
     * - Execution graph remains unmodified
     * - Only structural data extracted for monitoring
     * - No execution planning influence
     */
    void notify_execution_graph_created(const nx::batch::ExecutionGraph& graph, 
                                       const nx::batch::SessionId& session_id);
    
    /**
     * Setup telemetry correlation for session
     * 
     * @param session_id Session identity for future correlation
     * 
     * GUARANTEES:
     * - No session data access
     * - No execution influence
     * - Pure correlation preparation
     */
    void setup_telemetry_correlation(const nx::batch::SessionId& session_id);

private:
    MonitorObserver* observer_;  // NON-OWNING: Observer for notifications
};

} // namespace nx::monitor