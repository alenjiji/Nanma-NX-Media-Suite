#include "nx_cli.h"
#include "convert_command.h"
#include "metafix_command.h"
#include "audio_command.h"
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
    } else if (component == "metafix") {
        return MetaFixCommand::execute(component_args);
    } else if (component == "audio") {
        return AudioCommand::execute(component_args);
    } else if (component == "video") {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "nx video not yet implemented");
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
    } else if (component == "metafix") {
        std::cout << "nx metafix - Metadata and container repair operations\n\n";
        std::cout << "Operations:\n";
        std::cout << "  repair              Structural container repair\n";
        std::cout << "  validate            Compliance and integrity validation\n";
        std::cout << "  metadata-copy       Copy metadata categories between containers\n";
        std::cout << "  metadata-merge      Merge metadata from multiple sources\n";
        std::cout << "  metadata-normalize  Schema-based metadata normalization\n\n";
        std::cout << "IMPORTANT: --allow-essence-modification breaks read-only guarantee\n";
        std::cout << "Use 'nx metafix <operation> --help' for operation-specific help\n";
    } else if (component == "audio") {
        std::cout << "nx audio - Sample-accurate audio processing operations\n\n";
        std::cout << "Operations:\n";
        std::cout << "  measure    Read-only loudness and peak analysis\n";
        std::cout << "  process    Apply explicit DSP graph to audio\n";
        std::cout << "  verify     Verify deterministic correctness\n\n";
        std::cout << "IMPORTANT: measure is read-only, process requires explicit DSP intent\n";
        std::cout << "Use 'nx audio <operation> --help' for operation-specific help\n";
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