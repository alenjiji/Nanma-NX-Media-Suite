#include "metafix_argument_parser.h"
#include "metafix_command.h"
#include <cassert>
#include <iostream>
#include <vector>

using namespace nx::cli;

void test_repair_parsing() {
    std::vector<std::string> args = {
        "--input", "broken.mov",
        "--output", "fixed.mov",
        "--mode", "header-rebuild"
    };
    
    RepairRequest request;
    auto result = MetaFixArgumentParser::parse_repair(args, request);
    
    assert(result.success);
    assert(request.input_path == "broken.mov");
    assert(request.output_path == "fixed.mov");
    assert(request.mode == RepairMode::HeaderRebuild);
    assert(!request.allow_essence_modification);
    assert(!request.report_path.has_value());
    
    std::cout << "✓ Repair parsing test passed\n";
}

void test_repair_with_essence_modification() {
    std::vector<std::string> args = {
        "--input", "broken.mov",
        "--output", "fixed.mov", 
        "--mode", "interleave-correct",
        "--allow-essence-modification",
        "--report", "repair.log"
    };
    
    RepairRequest request;
    auto result = MetaFixArgumentParser::parse_repair(args, request);
    
    assert(result.success);
    assert(request.mode == RepairMode::InterleaveCorrect);
    assert(request.allow_essence_modification);
    assert(request.report_path.has_value());
    assert(*request.report_path == "repair.log");
    
    std::cout << "✓ Repair with essence modification test passed\n";
}

void test_validate_parsing() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--profile", "broadcast"
    };
    
    ValidateRequest request;
    auto result = MetaFixArgumentParser::parse_validate(args, request);
    
    assert(result.success);
    assert(request.input_path == "test.mov");
    assert(request.profile.has_value());
    assert(*request.profile == ValidationProfile::Broadcast);
    
    std::cout << "✓ Validate parsing test passed\n";
}

void test_validate_without_profile() {
    std::vector<std::string> args = {
        "--input", "test.mov"
    };
    
    ValidateRequest request;
    auto result = MetaFixArgumentParser::parse_validate(args, request);
    
    assert(result.success);
    assert(request.input_path == "test.mov");
    assert(!request.profile.has_value());  // Profile is optional
    
    std::cout << "✓ Validate without profile test passed\n";
}

void test_metadata_copy_parsing() {
    std::vector<std::string> args = {
        "--source", "source.mov",
        "--target", "target.mov",
        "--categories", "technical,rights,chapters"
    };
    
    MetadataCopyRequest request;
    auto result = MetaFixArgumentParser::parse_metadata_copy(args, request);
    
    assert(result.success);
    assert(request.source_path == "source.mov");
    assert(request.target_path == "target.mov");
    assert(request.categories.size() == 3);
    assert(request.categories[0] == MetadataCategory::Technical);
    assert(request.categories[1] == MetadataCategory::Rights);
    assert(request.categories[2] == MetadataCategory::Chapters);
    assert(!request.overwrite);
    
    std::cout << "✓ Metadata copy parsing test passed\n";
}

void test_metadata_merge_parsing() {
    std::vector<std::string> args = {
        "--inputs", "file1.mov,file2.mov,file3.mov",
        "--output", "merged.mov",
        "--precedence", "file1.mov,file2.mov,file3.mov"
    };
    
    MetadataMergeRequest request;
    auto result = MetaFixArgumentParser::parse_metadata_merge(args, request);
    
    assert(result.success);
    assert(request.input_paths.size() == 3);
    assert(request.input_paths[0] == "file1.mov");
    assert(request.output_path == "merged.mov");
    assert(request.precedence_order.size() == 3);
    assert(request.precedence_order[0] == "file1.mov");
    
    std::cout << "✓ Metadata merge parsing test passed\n";
}

void test_metadata_normalize_parsing() {
    std::vector<std::string> args = {
        "--input", "input.mov",
        "--output", "normalized.mov",
        "--schema", "ebu-r128"
    };
    
    MetadataNormalizeRequest request;
    auto result = MetaFixArgumentParser::parse_metadata_normalize(args, request);
    
    assert(result.success);
    assert(request.input_path == "input.mov");
    assert(request.output_path == "normalized.mov");
    assert(request.schema_id == "ebu-r128");
    
    std::cout << "✓ Metadata normalize parsing test passed\n";
}

void test_missing_required_flag() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--output", "fixed.mov"
        // Missing --mode
    };
    
    RepairRequest request;
    auto result = MetaFixArgumentParser::parse_repair(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Missing required flag test passed\n";
}

void test_invalid_repair_mode() {
    std::vector<std::string> args = {
        "--input", "test.mov",
        "--output", "fixed.mov",
        "--mode", "invalid-mode"
    };
    
    RepairRequest request;
    auto result = MetaFixArgumentParser::parse_repair(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Invalid repair mode test passed\n";
}

void test_invalid_metadata_category() {
    std::vector<std::string> args = {
        "--source", "source.mov",
        "--target", "target.mov",
        "--categories", "technical,invalid-category"
    };
    
    MetadataCopyRequest request;
    auto result = MetaFixArgumentParser::parse_metadata_copy(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_ENUM_ERROR);
    
    std::cout << "✓ Invalid metadata category test passed\n";
}

void test_precedence_duplicate() {
    std::vector<std::string> args = {
        "--inputs", "file1.mov,file2.mov",
        "--output", "merged.mov",
        "--precedence", "file1.mov,file1.mov"  // Duplicate in precedence
    };
    
    MetadataMergeRequest request;
    auto result = MetaFixArgumentParser::parse_metadata_merge(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Precedence duplicate test passed\n";
}
void test_precedence_mismatch() {
    std::vector<std::string> args = {
        "--inputs", "file1.mov,file2.mov",
        "--output", "merged.mov",
        "--precedence", "file1.mov"  // Mismatch: 2 inputs, 1 precedence
    };
    
    MetadataMergeRequest request;
    auto result = MetaFixArgumentParser::parse_metadata_merge(args, request);
    
    assert(!result.success);
    assert(result.error_code == CliErrorCode::NX_CLI_USAGE_ERROR);
    
    std::cout << "✓ Precedence mismatch test passed\n";
}

int main() {
    std::cout << "Running MetaFix CLI tests...\n";
    
    test_repair_parsing();
    test_repair_with_essence_modification();
    test_validate_parsing();
    test_validate_without_profile();
    test_metadata_copy_parsing();
    test_metadata_merge_parsing();
    test_metadata_normalize_parsing();
    test_missing_required_flag();
    test_invalid_repair_mode();
    test_invalid_metadata_category();
    test_precedence_mismatch();
    test_precedence_duplicate();
    
    std::cout << "All MetaFix CLI tests passed!\n";
    return 0;
}