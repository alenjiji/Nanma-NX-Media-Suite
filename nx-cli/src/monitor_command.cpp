#include "monitor_command.h"
#include "monitor_argument_parser.h"
#include <iostream>

namespace nx::cli {

CliResult MonitorCommand::execute(const std::vector<std::string>& args) {
    if (args.empty() || args[0] == "--help" || args[0] == "-h") {
        std::cout << "nx monitor - Read-only system observation\n\n";
        std::cout << "Operations:\n";
        std::cout << "  status      Global system status snapshot\n";
        std::cout << "  jobs        List known jobs (summary only)\n";
        std::cout << "  job         Single job snapshot\n";
        std::cout << "  engines     List registered engines\n";
        std::cout << "  version     Static version and build metadata\n\n";
        std::cout << "IMPORTANT: Read-only observation only, no control operations\n";
        std::cout << "Use 'nx monitor <operation> --help' for operation-specific help\n";
        return CliResult::ok();
    }
    
    const std::string& operation = args[0];
    std::vector<std::string> operation_args(args.begin() + 1, args.end());
    
    if (operation == "status") {
        MonitorStatusRequest request;
        auto parse_result = MonitorArgumentParser::parse_status_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_status(request);
        
    } else if (operation == "jobs") {
        MonitorJobsRequest request;
        auto parse_result = MonitorArgumentParser::parse_jobs_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_jobs(request);
        
    } else if (operation == "job") {
        MonitorJobRequest request;
        auto parse_result = MonitorArgumentParser::parse_job_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_job(request);
        
    } else if (operation == "engines") {
        MonitorEnginesRequest request;
        auto parse_result = MonitorArgumentParser::parse_engines_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_engines(request);
        
    } else if (operation == "version") {
        MonitorVersionRequest request;
        auto parse_result = MonitorArgumentParser::parse_version_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_version(request);
        
    } else {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Unknown monitor operation: " + operation + ". Available: status, jobs, job, engines, version"
        );
    }
}

CliResult MonitorCommand::handle_status(const MonitorStatusRequest& request) {
    std::cout << "nx monitor: request accepted\n";
    
    return CliResult::error(
        CliErrorCode::NX_ENGINE_REJECTED,
        "MonitorEngine status not yet implemented"
    );
}

CliResult MonitorCommand::handle_jobs(const MonitorJobsRequest& request) {
    std::cout << "nx monitor: request accepted\n";
    
    return CliResult::error(
        CliErrorCode::NX_ENGINE_REJECTED,
        "MonitorEngine jobs not yet implemented"
    );
}

CliResult MonitorCommand::handle_job(const MonitorJobRequest& request) {
    std::cout << "nx monitor: request accepted\n";
    
    return CliResult::error(
        CliErrorCode::NX_ENGINE_REJECTED,
        "MonitorEngine job not yet implemented"
    );
}

CliResult MonitorCommand::handle_engines(const MonitorEnginesRequest& request) {
    std::cout << "nx monitor: request accepted\n";
    
    return CliResult::error(
        CliErrorCode::NX_ENGINE_REJECTED,
        "MonitorEngine engines not yet implemented"
    );
}

CliResult MonitorCommand::handle_version(const MonitorVersionRequest& request) {
    std::cout << "nx monitor: request accepted\n";
    
    return CliResult::error(
        CliErrorCode::NX_ENGINE_REJECTED,
        "MonitorEngine version not yet implemented"
    );
}

} // namespace nx::cli