#include "CliError.h"

namespace nx::cli::error {

CliError CliErrorMapper::from_monitor_error(const nx::monitor::MonitorError& error) {
    return CliError::InternalError;
}

int CliErrorMapper::to_exit_code(CliError error) {
    switch (error) {
        case CliError::Success:
            return 0;
        case CliError::InvalidCommand:
            return 64;
        case CliError::EngineUnavailable:
            return 10;
        case CliError::ArtifactNotFound:
            return 11;
        case CliError::ArtifactMalformed:
            return 12;
        case CliError::IOError:
            return 20;
        case CliError::MemoryError:
            return 21;
        case CliError::InternalError:
            return 99;
    }
    return 99;
}

} // namespace nx::cli::error