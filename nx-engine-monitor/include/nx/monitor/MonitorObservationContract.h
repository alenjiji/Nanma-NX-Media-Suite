#pragma once

#include "nx/batch/SessionTypes.h"
#include "nx/batch/BatchPlanSession.h"
#include "nx/batch/ExecutionGraph.h"
#include <vector>

namespace nx::monitor {

/**
 * Session creation observation event
 * 
 * Immutable snapshot of session creation for monitoring purposes.
 * Contains only structural information needed for correlation.
 */
struct SessionCreationEvent {
    nx::batch::SessionId session_id;    // REFERENCED: Session identity for correlation
    size_t job_count;                   // COPIED: Number of jobs in session
    
    bool operator==(const SessionCreationEvent& other) const = default;
};

/**
 * Execution graph creation observation event
 * 
 * Immutable snapshot of execution graph creation for monitoring purposes.
 * Contains only structural information about the execution topology.
 */
struct ExecutionGraphEvent {
    nx::batch::SessionId session_id;    // REFERENCED: Session identity for correlation
    size_t node_count;                  // COPIED: Number of nodes in execution graph
    
    bool operator==(const ExecutionGraphEvent& other) const = default;
};

/**
 * Monitor observation contract for execution boundaries
 * 
 * ONE-WAY DATA FLOW GUARANTEE:
 * - Data flows FROM execution domain TO monitor domain only
 * - Monitor receives immutable snapshots of execution events
 * - Monitor cannot influence or modify execution state
 * - Monitor serves as passive observer for telemetry correlation
 * 
 * WHY MONITOR MUST NEVER HOLD EXECUTION HANDLES:
 * - Execution handles could create hidden coupling between domains
 * - Monitor holding mutable references violates immutability contracts
 * - Execution lifecycle must remain independent of monitoring
 * - Monitor observations must not affect execution determinism
 * 
 * FORBIDDEN OPERATIONS:
 * - Modifying session or execution graph state
 * - Influencing execution decisions or flow
 * - Caching mutable references to execution objects
 * - Creating feedback loops from monitor to execution
 */
class MonitorObserver {
public:
    virtual ~MonitorObserver() = default;
    
    /**
     * Observe session creation event
     * 
     * @param event Immutable session creation snapshot
     * 
     * CONSTRAINTS:
     * - Must not modify session state
     * - Must not influence session lifecycle
     * - Must not cache mutable session references
     * - Used only for telemetry correlation setup
     */
    virtual void observe_session_creation(const SessionCreationEvent& event) = 0;
    
    /**
     * Observe execution graph creation event
     * 
     * @param event Immutable execution graph snapshot
     * 
     * CONSTRAINTS:
     * - Must not modify execution graph state
     * - Must not influence execution planning
     * - Must not cache mutable graph references
     * - Used only for execution topology awareness
     */
    virtual void observe_execution_graph_creation(const ExecutionGraphEvent& event) = 0;
    
    /**
     * Prepare for future telemetry correlation
     * 
     * @param session_id Session identity for future correlation
     * 
     * CONSTRAINTS:
     * - Must not access session data
     * - Must not influence execution
     * - Used only to prepare correlation structures
     */
    virtual void prepare_telemetry_correlation(const nx::batch::SessionId& session_id) = 0;
};

} // namespace nx::monitor