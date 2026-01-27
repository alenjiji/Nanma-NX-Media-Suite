#pragma once

namespace nx::cli {

enum class CommandId {
    MonitorStatus,
    MonitorJobs,
    MonitorJob,
    BatchStatus,
    BatchSession,
    BatchJobs,
    Invalid
};

} // namespace nx::cli