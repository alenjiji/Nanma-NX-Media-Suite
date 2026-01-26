#pragma once

#include <string>
#include <vector>
#include <optional>

namespace nx::cli {

// CLI Error Codes (Phase 4 specification)
enum class CliErrorCode {
    NONE,                  // Success, no error
    NX_CLI_USAGE_ERROR,    // Invalid flags / syntax
    NX_CLI_ENUM_ERROR,     // Unknown enum
    NX_ENGINE_REJECTED,    // Engine validation failure
    NX_EXEC_FAILED         // Runtime failure
};

// Stream Policy Types (Explicit)
enum class VideoPolicy {
    Passthrough,
    Encode
};

enum class AudioPolicy {
    Passthrough,
    Encode
};

enum class SubtitlePolicy {
    Passthrough,
    Drop
};

enum class MetadataPolicy {
    Passthrough,
    Strip
};

// Container Types (Static Registry)
enum class ContainerType {
    MP4,
    MOV,
    MKV,
    AVI
};

// Transcode Request (Maps 1:1 to Engine)
struct TranscodeRequest {
    std::string input_path;
    std::string output_path;
    ContainerType target_container;
    VideoPolicy video_policy;
    std::string video_codec;  // Only used if video_policy == Encode
    AudioPolicy audio_policy;
    std::string audio_codec;  // Only used if audio_policy == Encode
    std::optional<SubtitlePolicy> subtitle_policy;  // Unspecified if not provided
    std::optional<MetadataPolicy> metadata_policy;  // Unspecified if not provided
    
    struct Flags {
        bool dry_run = false;
        bool json_output = false;
    } flags;
};

// CLI Result
struct CliResult {
    CliErrorCode error_code;
    std::string message;
    bool success;
    
    static CliResult ok() {
        return {CliErrorCode::NONE, "", true};
    }
    
    static CliResult error(CliErrorCode code, const std::string& msg) {
        return {code, msg, false};
    }
};

} // namespace nx::cli