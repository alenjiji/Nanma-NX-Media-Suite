#include "video_argument_parser.h"
#include "video_command.h"
#include <cassert>
#include <iostream>
#include <vector>

using namespace nx::cli;

void test_analyze_parsing() {
    std::vector<std::string> args = {
        "--input", "video.mp4"
    };
    
    VideoAnalyzeRequest request;
    auto result = VideoArgumentParser::parse_analyze(args, request);
    
    assert(result.success);
    assert(request.input_path == "video.mp4");
    
    std::cout << "✓ Analyze parsing test passed\n";
}

void test_process_scale_only() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--scale", "1920x1080"
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(result.success);
    assert(request.input_path == "input.mp4");
    assert(request.output_path == "output.mp4");
    assert(request.scale_dimensions.has_value());
    assert(*request.scale_dimensions == "1920x1080");
    assert(!request.crop_parameters.has_value());
    assert(request.has_video_operations());
    
    std::cout << "✓ Process scale only test passed\n";
}

void test_process_multiple_operations() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--scale", "3840x2160",
        "--crop", "1920x800+0+140",
        "--pixel-format", "yuv420p",
        "--color-space", "bt709",
        "--range", "full",
        "--hdr", "strip"
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(result.success);
    assert(request.scale_dimensions.has_value());
    assert(*request.scale_dimensions == "3840x2160");
    assert(request.crop_parameters.has_value());
    assert(*request.crop_parameters == "1920x800+0+140");
    assert(request.pixel_format.has_value());
    assert(*request.pixel_format == "yuv420p");
    assert(request.color_space.has_value());
    assert(*request.color_space == "bt709");
    assert(request.range.has_value());
    assert(*request.range == VideoRange::Full);
    assert(request.hdr_mode.has_value());
    assert(*request.hdr_mode == HdrMode::Strip);
    assert(request.has_video_operations());
    
    std::cout << "✓ Process multiple operations test passed\n";
}

void test_process_no_video_operations() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4"
        // No video operations
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Process no video operations test passed\n";
}

void test_process_range_without_color_space() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--range", "full"
        // Missing --color-space
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Process range without color-space test passed\n";
}

void test_verify_parsing() {
    std::vector<std::string> args = {
        "--input", "original.mp4",
        "--output", "processed.mp4"
    };
    
    VideoVerifyRequest request;
    auto result = VideoArgumentParser::parse_verify(args, request);
    
    assert(result.success);
    assert(request.input_path == "original.mp4");
    assert(request.output_path == "processed.mp4");
    
    std::cout << "✓ Verify parsing test passed\n";
}

void test_missing_required_flag() {
    std::vector<std::string> args = {
        "--output", "output.mp4",
        "--scale", "1920x1080"
        // Missing --input
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Missing required flag test passed\n";
}

void test_invalid_scale_format() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--scale", "1920-1080"  // Invalid format
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Invalid scale format test passed\n";
}

void test_invalid_crop_format() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--crop", "1920x800-0-140"  // Invalid format
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Invalid crop format test passed\n";
}

void test_invalid_range() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--color-space", "bt709",
        "--range", "invalid-range"
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Invalid range test passed\n";
}

void test_invalid_hdr_mode() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--hdr", "invalid-hdr"
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Invalid HDR mode test passed\n";
}

void test_pixel_format_opaque() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--pixel-format", "custom_format_123"  // Treated as opaque
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(result.success);
    assert(request.pixel_format.has_value());
    assert(*request.pixel_format == "custom_format_123");
    
    std::cout << "✓ Pixel format opaque test passed\n";
}

void test_color_space_opaque() {
    std::vector<std::string> args = {
        "--input", "input.mp4",
        "--output", "output.mp4",
        "--color-space", "custom_space_xyz"  // Treated as opaque
    };
    
    VideoProcessRequest request;
    auto result = VideoArgumentParser::parse_process(args, request);
    
    assert(result.success);
    assert(request.color_space.has_value());
    assert(*request.color_space == "custom_space_xyz");
    
    std::cout << "✓ Color space opaque test passed\n";
}

int main() {
    std::cout << "Running Video CLI tests...\n";
    
    test_analyze_parsing();
    test_process_scale_only();
    test_process_multiple_operations();
    test_process_no_video_operations();
    test_process_range_without_color_space();
    test_verify_parsing();
    test_missing_required_flag();
    test_invalid_scale_format();
    test_invalid_crop_format();
    test_invalid_range();
    test_invalid_hdr_mode();
    test_pixel_format_opaque();
    test_color_space_opaque();
    
    std::cout << "All Video CLI tests passed!\n";
    return 0;
}