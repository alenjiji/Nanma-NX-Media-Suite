#include "video_command.h"
#include "video_argument_parser.h"
#include "nx/video/VideoEngine.h"
#include <iostream>

namespace nx::cli {

static nx::video::VideoRequest translate_to_video_engine_request(const VideoAnalyzeRequest& /* cli_request */) {
    return nx::video::VideoRequest{};
}

static nx::video::VideoRequest translate_to_video_engine_request(const VideoProcessRequest& /* cli_request */) {
    return nx::video::VideoRequest{};
}

static nx::video::VideoRequest translate_to_video_engine_request(const VideoVerifyRequest& /* cli_request */) {
    return nx::video::VideoRequest{};
}

// Enum to string conversion for readable output
static std::string video_range_to_string(VideoRange range) {
    switch (range) {
        case VideoRange::Full: return "full";
        case VideoRange::Limited: return "limited";
        default: return "unknown";
    }
}

static std::string hdr_mode_to_string(HdrMode mode) {
    switch (mode) {
        case HdrMode::Passthrough: return "passthrough";
        case HdrMode::Strip: return "strip";
        default: return "unknown";
    }
}

CliResult VideoCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing operation for nx video");
    }
    
    const std::string& operation = args[0];
    std::vector<std::string> operation_args(args.begin() + 1, args.end());
    
    if (operation == "analyze") {
        return handle_analyze(operation_args);
    } else if (operation == "process") {
        return handle_process(operation_args);
    } else if (operation == "verify") {
        return handle_verify(operation_args);
    } else {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown operation: " + operation);
    }
}

CliResult VideoCommand::handle_analyze(const std::vector<std::string>& args) {
    VideoAnalyzeRequest request;
    auto parse_result = VideoArgumentParser::parse_analyze(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_analyze_engine(request);
}

CliResult VideoCommand::handle_process(const std::vector<std::string>& args) {
    VideoProcessRequest request;
    auto parse_result = VideoArgumentParser::parse_process(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_process_engine(request);
}

CliResult VideoCommand::handle_verify(const std::vector<std::string>& args) {
    VideoVerifyRequest request;
    auto parse_result = VideoArgumentParser::parse_verify(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_verify_engine(request);
}

CliResult VideoCommand::invoke_analyze_engine(const VideoAnalyzeRequest& request) {
    nx::video::VideoEngine engine;
    auto result = engine.prepare(translate_to_video_engine_request(request));
    
    if (result.is_success()) {
        auto outcome = result.get_value();
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"analyze\",\n";
            std::cout << "  \"input\": \"" << request.input_path << "\",\n";
            std::cout << "  \"status\": \"prepared\",\n";
            std::cout << "  \"graph_id\": \"" << outcome.graph_id << "\",\n";
            std::cout << "  \"verification_token\": \"" << outcome.verification_token << "\"\n";
            std::cout << "}\n";
        } else {
            std::cout << "ANALYZE: " << request.input_path << "\n";
            std::cout << "Status: Prepared successfully\n";
            std::cout << "Graph ID: " << outcome.graph_id << "\n";
            std::cout << "Verification: " << outcome.verification_token << "\n";
        }
        return CliResult::ok();
    } else {
        return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "VideoEngine rejected analyze request");
    }
}

CliResult VideoCommand::invoke_process_engine(const VideoProcessRequest& request) {
    if (request.flags.dry_run) {
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"process\",\n";
            std::cout << "  \"input\": \"" << request.input_path << "\",\n";
            std::cout << "  \"output\": \"" << request.output_path << "\",\n";
            std::cout << "  \"video_operations\": {\n";
            
            bool first = true;
            if (request.scale_dimensions.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"scale\": \"" << *request.scale_dimensions << "\"";
                first = false;
            }
            if (request.crop_parameters.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"crop\": \"" << *request.crop_parameters << "\"";
                first = false;
            }
            if (request.pixel_format.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"pixel_format\": \"" << *request.pixel_format << "\"";
                first = false;
            }
            if (request.color_space.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"color_space\": \"" << *request.color_space << "\"";
                first = false;
            }
            if (request.range.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"range\": \"" << video_range_to_string(*request.range) << "\"";
                first = false;
            }
            if (request.hdr_mode.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"hdr_mode\": \"" << hdr_mode_to_string(*request.hdr_mode) << "\"";
                first = false;
            }
            
            std::cout << "\n  }";
            if (request.report_path.has_value()) {
                std::cout << ",\n  \"report\": \"" << *request.report_path << "\"";
            }
            std::cout << "\n}\n";
        } else {
            std::cout << "DRY RUN: Would execute video process with:\n";
            std::cout << "  Input: " << request.input_path << "\n";
            std::cout << "  Output: " << request.output_path << "\n";
            std::cout << "  Video Operations:\n";
            
            if (request.scale_dimensions.has_value()) {
                std::cout << "    Scale: " << *request.scale_dimensions << "\n";
            }
            if (request.crop_parameters.has_value()) {
                std::cout << "    Crop: " << *request.crop_parameters << "\n";
            }
            if (request.pixel_format.has_value()) {
                std::cout << "    Pixel Format: " << *request.pixel_format << "\n";
            }
            if (request.color_space.has_value()) {
                std::cout << "    Color Space: " << *request.color_space << "\n";
            }
            if (request.range.has_value()) {
                std::cout << "    Range: " << video_range_to_string(*request.range) << "\n";
            }
            if (request.hdr_mode.has_value()) {
                std::cout << "    HDR Mode: " << hdr_mode_to_string(*request.hdr_mode) << "\n";
            }
            
            if (request.report_path.has_value()) {
                std::cout << "  Report: " << *request.report_path << "\n";
            }
        }
        return CliResult::ok();
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "VideoEngine process not yet implemented");
}

CliResult VideoCommand::invoke_verify_engine(const VideoVerifyRequest& request) {
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"verify\",\n";
        std::cout << "  \"input\": \"" << request.input_path << "\",\n";
        std::cout << "  \"output\": \"" << request.output_path << "\",\n";
        std::cout << "  \"status\": \"not_implemented\"\n";
        std::cout << "}\n";
    } else {
        std::cout << "VERIFY: " << request.input_path << " vs " << request.output_path << "\n";
        std::cout << "Status: Not yet implemented\n";
    }
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "VideoEngine verify not yet implemented");
}

} // namespace nx::cli