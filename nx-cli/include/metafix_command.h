#pragma once

#include "cli_types.h"
#include "metafix_types.h"
#include <vector>
#include <string>

namespace nx::cli {

class MetaFixCommand {
public:
    // Execute nx metafix <operation>
    static CliResult execute(const std::vector<std::string>& args);

private:
    // Operation handlers
    static CliResult handle_repair(const std::vector<std::string>& args);
    static CliResult handle_validate(const std::vector<std::string>& args);
    static CliResult handle_metadata_copy(const std::vector<std::string>& args);
    static CliResult handle_metadata_merge(const std::vector<std::string>& args);
    static CliResult handle_metadata_normalize(const std::vector<std::string>& args);
    
    // Engine invocation (adapter only)
    static CliResult invoke_repair_engine(const RepairRequest& request);
    static CliResult invoke_validate_engine(const ValidateRequest& request);
    static CliResult invoke_metadata_copy_engine(const MetadataCopyRequest& request);
    static CliResult invoke_metadata_merge_engine(const MetadataMergeRequest& request);
    static CliResult invoke_metadata_normalize_engine(const MetadataNormalizeRequest& request);
};

} // namespace nx::cli