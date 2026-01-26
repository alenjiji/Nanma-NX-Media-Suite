#include "argument_parser.h"
#include "convert_command.h"
#include <cassert>
#include <iostream>
#include <vector>

using namespace nx::cli;

void test_transcode_parsing() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--output", "test.mp4", 
        "--container", "mp4",
        "--video", "passthrough",
        "--audio", "encode:aac"
    };
    
    TranscodeRequest request;
    auto result = ArgumentParser::parse_transcode(args, request);
    
    assert(result.success);
    assert(request.input_path == "test.mov");
    assert(request.output_path == "test.mp4");
    assert(request.target_container == ContainerType::MP4);
    assert(request.video_policy == VideoPolicy::Passthrough);
    assert(request.audio_policy == AudioPolicy::Encode);
    assert(request.audio_codec == "aac");
    assert(!request.subtitle_policy.has_value());  // Not specified
    assert(!request.metadata_policy.has_value());  // Not specified
    
    std::cout << "✓ Transcode parsing test passed\n";
}

void test_missing_required_flag() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--container", "mp4",
        "--video", "passthrough",
        "--audio", "encode:aac"
        // Missing --output
    };
    
    TranscodeRequest request;
    auto result = ArgumentParser::parse_transcode(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Missing required flag test passed\n";
}

void test_invalid_enum() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--output", "test.mp4",
        "--container", "invalid_container",
        "--video", "passthrough", 
        "--audio", "encode:aac"
    };
    
    TranscodeRequest request;
    auto result = ArgumentParser::parse_transcode(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Invalid enum test passed\n";
}

void test_unknown_flag() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--output", "test.mp4",
        "--container", "mp4",
        "--video", "passthrough",
        "--audio", "encode:aac",
        "--unknown-flag", "value"
    };
    
    TranscodeRequest request;
    auto result = ArgumentParser::parse_transcode(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Unknown flag test passed\n";
}

void test_duplicate_flag() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--output", "test.mp4",
        "--container", "mp4",
        "--video", "passthrough",
        "--video", "encode:h264",  // Duplicate
        "--audio", "encode:aac"
    };
    
    TranscodeRequest request;
    auto result = ArgumentParser::parse_transcode(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Duplicate flag test passed\n";
}
void test_encode_without_codec() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--output", "test.mp4",
        "--container", "mp4",
        "--video", "encode:",  // Missing codec
        "--audio", "passthrough"
    };
    
    TranscodeRequest request;
    auto result = ArgumentParser::parse_transcode(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Encode without codec test passed\n";
}

int main() {
    std::cout << "Running CLI tests...\n";
    
    test_transcode_parsing();
    test_missing_required_flag();
    test_invalid_enum();
    test_unknown_flag();
    test_duplicate_flag();
    test_encode_without_codec();
    
    std::cout << "All CLI tests passed!\n";
    return 0;
}