#include "audio_argument_parser.h"
#include "audio_command.h"
#include <cassert>
#include <iostream>
#include <vector>

using namespace nx::cli;

void test_measure_parsing() {
    std::vector<std::string> args = {
        "--input", "audio.wav",
        "--standard", "ebu-r128"
    };
    
    AudioMeasureRequest request;
    auto result = AudioArgumentParser::parse_measure(args, request);
    
    assert(result.success);
    assert(request.input_path == "audio.wav");
    assert(request.standard == LoudnessStandard::EbuR128);
    assert(!request.report_path.has_value());
    
    std::cout << "✓ Measure parsing test passed\n";
}

void test_measure_with_report() {
    std::vector<std::string> args = {
        "--input", "audio.wav",
        "--standard", "itu-bs1770",
        "--report", "loudness.json"
    };
    
    AudioMeasureRequest request;
    auto result = AudioArgumentParser::parse_measure(args, request);
    
    assert(result.success);
    assert(request.standard == LoudnessStandard::ItuBs1770);
    assert(request.report_path.has_value());
    assert(*request.report_path == "loudness.json");
    
    std::cout << "✓ Measure with report test passed\n";
}

void test_process_src_only() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav",
        "--src", "48000"
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(result.success);
    assert(request.input_path == "input.wav");
    assert(request.output_path == "output.wav");
    assert(request.src_rate.has_value());
    assert(*request.src_rate == 48000);
    assert(!request.gain_db.has_value());
    assert(request.has_dsp_operations());
    
    std::cout << "✓ Process SRC only test passed\n";
}

void test_process_multiple_dsp() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav",
        "--gain", "-3.5",
        "--loudness-normalize", "-23LUFS",
        "--bit-depth", "16",
        "--dither", "triangular"
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(result.success);
    assert(request.gain_db.has_value());
    assert(*request.gain_db == -3.5);
    assert(request.loudness_target.has_value());
    assert(*request.loudness_target == "-23LUFS");
    assert(request.bit_depth.has_value());
    assert(*request.bit_depth == 16);
    assert(request.dither_type.has_value());
    assert(*request.dither_type == DitherType::Triangular);
    assert(request.has_dsp_operations());
    
    std::cout << "✓ Process multiple DSP test passed\n";
}

void test_process_no_dsp_operations() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav"
        // No DSP operations
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Process no DSP operations test passed\n";
}

void test_process_dither_without_bit_depth() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav",
        "--dither", "triangular"
        // Missing --bit-depth
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Process dither without bit-depth test passed\n";
}

void test_verify_parsing() {
    std::vector<std::string> args = {
        "--input", "original.wav",
        "--output", "processed.wav"
    };
    
    AudioVerifyRequest request;
    auto result = AudioArgumentParser::parse_verify(args, request);
    
    assert(result.success);
    assert(request.input_path == "original.wav");
    assert(request.output_path == "processed.wav");
    
    std::cout << "✓ Verify parsing test passed\n";
}

void test_missing_required_flag() {
    std::vector<std::string> args = {
        "--input", "audio.wav"
        // Missing --standard
    };
    
    AudioMeasureRequest request;
    auto result = AudioArgumentParser::parse_measure(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Missing required flag test passed\n";
}

void test_invalid_loudness_standard() {
    std::vector<std::string> args = {
        "--input", "audio.wav",
        "--standard", "invalid-standard"
    };
    
    AudioMeasureRequest request;
    auto result = AudioArgumentParser::parse_measure(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Invalid loudness standard test passed\n";
}

void test_invalid_dither_type() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav",
        "--bit-depth", "16",
        "--dither", "invalid-dither"
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Invalid dither type test passed\n";
}

void test_loudness_target_without_lufs() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav",
        "--loudness-normalize", "-23"  // Missing LUFS unit
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Loudness target without LUFS test passed\n";
}

void test_invalid_sample_rate() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav",
        "--src", "invalid"
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Invalid sample rate test passed\n";
}

void test_invalid_gain() {
    std::vector<std::string> args = {
        "--input", "input.wav",
        "--output", "output.wav",
        "--gain", "not-a-number"
    };
    
    AudioProcessRequest request;
    auto result = AudioArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Invalid gain test passed\n";
}

int main() {
    std::cout << "Running Audio CLI tests...\n";
    
    test_measure_parsing();
    test_measure_with_report();
    test_process_src_only();
    test_process_multiple_dsp();
    test_process_no_dsp_operations();
    test_process_dither_without_bit_depth();
    test_verify_parsing();
    test_missing_required_flag();
    test_invalid_loudness_standard();
    test_invalid_dither_type();
    test_loudness_target_without_lufs();
    test_invalid_sample_rate();
    test_invalid_gain();
    
    std::cout << "All Audio CLI tests passed!\n";
    return 0;
}