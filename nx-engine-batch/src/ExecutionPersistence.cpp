#include "nx/batch/ExecutionPersistence.h"

namespace nx::batch {

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