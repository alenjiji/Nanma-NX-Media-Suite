// PHASE 15 INVARIANT:
// This CLI invoker is a subprocess bridge only.
// It adds no retries, no argument mutation, no error rewriting.
// All execution is deterministic and mockable.

#pragma once

#include <string>
#include <vector>

namespace nx::ui::cli {

struct CliExecutionResult {
    int exit_code;
    std::string stdout_content;
    std::string stderr_content;
};

class CliInvoker {
public:
    virtual ~CliInvoker() = default;
    virtual CliExecutionResult execute(const std::vector<std::string>& args) = 0;
};

class SubprocessCliInvoker : public CliInvoker {
public:
    explicit SubprocessCliInvoker(const std::string& nx_executable_path);
    CliExecutionResult execute(const std::vector<std::string>& args) override;

private:
    std::string nx_executable_path_;
};

class MockCliInvoker : public CliInvoker {
public:
    void set_mock_result(const CliExecutionResult& result);
    CliExecutionResult execute(const std::vector<std::string>& args) override;
    
    const std::vector<std::string>& last_args() const;

private:
    CliExecutionResult mock_result_;
    std::vector<std::string> last_args_;
};

} // namespace nx::ui::cli