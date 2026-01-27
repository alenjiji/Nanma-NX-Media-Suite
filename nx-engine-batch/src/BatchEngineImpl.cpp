#include "nx/batch/BatchEngineImpl.h"
#include "determinism_guards.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstdint>

namespace nx::batch {

BatchPlan BatchEngineImpl::plan_batch(const std::vector<ParsedBatchCommand>& commands) const {
    BatchPlan plan;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        JobId job_id{generate_job_id(i + 1)};
        JobState state = commands[i].valid ? JobState::Planned : JobState::Rejected;
        
        plan.jobs.push_back(BatchJobSummary{
            .job_id = job_id,
            .command = commands[i].command,
            .state = state
        });
    }
    
    return plan;
}

BatchPlanSession BatchEngineImpl::create_session(const std::vector<ParsedBatchCommand>& commands) const {
    NX_DETERMINISTIC_FUNCTION;
    nx::core::DeterminismGuard::assert_no_time_access();
    
    SessionId session_id = generate_session_id(commands);
    std::vector<SessionJobDescriptor> jobs;
    
    for (size_t i = 0; i < commands.size(); ++i) {
        if (commands[i].valid) {
            SessionJobId job_id = SessionJobId::create_initial(session_id, generate_job_id(i + 1));
            
            jobs.push_back(SessionJobDescriptor{
                .job_id = job_id,
                .command = commands[i].command,
                .arguments = commands[i].arguments,
                .dependencies = {} // No dependencies in Phase 7
            });
        }
    }
    
    return BatchPlanSession(session_id, std::move(jobs));
}

ExecutionInput BatchEngineImpl::prepare_execution(const BatchPlanSession& session) const {
    std::vector<ExecutionJobRef> job_refs;
    
    for (const auto& job : session.jobs()) {
        job_refs.push_back(ExecutionJobRef{
            .job_id = job.job_id
        });
    }
    
    return ExecutionInput(session.id(), std::move(job_refs));
}

ExecutionGraph BatchEngineImpl::create_execution_graph(const BatchPlanSession& session) const {
    NX_DETERMINISTIC_FUNCTION;
    nx::core::DeterminismGuard::assert_session_immutable();
    
    std::vector<ExecutionNode> nodes;
    
    for (const auto& job : session.jobs()) {
        nodes.push_back(ExecutionNode{
            .job_id = job.job_id,           // REFERENCED: Session job identity
            .dependencies = job.dependencies // COPIED: Dependency structure from session
        });
    }
    
    return ExecutionGraph(std::move(nodes));
}

std::string BatchEngineImpl::generate_job_id(size_t index) const {
    std::ostringstream oss;
    oss << "job-" << std::setfill('0') << std::setw(3) << index;
    return oss.str();
}

SessionId BatchEngineImpl::generate_session_id(const std::vector<ParsedBatchCommand>& commands) const {
    NX_DETERMINISTIC_FUNCTION;
    nx::core::DeterminismGuard::assert_no_random_access();
    
    // Deterministic session ID based on command content using fixed-width hash
    uint64_t content_hash = 14695981039346656037ULL; // FNV-1a offset basis
    
    for (const auto& cmd : commands) {
        for (char c : cmd.command) {
            content_hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
            content_hash *= 1099511628211ULL; // FNV-1a prime
        }
    }
    
    std::ostringstream oss;
    oss << std::hex << content_hash;
    return SessionId{oss.str()};
}

} // namespace nx::batch