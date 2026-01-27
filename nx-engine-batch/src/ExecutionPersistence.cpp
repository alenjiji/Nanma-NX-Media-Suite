#include "nx/batch/ExecutionPersistence.h"

namespace nx::batch {

ExecutionRecord ExecutionRecord::create(
    SessionJobId attempt_id,
    std::optional<SessionJobId> parent_attempt_id,
    uint32_t retry_index,
    JobExecutionSpec intent,
    ExecutionOutcome outcome
) {
    return ExecutionRecord{
        .attempt_id = std::move(attempt_id),
        .parent_attempt_id = std::move(parent_attempt_id),
        .retry_index = retry_index,
        .intent = std::move(intent),
        .outcome = outcome
    };
}

void InMemoryExecutionRecorder::record(const ExecutionRecord& record) {
    records_.push_back(record);
}

const std::vector<ExecutionRecord>& InMemoryExecutionRecorder::get_records() const {
    return records_;
}

void InMemoryExecutionRecorder::clear() {
    records_.clear();
}

InMemoryExecutionReplaySource::InMemoryExecutionReplaySource(std::vector<ExecutionRecord> records)
    : records_(std::move(records)) {
}

std::vector<ExecutionRecord> InMemoryExecutionReplaySource::load_all() const {
    return records_;
}

} // namespace nx::batch