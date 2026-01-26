#pragma once

#include "cli_types.h"
#include "metafix_types.h"
#include <vector>
#include <string>
#include <optional>

namespace nx::cli {

class MetaFixArgumentParser {
public:
    // Parse nx metafix repair arguments
    static CliResult parse_repair(const std::vector<std::string>& args, RepairRequest& request);
    
    // Parse nx metafix validate arguments
    static CliResult parse_validate(const std::vector<std::string>& args, ValidateRequest& request);
    
    // Parse nx metafix metadata-copy arguments
    static CliResult parse_metadata_copy(const std::vector<std::string>& args, MetadataCopyRequest& request);
    
    // Parse nx metafix metadata-merge arguments
    static CliResult parse_metadata_merge(const std::vector<std::string>& args, MetadataMergeRequest& request);
    
    // Parse nx metafix metadata-normalize arguments
    static CliResult parse_metadata_normalize(const std::vector<std::string>& args, MetadataNormalizeRequest& request);

private:
    // Enum parsers (static registry validation only)
    static std::optional<RepairMode> parse_repair_mode(const std::string& value);
    static std::optional<ValidationProfile> parse_validation_profile(const std::string& value);
    static std::optional<MetadataCategory> parse_metadata_category(const std::string& value);
    static CliResult parse_metadata_categories(const std::string& value, std::vector<MetadataCategory>& categories);
    static CliResult parse_input_list(const std::string& value, std::vector<std::string>& inputs);
    static CliResult parse_precedence_list(const std::string& value, std::vector<std::string>& precedence);
    
    // Utility functions
    static std::string get_flag_value(const std::vector<std::string>& args, const std::string& flag);
    static bool has_flag(const std::vector<std::string>& args, const std::string& flag);
    static CliResult validate_flags(const std::vector<std::string>& args, const std::vector<std::string>& allowed_flags);
};

} // namespace nx::cli