// PHASE 15 INVARIANT:
// This adapter is a stateless CLI bridge.
// It adds no defaults, no retries, no inference.
// All parameters map 1:1 to CLI arguments.
// If a value is missing, this is a caller error.

#pragma once

#include <string>

namespace nx::ui::adapters {

struct MetaFixRepairDto {};
struct MetaFixValidateDto {};
struct MetaFixMetadataCopyDto {};
struct MetaFixMetadataMergeDto {};
struct MetaFixMetadataNormalizeDto {};

template <typename Dto>
struct CliTypedResult {
    int exit_code;
    std::string raw_json;
};

struct MetaFixRepairArgs {
    std::string input_path;
    std::string output_path;
    std::string repair_mode;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct MetaFixValidateArgs {
    std::string input_path;
    std::string validation_level;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct MetaFixMetadataCopyArgs {
    std::string input_path;
    std::string output_path;
    std::string metadata_source;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct MetaFixMetadataMergeArgs {
    std::string input_path;
    std::string output_path;
    std::string merge_strategy;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

struct MetaFixMetadataNormalizeArgs {
    std::string input_path;
    std::string output_path;
    std::string output_format; // output_format must be "json"; adapters do not support human output
};

class MetaFixAdapter {
public:
    static CliTypedResult<MetaFixRepairDto> repair(const MetaFixRepairArgs& args);
    static CliTypedResult<MetaFixValidateDto> validate(const MetaFixValidateArgs& args);
    static CliTypedResult<MetaFixMetadataCopyDto> metadata_copy(const MetaFixMetadataCopyArgs& args);
    static CliTypedResult<MetaFixMetadataMergeDto> metadata_merge(const MetaFixMetadataMergeArgs& args);
    static CliTypedResult<MetaFixMetadataNormalizeDto> metadata_normalize(const MetaFixMetadataNormalizeArgs& args);
};

} // namespace nx::ui::adapters