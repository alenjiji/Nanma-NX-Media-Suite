#pragma once

namespace nx::monitor {
    class MonitorError;
}

namespace nx::cli::error {

enum class CliError {
    Success,
    InvalidCommand,
    EngineUnavailable,
    ArtifactNotFound,
    ArtifactMalformed,
    IOError,
    MemoryError,
    InternalError
};

class CliErrorMapper {
public:
    static CliError from_monitor_error(const nx::monitor::MonitorError& error);
    static int to_exit_code(CliError error);
};

} // namespace nx::cli::error