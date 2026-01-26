#include "nx_cli.h"
#include "convert_command.h"
#include <iostream>
#include <vector>
#include <string>

namespace nx::cli {

int NxCli::main(int argc, char* argv[]) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    
    if (args.empty() || args[0] == "--help" || args[0] == "-h") {
        print_help();
        return 0;
    }
    
    auto result = route_command(args);
    return handle_result(result);
}

CliResult NxCli::route_command(const std::vector<std::string>& args) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing component");
    }
    
    const std::string& component = args[0];
    std::vector<std::string> component_args(args.begin() + 1, args.end());
    
    if (component == "convert") {
        return ConvertCommand::execute(component_args);
    } else if (component == "audio") {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "nx audio not yet implemented");
    } else if (component == "video") {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "nx video not yet implemented");
    } else if (component == "metafix") {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "nx metafix not yet implemented");
    } else if (component == "batch") {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "nx batch not yet implemented");
    } else if (component == "monitor") {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "nx monitor not yet implemented");
    } else {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown component: " + component);
    }
}

void NxCli::print_help() {
    std::cout << "nx - Nanma NX-MediaSuite CLI\n\n";
    std::cout << "Usage: nx <component> <operation> [flags]\n\n";
    std::cout << "Components:\n";
    std::cout << "  convert     Lossless transcoding operations\n";
    std::cout << "  audio       Audio processing operations\n";
    std::cout << "  video       Video processing operations\n";
    std::cout << "  metafix     Metadata repair operations\n";
    std::cout << "  batch       Batch processing operations\n";
    std::cout << "  monitor     Monitoring and telemetry\n\n";
    std::cout << "Use 'nx <component> --help' for component-specific help\n";
}

void NxCli::print_component_help(const std::string& component) {
    if (component == "convert") {
        std::cout << "nx convert - Lossless transcoding operations\n\n";
        std::cout << "Operations:\n";
        std::cout << "  transcode   Convert media with explicit policies\n";
        std::cout << "  analyze     Analyze media streams (read-only)\n";
        std::cout << "  verify      Verify transcoding results\n\n";
        std::cout << "Use 'nx convert <operation> --help' for operation-specific help\n";
    } else {
        std::cout << "Help for " << component << " not yet implemented\n";
    }
}

int NxCli::handle_result(const CliResult& result) {
    if (result.success) {
        return 0;
    }
    
    // Error output to stderr
    std::cerr << "Error: " << result.message << "\n";
    
    switch (result.error_code) {
        case CliErrorCode::NONE:
            return 0;
        case CliErrorCode::NX_CLI_USAGE_ERROR:
            return 1;
        case CliErrorCode::NX_CLI_ENUM_ERROR:
            return 2;
        case CliErrorCode::NX_ENGINE_REJECTED:
            return 3;
        case CliErrorCode::NX_EXEC_FAILED:
            return 4;
        default:
            return 99;
    }
}

} // namespace nx::cli