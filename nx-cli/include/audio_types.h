#pragma once

#include <string>
#include <vector>
#include <optional>

namespace nx::cli {

// Loudness Standards (Explicit)
enum class LoudnessStandard {
    EbuR128,
    ItuBs1770,
    AtscA85
};

// Dither Types (Explicit)
enum class DitherType {
    Triangular,
    Rectangular,
    NoiseShaped
};

// Audio Request Types (Maps 1:1 to Engine)
struct AudioMeasureRequest {
    std::string input_path;
    LoudnessStandard standard;
    std::optional<std::string> report_path;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

struct AudioProcessRequest {
    std::string input_path;
    std::string output_path;
    
    // DSP Nodes (Explicit)
    std::optional<int> src_rate;
    std::optional<double> gain_db;
    std::optional<std::string> loudness_target;  // e.g. "-23LUFS"
    std::optional<int> bit_depth;
    std::optional<DitherType> dither_type;
    
    std::optional<std::string> report_path;
    
    struct Flags {
        bool dry_run = false;
        bool json_output = false;
    } flags;
    
    // Validation helper
    bool has_dsp_operations() const {
        return src_rate.has_value() || 
               gain_db.has_value() || 
               loudness_target.has_value() || 
               bit_depth.has_value() || 
               dither_type.has_value();
    }
};

struct AudioVerifyRequest {
    std::string input_path;
    std::string output_path;
    
    struct Flags {
        bool json_output = false;
    } flags;
};

} // namespace nx::cli