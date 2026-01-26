#include "batch_command.h"
#include "batch_argument_parser.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

namespace nx::cli {

CliResult BatchCommand::execute(const std::vector<std::string>& args) {
    if (args.empty() || args[0] == "--help" || args[0] == "-h") {
        std::cout << "nx batch - Deterministic command list executor\\n\\n";
        std::cout << "Operations:\\n";
        std::cout << "  run         Execute batch file sequentially\\n";
        std::cout << "  validate    Validate batch file without execution\\n";
        std::cout << "  summarize   Static summary of batch file contents\\n\\n";
        std::cout << "Use 'nx batch <operation> --help' for operation-specific help\\n";
        return CliResult::ok();
    }
    
    const std::string& operation = args[0];
    std::vector<std::string> operation_args(args.begin() + 1, args.end());
    
    if (operation == "run") {
        BatchRunRequest request;
        auto parse_result = BatchArgumentParser::parse_run_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_run(request);
        
    } else if (operation == "validate") {
        BatchValidateRequest request;
        auto parse_result = BatchArgumentParser::parse_validate_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_validate(request);
        
    } else if (operation == "summarize") {
        BatchSummaryRequest request;
        auto parse_result = BatchArgumentParser::parse_summary_args(operation_args, request);
        if (!parse_result.success) {
            return CliResult::error(parse_result.error_code, parse_result.message);
        }
        return handle_summarize(request);
        
    } else {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Unknown batch operation: " + operation + ". Available: run, validate, summarize"
        );
    }
}

CliResult BatchCommand::handle_run(const BatchRunRequest& request) {
    std::vector<std::string> commands;
    auto file_result = read_batch_file(request.batch_file, commands);
    if (!file_result.success) {
        return file_result;
    }
    
    // Validate all commands before execution
    for (size_t i = 0; i < commands.size(); ++i) {
        auto validation_result = validate_command_line(commands[i], static_cast<int>(i + 1));
        if (!validation_result.success) {
            return validation_result;
        }
    }
    
    print_run_output(request, commands);
    return CliResult::ok();
}

CliResult BatchCommand::handle_validate(const BatchValidateRequest& request) {
    std::vector<std::string> commands;
    auto file_result = read_batch_file(request.batch_file, commands);
    if (!file_result.success) {
        return file_result;
    }
    
    // Validate all commands
    for (size_t i = 0; i < commands.size(); ++i) {
        auto validation_result = validate_command_line(commands[i], static_cast<int>(i + 1));
        if (!validation_result.success) {
            return validation_result;
        }
    }
    
    print_validate_output(request, commands);
    return CliResult::ok();
}

CliResult BatchCommand::handle_summarize(const BatchSummaryRequest& request) {
    std::vector<std::string> commands;
    auto file_result = read_batch_file(request.batch_file, commands);
    if (!file_result.success) {
        return file_result;
    }
    
    // Validate all commands
    for (size_t i = 0; i < commands.size(); ++i) {
        auto validation_result = validate_command_line(commands[i], static_cast<int>(i + 1));
        if (!validation_result.success) {
            return validation_result;
        }
    }
    
    print_summary_output(request, commands);
    return CliResult::ok();
}

CliResult BatchCommand::read_batch_file(const std::string& file_path, std::vector<std::string>& commands) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Cannot read batch file: " + file_path
        );
    }
    
    commands.clear();
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        
        // Reject empty lines
        if (line.empty()) {
            return CliResult::error(
                CliErrorCode::NX_CLI_USAGE_ERROR,
                "Empty command at line " + std::to_string(line_number)
            );
        }
        
        commands.push_back(line);
    }
    
    return CliResult::ok();
}

CliResult BatchCommand::validate_command_line(const std::string& line, int line_number) {
    // Reject empty lines (already filtered in read_batch_file)
    if (line.empty()) {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Empty command at line " + std::to_string(line_number)
        );
    }
    
    // Must start with "nx "
    if (!line.starts_with("nx ")) {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Invalid command at line " + std::to_string(line_number) + ": must start with 'nx '"
        );
    }
    
    // Check for forbidden shell operators
    std::vector<std::string> forbidden = {"&&", "|", ";", "$", "${", "./nx"};
    for (const auto& op : forbidden) {
        if (line.find(op) != std::string::npos) {
            return CliResult::error(
                CliErrorCode::NX_CLI_USAGE_ERROR,
                "Forbidden operator '" + op + "' at line " + std::to_string(line_number)
            );
        }
    }
    
    // Extract component name (word after "nx ")
    std::istringstream iss(line);
    std::string nx, component;
    iss >> nx >> component;
    
    if (component.empty()) {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Missing component at line " + std::to_string(line_number)
        );
    }
    
    // Validate known components (exclude batch to prevent recursion)
    std::vector<std::string> known_components = {"convert", "metafix", "audio", "video"};
    if (std::find(known_components.begin(), known_components.end(), component) == known_components.end()) {
        return CliResult::error(
            CliErrorCode::NX_CLI_USAGE_ERROR,
            "Unknown component '" + component + "' at line " + std::to_string(line_number)
        );
    }
    
    return CliResult::ok();
}

void BatchCommand::print_run_output(const BatchRunRequest& request, const std::vector<std::string>& commands) {
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"run\",\n";
        std::cout << "  \"file\": \"" << request.batch_file << "\",\n";
        std::cout << "  \"dry_run\": " << (request.flags.dry_run ? "true" : "false") << ",\n";
        std::cout << "  \"commands\": [\n";
        
        for (size_t i = 0; i < commands.size(); ++i) {
            std::cout << "    { \"index\": " << (i + 1) << ", \"command\": \"" << commands[i] << "\" }";
            if (i < commands.size() - 1) std::cout << ",";
            std::cout << "\n";
        }
        
        std::cout << "  ]\n";
        std::cout << "}\n";
    } else {
        std::cout << "Batch execution plan:\n";
        for (size_t i = 0; i < commands.size(); ++i) {
            std::cout << (i + 1) << ". " << commands[i] << "\n";
        }
    }
}

void BatchCommand::print_validate_output(const BatchValidateRequest& request, const std::vector<std::string>& commands) {
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"validate\",\n";
        std::cout << "  \"file\": \"" << request.batch_file << "\",\n";
        std::cout << "  \"valid\": true,\n";
        std::cout << "  \"command_count\": " << commands.size() << "\n";
        std::cout << "}\n";
    } else {
        std::cout << "Batch file validation: PASSED\n";
        std::cout << "Commands: " << commands.size() << "\n";
        std::cout << "File: " << request.batch_file << "\n";
    }
}

void BatchCommand::print_summary_output(const BatchSummaryRequest& request, const std::vector<std::string>& commands) {
    // Count commands per component
    std::map<std::string, int> component_counts;
    
    for (const auto& command : commands) {
        std::istringstream iss(command);
        std::string nx, component;
        iss >> nx >> component;
        
        if (!component.empty()) {
            component_counts[component]++;
        }
    }
    
    if (request.flags.json_output) {
        std::cout << "{\n";
        std::cout << "  \"operation\": \"summarize\",\n";
        std::cout << "  \"file\": \"" << request.batch_file << "\",\n";
        std::cout << "  \"total_commands\": " << commands.size() << ",\n";
        std::cout << "  \"components\": {\n";
        
        bool first = true;
        for (const auto& [component, count] : component_counts) {
            if (!first) std::cout << ",\n";
            std::cout << "    \"" << component << "\": " << count;
            first = false;
        }
        
        std::cout << "\n  }\n";
        std::cout << "}\n";
    } else {
        std::cout << "Batch file summary:\n";
        std::cout << "File: " << request.batch_file << "\n";
        std::cout << "Total commands: " << commands.size() << "\n";
        std::cout << "Components:\n";
        
        for (const auto& [component, count] : component_counts) {
            std::cout << "  " << component << ": " << count << "\n";
        }
    }
}

} // namespace nx::cli