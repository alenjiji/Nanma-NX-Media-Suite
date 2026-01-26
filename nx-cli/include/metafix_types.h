#pragma once

#include <string>
#include <vector>
#include <optional>

namespace nx::cli {

// Repair Modes (Explicit)
enum class RepairMode {
    HeaderRebuild,
    IndexRegenerate,
    AtomRepair,
    InterleaveCorrect
};

// Validation Profiles (Explicit)
enum class ValidationProfile {
    Container,
    Metadata,
    Broadcast
};

// Metadata Categories (Explicit)
enum class MetadataCategory {
    Technical,
    Descriptive,
    Rights,
    Chapters,
    Artwork
};

// MetaFix Request Types (Maps 1:1 to Engine)
struct RepairRequest {
    std::string input_path;
    std::string output_path;
    RepairMode mode;
    bool allow_essence_modification = false;
    std::optional<std::string> report_path;
    
    struct Flags {
        bool dry_run = false;
        bool json_output = false;
    } flags;
};

struct ValidateRequest {
    std::string input_path;
    std::optional<ValidationProfile> profile;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

struct MetadataCopyRequest {
    std::string source_path;
    std::string target_path;
    std::vector<MetadataCategory> categories;
    bool overwrite = false;
    std::optional<std::string> report_path;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

struct MetadataMergeRequest {
    std::vector<std::string> input_paths;
    std::string output_path;
    std::vector<std::string> precedence_order;
    std::optional<std::string> report_path;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

struct MetadataNormalizeRequest {
    std::string input_path;
    std::string output_path;
    std::string schema_id;
    std::optional<std::string> report_path;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

} // namespace nx::cli