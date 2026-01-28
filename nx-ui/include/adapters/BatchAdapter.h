// PHASE 15 INVARIANT:
// This adapter is a stateless CLI bridge.
// It adds no defaults, no retries, no inference.
// All parameters map 1:1 to CLI arguments.
// If a value is missing, this is a caller error.

#pragma once

#include <string>

namespace nx::ui::adapters {

struct BatchRunDto {};
struct BatchValidateDto {};
struct BatchSummaryDto {};

template <typename Dto>
struct CliTypedResult {
    int exit_code;
    std::string raw_json;
};

struct BatchRunArgs {
    std::string batch_file_path;
    std::string format;
    bool dry_run;
};

struct BatchValidateArgs {
    std::string batch_file_path;
    std::string format;
};

struct BatchSummaryArgs {
    std::string batch_file_path;
    std::string format;
};

class BatchAdapter {
public:
    static CliTypedResult<BatchRunDto> run(const BatchRunArgs& args);
    static CliTypedResult<BatchValidateDto> validate(const BatchValidateArgs& args);
    static CliTypedResult<BatchSummaryDto> summarize(const BatchSummaryArgs& args);
};

} // namespace nx::ui::adapters