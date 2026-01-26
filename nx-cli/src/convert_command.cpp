#include "convert_command.h"
#include "argument_parser.h"
#include <iostream>

namespace nx::cli {

CliResult ConvertCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing operation for nx convert");
    }
    
    const std::string& operation = args[0];
    std::vector<std::string> operation_args(args.begin() + 1, args.end());
    
    if (operation == "transcode") {
        return handle_transcode(operation_args);
    } else if (operation == "analyze") {
        return handle_analyze(operation_args);
    } else if (operation == "verify") {
        return handle_verify(operation_args);
    } else {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown operation: " + operation);
    }
}

CliResult ConvertCommand::handle_transcode(const std::vector<std::string>& args) {
    TranscodeRequest request;
    auto parse_result = ArgumentParser::parse_transcode(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_transcode_engine(request);
}

CliResult ConvertCommand::handle_analyze(const std::vector<std::string>& args) {
    std::string input_path;
    bool json_output;
    auto parse_result = ArgumentParser::parse_analyze(args, input_path, json_output);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_analyze_engine(input_path, json_output);
}

CliResult ConvertCommand::handle_verify(const std::vector<std::string>& args) {
    std::string input_path, output_path;
    bool json_output;
    auto parse_result = ArgumentParser::parse_verify(args, input_path, output_path, json_output);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_verify_engine(input_path, output_path, json_output);
}

CliResult ConvertCommand::invoke_transcode_engine(const TranscodeRequest& request) {
    // TODO: This is where we would invoke nx::convert::TranscodeEngine::execute()
    // For now, implement dry-run behavior
    
    if (request.flags.dry_run) {
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"transcode\",\n";
            std::cout << "  \"input\": \"" << request.input_path << "\",\n";
            std::cout << "  \"output\": \"" << request.output_path << "\",\n";
            std::cout << "  \"container\": \"" << static_cast<int>(request.target_container) << "\",\n";
            std::cout << "  \"video_policy\": \"" << static_cast<int>(request.video_policy) << "\",\n";
            std::cout << "  \"audio_policy\": \"" << static_cast<int>(request.audio_policy) << "\"\n";
            std::cout << "}\n";
        } else {
            std::cout << "DRY RUN: Would execute transcode with:\n";
            std::cout << "  Input: " << request.input_path << "\n";
            std::cout << "  Output: " << request.output_path << "\n";
            std::cout << "  Container: " << static_cast<int>(request.target_container) << "\n";
            std::cout << "  Video: " << static_cast<int>(request.video_policy);
            if (!request.video_codec.empty()) {
                std::cout << " (" << request.video_codec << ")";
            }
            std::cout << "\n";
            std::cout << "  Audio: " << static_cast<int>(request.audio_policy);
            if (!request.audio_codec.empty()) {
                std::cout << " (" << request.audio_codec << ")";
            }
            std::cout << "\n";
        }
        return CliResult::ok();
    }
    
    // Placeholder for actual engine invocation
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "TranscodeEngine not yet implemented");
}

CliResult ConvertCommand::invoke_analyze_engine(const std::string& input_path, bool json_output) {
    // TODO: Invoke nx::convert::TranscodeEngine::analyze()
    
    if (json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"analyze\",\n";
        std::cout << "  \"input\": \"" << input_path << "\",\n";
        std::cout << "  \"status\": \"not_implemented\"\n";
        std::cout << "}\n";
    } else {
        std::cout << "ANALYZE: " << input_path << "\n";
        std::cout << "Status: Not yet implemented\n";
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "AnalyzeEngine not yet implemented");
}

CliResult ConvertCommand::invoke_verify_engine(const std::string& input_path, const std::string& output_path, bool json_output) {
    // TODO: Invoke nx::convert::TranscodeEngine::verify()
    
    if (json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"verify\",\n";
        std::cout << "  \"input\": \"" << input_path << "\",\n";
        std::cout << "  \"output\": \"" << output_path << "\",\n";
        std::cout << "  \"status\": \"not_implemented\"\n";
        std::cout << "}\n";
    } else {
        std::cout << "VERIFY: " << input_path << " vs " << output_path << "\n";
        std::cout << "Status: Not yet implemented\n";
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "VerifyEngine not yet implemented");
}

} // namespace nx::cli