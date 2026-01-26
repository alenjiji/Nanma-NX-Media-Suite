#include "metafix_command.h"
#include "metafix_argument_parser.h"
#include <iostream>

namespace nx::cli {

// Enum to string conversion for readable output
static std::string repair_mode_to_string(RepairMode mode) {
    switch (mode) {
        case RepairMode::HeaderRebuild: return "header-rebuild";
        case RepairMode::IndexRegenerate: return "index-regenerate";
        case RepairMode::AtomRepair: return "atom-repair";
        case RepairMode::InterleaveCorrect: return "interleave-correct";
        default: return "unknown";
    }
}

static std::string validation_profile_to_string(ValidationProfile profile) {
    switch (profile) {
        case ValidationProfile::Container: return "container";
        case ValidationProfile::Metadata: return "metadata";
        case ValidationProfile::Broadcast: return "broadcast";
        default: return "unknown";
    }
}

static std::string metadata_category_to_string(MetadataCategory category) {
    switch (category) {
        case MetadataCategory::Technical: return "technical";
        case MetadataCategory::Descriptive: return "descriptive";
        case MetadataCategory::Rights: return "rights";
        case MetadataCategory::Chapters: return "chapters";
        case MetadataCategory::Artwork: return "artwork";
        default: return "unknown";
    }
}

CliResult MetaFixCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Missing operation for nx metafix");
    }
    
    const std::string& operation = args[0];
    std::vector<std::string> operation_args(args.begin() + 1, args.end());
    
    if (operation == "repair") {
        return handle_repair(operation_args);
    } else if (operation == "validate") {
        return handle_validate(operation_args);
    } else if (operation == "metadata-copy") {
        return handle_metadata_copy(operation_args);
    } else if (operation == "metadata-merge") {
        return handle_metadata_merge(operation_args);
    } else if (operation == "metadata-normalize") {
        return handle_metadata_normalize(operation_args);
    } else {
        return CliResult::error(CliErrorCode::NX_CLI_USAGE_ERROR, "Unknown operation: " + operation);
    }
}

CliResult MetaFixCommand::handle_repair(const std::vector<std::string>& args) {
    RepairRequest request;
    auto parse_result = MetaFixArgumentParser::parse_repair(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_repair_engine(request);
}

CliResult MetaFixCommand::handle_validate(const std::vector<std::string>& args) {
    ValidateRequest request;
    auto parse_result = MetaFixArgumentParser::parse_validate(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_validate_engine(request);
}

CliResult MetaFixCommand::handle_metadata_copy(const std::vector<std::string>& args) {
    MetadataCopyRequest request;
    auto parse_result = MetaFixArgumentParser::parse_metadata_copy(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_metadata_copy_engine(request);
}

CliResult MetaFixCommand::handle_metadata_merge(const std::vector<std::string>& args) {
    MetadataMergeRequest request;
    auto parse_result = MetaFixArgumentParser::parse_metadata_merge(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_metadata_merge_engine(request);
}

CliResult MetaFixCommand::handle_metadata_normalize(const std::vector<std::string>& args) {
    MetadataNormalizeRequest request;
    auto parse_result = MetaFixArgumentParser::parse_metadata_normalize(args, request);
    if (!parse_result.success) {
        return parse_result;
    }
    
    return invoke_metadata_normalize_engine(request);
}

CliResult MetaFixCommand::invoke_repair_engine(const RepairRequest& request) {
    // TODO: Invoke nx::meta::MetaEngine::repair()
    
    if (request.flags.dry_run) {
        if (request.flags.json_output) {
            std::cout << "{\n";
            std::cout << "  \"operation\": \"repair\",\n";
            std::cout << "  \"input\": \"" << request.input_path << "\",\n";
            std::cout << "  \"output\": \"" << request.output_path << "\",\n";
            std::cout << "  \"mode\": \"" << repair_mode_to_string(request.mode) << "\",\n";
            std::cout << "  \"allow_essence_modification\": " << (request.allow_essence_modification ? "true" : "false") << "\n";
            std::cout << "}\n";
        } else {
            std::cout << "DRY RUN: Would execute repair with:\n";
            std::cout << "  Input: " << request.input_path << "\n";
            std::cout << "  Output: " << request.output_path << "\n";
            std::cout << "  Mode: " << repair_mode_to_string(request.mode) << "\n";
            std::cout << "  Allow essence modification: " << (request.allow_essence_modification ? "yes" : "no") << "\n";
            if (request.report_path.has_value()) {
                std::cout << "  Report: " << *request.report_path << "\n";
            }
        }
        return CliResult::ok();
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "MetaEngine repair not yet implemented");
}

CliResult MetaFixCommand::invoke_validate_engine(const ValidateRequest& request) {
    // TODO: Invoke nx::meta::MetaEngine::validate()
    
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"validate\",\n";
        std::cout << "  \"input\": \"" << request.input_path << "\",\n";
        if (request.profile.has_value()) {
            std::cout << "  \"profile\": \"" << validation_profile_to_string(*request.profile) << "\",\n";
        }
        std::cout << "  \"status\": \"not_implemented\"\n";
        std::cout << "}\n";
    } else {
        std::cout << "VALIDATE: " << request.input_path << "\n";
        if (request.profile.has_value()) {
            std::cout << "Profile: " << validation_profile_to_string(*request.profile) << "\n";
        }
        std::cout << "Status: Not yet implemented\n";
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "MetaEngine validate not yet implemented");
}

CliResult MetaFixCommand::invoke_metadata_copy_engine(const MetadataCopyRequest& request) {
    // TODO: Invoke nx::meta::MetaEngine::copyMetadata()
    
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"metadata-copy\",\n";
        std::cout << "  \"source\": \"" << request.source_path << "\",\n";
        std::cout << "  \"target\": \"" << request.target_path << "\",\n";
        std::cout << "  \"categories\": [";
        for (size_t i = 0; i < request.categories.size(); ++i) {
            if (i > 0) std::cout << ",";
            std::cout << "\"" << metadata_category_to_string(request.categories[i]) << "\"";
        }
        std::cout << "],\n";
        std::cout << "  \"overwrite\": " << (request.overwrite ? "true" : "false") << ",\n";
        std::cout << "  \"status\": \"not_implemented\"\n";
        std::cout << "}\n";
    } else {
        std::cout << "METADATA-COPY: " << request.source_path << " -> " << request.target_path << "\n";
        std::cout << "Categories: " << request.categories.size() << " specified\n";
        std::cout << "Overwrite: " << (request.overwrite ? "yes" : "no") << "\n";
        std::cout << "Status: Not yet implemented\n";
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "MetaEngine copyMetadata not yet implemented");
}

CliResult MetaFixCommand::invoke_metadata_merge_engine(const MetadataMergeRequest& request) {
    // TODO: Invoke nx::meta::MetaEngine::mergeMetadata()
    
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"metadata-merge\",\n";
        std::cout << "  \"inputs\": [";
        for (size_t i = 0; i < request.input_paths.size(); ++i) {
            if (i > 0) std::cout << ",";
            std::cout << "\"" << request.input_paths[i] << "\"";
        }
        std::cout << "],\n";
        std::cout << "  \"output\": \"" << request.output_path << "\",\n";
        std::cout << "  \"status\": \"not_implemented\"\n";
        std::cout << "}\n";
    } else {
        std::cout << "METADATA-MERGE: " << request.input_paths.size() << " inputs -> " << request.output_path << "\n";
        std::cout << "Status: Not yet implemented\n";
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "MetaEngine mergeMetadata not yet implemented");
}

CliResult MetaFixCommand::invoke_metadata_normalize_engine(const MetadataNormalizeRequest& request) {
    // TODO: Invoke nx::meta::MetaEngine::normalizeMetadata()
    
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"metadata-normalize\",\n";
        std::cout << "  \"input\": \"" << request.input_path << "\",\n";
        std::cout << "  \"output\": \"" << request.output_path << "\",\n";
        std::cout << "  \"schema\": \"" << request.schema_id << "\",\n";
        std::cout << "  \"status\": \"not_implemented\"\n";
        std::cout << "}\n";
    } else {
        std::cout << "METADATA-NORMALIZE: " << request.input_path << " -> " << request.output_path << "\n";
        std::cout << "Schema: " << request.schema_id << "\n";
        std::cout << "Status: Not yet implemented\n";
    }
    
    return CliResult::error(CliErrorCode::NX_ENGINE_REJECTED, "MetaEngine normalizeMetadata not yet implemented");
}

} // namespace nx::cli