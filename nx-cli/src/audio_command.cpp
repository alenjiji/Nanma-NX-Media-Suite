#include "audio_command.h"
#include "audio_argument_parser.h"
#include "nx/audio/AudioEngine.h"
#include <iostream>
#include <functional>

namespace nx::cli {

// Translation functions: CLI types -> Engine types
static nx::audio::AudioRequest translate_to_audio_engine_request(const AudioMeasureRequest& cli_request) {
    nx::audio::AudioRequest engine_request;
    
    // Use hash of input path as audio ID for deterministic mapping
    std::hash<std::string> hasher;
    engine_request.input_audio_id = hasher(cli_request.input_path);
    
    // Use hash of standard as target format ID
    std::string target_spec = std::to_string(static_cast<int>(cli_request.standard));
    engine_request.target_format_id = hasher(target_spec);
    
    // Generate deterministic request ID
    std::string request_spec = cli_request.input_path + target_spec;
    engine_request.request_id = hasher(request_spec);
    
    // Set logical clock to 1 (deterministic)
    engine_request.clock = {1};
    
    return engine_request;
}

static nx::audio::AudioRequest translate_to_audio_engine_request(const AudioProcessRequest& cli_request) {
    nx::audio::AudioRequest engine_request;
    
    // Use hash of input path as audio ID for deterministic mapping
    std::hash<std::string> hasher;
    engine_request.input_audio_id = hasher(cli_request.input_path);
    
    // Use hash of all DSP parameters as target format ID
    std::string target_spec = cli_request.output_path;
    if (cli_request.src_rate.has_value()) target_spec += std::to_string(*cli_request.src_rate);
    if (cli_request.gain_db.has_value()) target_spec += std::to_string(*cli_request.gain_db);
    if (cli_request.loudness_target.has_value()) target_spec += *cli_request.loudness_target;
    if (cli_request.bit_depth.has_value()) target_spec += std::to_string(*cli_request.bit_depth);
    if (cli_request.dither_type.has_value()) target_spec += std::to_string(static_cast<int>(*cli_request.dither_type));
    
    engine_request.target_format_id = hasher(target_spec);
    
    // Generate deterministic request ID
    std::string request_spec = cli_request.input_path + target_spec;
    engine_request.request_id = hasher(request_spec);
    
    // Set logical clock to 1 (deterministic)
    engine_request.clock = {1};
    
    return engine_request;
}

static nx::audio::AudioRequest translate_to_audio_engine_request(const AudioVerifyRequest& cli_request) {
    nx::audio::AudioRequest engine_request;
    
    // Use hash of input path as audio ID for deterministic mapping
    std::hash<std::string> hasher;
    engine_request.input_audio_id = hasher(cli_request.input_path);
    
    // Use hash of output path as target format ID
    engine_request.target_format_id = hasher(cli_request.output_path);
    
    // Generate deterministic request ID
    std::string request_spec = cli_request.input_path + cli_request.output_path;
    engine_request.request_id = hasher(request_spec);
    
    // Set logical clock to 1 (deterministic)
    engine_request.clock = {1};
    
    return engine_request;
}

// Enum to string conversion for readable output
static std::string loudness_standard_to_string(LoudnessStandard standard) {
    switch (standard) {
        case LoudnessStandard::EbuR128: return "ebu-r128";
        case LoudnessStandard::ItuBs1770: return "itu-bs1770";
        case LoudnessStandard::AtscA85: return "atsc-a85";
        default: return "unknown";
    }
}

static std::string dither_type_to_string(DitherType type) {
    switch (type) {
        case DitherType::Triangular: return "triangular";
        case DitherType::Rectangular: return "rectangular";
        case DitherType::NoiseShaped: return "noise-shaped";
        default: return "unknown";
    }
}

CliResult AudioCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing operation for nx audio");
    }
    
    const std::string& operation = args[0];
    std::vector<std::string> operation_args(args.begin() + 1, args.end());
    
    if (operation == "measure") {
        return handle_measure(operation_args);
    } else if (operation == "process") {
        return handle_process(operation_args);
    } else if (operation == "verify") {
        return handle_verify(operation_args);
    } else {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown operation: " + operation);
    }
}

CliResult AudioCommand::handle_measure(const std::vector<std::string>& args) {
    AudioMeasureRequest request;
    auto parse_result = AudioArgumentParser::parse_measure(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_measure_engine(request);
}

CliResult AudioCommand::handle_process(const std::vector<std::string>& args) {
    AudioProcessRequest request;
    auto parse_result = AudioArgumentParser::parse_process(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_process_engine(request);
}

CliResult AudioCommand::handle_verify(const std::vector<std::string>& args) {
    AudioVerifyRequest request;
    auto parse_result = AudioArgumentParser::parse_verify(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_verify_engine(request);
}

CliResult AudioCommand::invoke_measure_engine(const AudioMeasureRequest& request) {
    // Invoke actual AudioEngine
    nx::audio::AudioEngine engine;
    nx::audio::AudioRequest engine_request = translate_to_audio_engine_request(request);
    
    auto result = engine.prepare(engine_request);
    
    if (result.is_success()) {
        // Engine succeeded - preserve existing CLI output format
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"measure\",\n";
            std::cout << "  \"input\": \"" << request.input_path << "\",\n";
            std::cout << "  \"standard\": \"" << loudness_standard_to_string(request.standard) << "\",\n";
            std::cout << "  \"status\": \"prepared\",\n";
            std::cout << "  \"graph_id\": \"" << result.outcome.graph_id << "\",\n";
            std::cout << "  \"verification_token\": \"" << result.outcome.verification_token << "\"\n";
            std::cout << "}\n";
        } else {
            std::cout << "MEASURE: " << request.input_path << "\n";
            std::cout << "Standard: " << loudness_standard_to_string(request.standard) << "\n";
            if (request.report_path.has_value()) {
                std::cout << "Report: " << *request.report_path << "\n";
            }
            std::cout << "Status: Prepared successfully\n";
            std::cout << "Graph ID: " << result.outcome.graph_id << "\n";
            std::cout << "Verification: " << result.outcome.verification_token << "\n";
        }
        return CliResult::ok();
    } else {
        // Engine failed - return existing CLI error format
        return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "AudioEngine rejected measure request");
    }
}

CliResult AudioCommand::invoke_process_engine(const AudioProcessRequest& request) {
    // Handle dry-run mode with existing CLI output format
    if (request.flags.dry_run) {
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"process\",\n";
            std::cout << "  \"input\": \"" << request.input_path << "\",\n";
            std::cout << "  \"output\": \"" << request.output_path << "\",\n";
            std::cout << "  \"dsp_operations\": {\n";
            
            bool first = true;
            if (request.src_rate.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"src_rate\": " << *request.src_rate;
                first = false;
            }
            if (request.gain_db.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"gain_db\": " << *request.gain_db;
                first = false;
            }
            if (request.loudness_target.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"loudness_target\": \"" << *request.loudness_target << "\"";
                first = false;
            }
            if (request.bit_depth.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"bit_depth\": " << *request.bit_depth;
                first = false;
            }
            if (request.dither_type.has_value()) {
                if (!first) std::cout << ",\n";
                std::cout << "    \"dither_type\": \"" << dither_type_to_string(*request.dither_type) << "\"";
                first = false;
            }
            
            std::cout << "\n  }\n";
            std::cout << "}\n";
        } else {
            std::cout << "DRY RUN: Would execute audio process with:\n";
            std::cout << "  Input: " << request.input_path << "\n";
            std::cout << "  Output: " << request.output_path << "\n";
            std::cout << "  DSP Operations:\n";
            
            if (request.src_rate.has_value()) {
                std::cout << "    Sample Rate Conversion: " << *request.src_rate << " Hz\n";
            }
            if (request.gain_db.has_value()) {
                std::cout << "    Gain Adjustment: " << *request.gain_db << " dB\n";
            }
            if (request.loudness_target.has_value()) {
                std::cout << "    Loudness Normalization: " << *request.loudness_target << "\n";
            }
            if (request.bit_depth.has_value()) {
                std::cout << "    Bit Depth Conversion: " << *request.bit_depth << " bits\n";
            }
            if (request.dither_type.has_value()) {
                std::cout << "    Dithering: " << dither_type_to_string(*request.dither_type) << "\n";
            }
            
            if (request.report_path.has_value()) {
                std::cout << "  Report: " << *request.report_path << "\n";
            }
        }
        return CliResult::ok();
    }
    
    // Invoke actual AudioEngine
    nx::audio::AudioEngine engine;
    nx::audio::AudioRequest engine_request = translate_to_audio_engine_request(request);
    
    auto result = engine.prepare(engine_request);
    
    if (result.is_success()) {
        // Engine succeeded - preserve existing CLI success output format
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"process\",\n";
            std::cout << "  \"status\": \"prepared\",\n";
            std::cout << "  \"graph_id\": \"" << result.outcome.graph_id << "\",\n";
            std::cout << "  \"verification_token\": \"" << result.outcome.verification_token << "\"\n";
            std::cout << "}\n";
        } else {
            std::cout << "Audio processing prepared successfully\n";
            std::cout << "Graph ID: " << result.outcome.graph_id << "\n";
            std::cout << "Verification: " << result.outcome.verification_token << "\n";
        }
        return CliResult::ok();
    } else {
        // Engine failed - return existing CLI error format
        return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "AudioEngine rejected process request");
    }
}

CliResult AudioCommand::invoke_verify_engine(const AudioVerifyRequest& request) {
    // Invoke actual AudioEngine
    nx::audio::AudioEngine engine;
    nx::audio::AudioRequest engine_request = translate_to_audio_engine_request(request);
    
    auto result = engine.prepare(engine_request);
    
    if (result.is_success()) {
        // Engine succeeded - preserve existing CLI output format
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"verify\",\n";
            std::cout << "  \"input\": \"" << request.input_path << "\",\n";
            std::cout << "  \"output\": \"" << request.output_path << "\",\n";
            std::cout << "  \"status\": \"prepared\",\n";
            std::cout << "  \"graph_id\": \"" << result.outcome.graph_id << "\",\n";
            std::cout << "  \"verification_token\": \"" << result.outcome.verification_token << "\"\n";
            std::cout << "}\n";
        } else {
            std::cout << "VERIFY: " << request.input_path << " vs " << request.output_path << "\n";
            std::cout << "Status: Prepared successfully\n";
            std::cout << "Graph ID: " << result.outcome.graph_id << "\n";
            std::cout << "Verification: " << result.outcome.verification_token << "\n";
        }
        return CliResult::ok();
    } else {
        // Engine failed - return existing CLI error format
        return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "AudioEngine rejected verify request");
    }
}

} // namespace nx::cli