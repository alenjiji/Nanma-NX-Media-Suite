#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>

namespace nx::core {

/**
 * Deterministic error system for Nanma NX-MediaSuite
 * 
 * Provides stable, serializable, and comparable error reporting that:
 * - Uses explicit value-based error reporting (no exceptions as control flow)
 * - Maintains stable error codes across versions and platforms
 * - Supports deterministic error aggregation and ordering
 * - Is fully serializable for automation, CI, and audit logs
 * 
 * Used by all processing engines, BatchFlow failure propagation,
 * Monitor diagnostics, and audit/compliance reports.
 */

/**
 * ErrorCode - Stable numeric error identifier
 * 
 * Represents specific error conditions with stable numeric codes.
 * Codes remain consistent across releases, platforms, and time.
 */
enum class ErrorCode : uint32_t {
    // Success (not an error)
    Success = 0,
    
    // Generic errors (1000-1999)
    UnknownError = 1000,
    InvalidInput = 1001,
    InvalidState = 1002,
    InvalidOperation = 1003,
    
    // Resource errors (2000-2999)
    ResourceNotFound = 2000,
    ResourceUnavailable = 2001,
    ResourceExhausted = 2002,
    ResourceCorrupted = 2003,
    
    // Processing errors (3000-3999)
    ProcessingFailed = 3000,
    ProcessingTimeout = 3001,
    ProcessingCancelled = 3002,
    ProcessingIncomplete = 3003,
    
    // Validation errors (4000-4999)
    ValidationFailed = 4000,
    ValidationIncomplete = 4001,
    ValidationTimeout = 4002,
    
    // System errors (5000-5999)
    SystemError = 5000,
    SystemUnavailable = 5001,
    SystemOverloaded = 5002
};

/**
 * ErrorSeverity - Explicit error severity levels
 * 
 * Deterministic severity classification for error conditions.
 */
enum class ErrorSeverity : uint8_t {
    Info = 0,     // Informational, no action required
    Warning = 1,  // Warning, operation continues
    Error = 2,    // Error, operation fails but system continues
    Fatal = 3     // Fatal, system cannot continue
};

/**
 * ErrorContext - Structured error context information
 * 
 * Immutable context carrying structured information about error conditions.
 * Fully serializable with deterministic ordering.
 */
struct ErrorContext final {
    std::string operation;  // What operation failed
    std::string location;   // Logical location (not source file path)
    std::map<std::string, std::string> parameters;  // Additional context
    
    // Create empty context
    static ErrorContext empty() {
        return ErrorContext{"", "", {}};
    }
    
    // Create context with operation and location
    static ErrorContext create(std::string_view op, std::string_view loc) {
        return ErrorContext{std::string(op), std::string(loc), {}};
    }
    
    // Add parameter to context (returns new ErrorContext)
    ErrorContext with_parameter(std::string_view key, std::string_view value) const {
        ErrorContext result = *this;
        result.parameters[std::string(key)] = std::string(value);
        return result;
    }
    
    // Comparison operators for deterministic ordering
    bool operator==(const ErrorContext&) const noexcept = default;
    bool operator!=(const ErrorContext&) const noexcept = default;
    
    bool operator<(const ErrorContext& other) const noexcept {
        if (operation != other.operation) {
            return operation < other.operation;
        }
        if (location != other.location) {
            return location < other.location;
        }
        return parameters < other.parameters;
    }
    
    // Serialization support
    std::string to_string() const;
};

/**
 * Error - Complete error information
 * 
 * Immutable error value containing code, severity, message, and context.
 * Fully comparable and serializable with deterministic behavior.
 */
struct Error final {
    ErrorCode code;
    ErrorSeverity severity;
    std::string message;
    ErrorContext context;
    
    // Create error with all components
    static Error create(ErrorCode code, ErrorSeverity severity, 
                       std::string_view message, ErrorContext context);
    
    // Create simple error with code and message
    static Error create(ErrorCode code, ErrorSeverity severity, 
                       std::string_view message);
    
    // Check if this represents success (no error)
    bool is_success() const noexcept;
    
    // Check if this represents a failure
    bool is_failure() const noexcept;
    
    // Comparison operators for deterministic ordering
    bool operator==(const Error&) const noexcept = default;
    bool operator!=(const Error&) const noexcept = default;
    bool operator<(const Error& other) const noexcept;
    
    // Serialization support
    std::string to_string() const;
};

/**
 * ErrorList - Deterministic collection of errors
 * 
 * Aggregates multiple errors with deterministic ordering.
 * Supports composable error handling and reporting.
 */
struct ErrorList final {
    std::vector<Error> errors;
    
    // Create empty error list
    static ErrorList empty();
    
    // Create error list with single error
    static ErrorList single(Error error);
    
    // Add error to list (returns new ErrorList)
    ErrorList add(Error error) const;
    
    // Combine with another error list (returns new ErrorList)
    ErrorList combine(const ErrorList& other) const;
    
    // Check if list is empty (no errors)
    bool is_empty() const noexcept;
    
    // Check if list contains any errors
    bool has_errors() const noexcept;
    
    // Get count of errors
    size_t count() const noexcept;
    
    // Check if list contains errors of specified severity or higher
    bool has_severity(ErrorSeverity min_severity) const noexcept;
    
    // Comparison operators
    bool operator==(const ErrorList&) const noexcept = default;
    bool operator!=(const ErrorList&) const noexcept = default;
    
    // Serialization support
    std::string to_string() const;
};

/**
 * Success constant for operations that complete without error
 */
inline Error success() noexcept {
    return Error{ErrorCode::Success, ErrorSeverity::Info, "", ErrorContext::empty()};
}

// Inline implementations for remaining methods

// ErrorContext::to_string implementation
inline std::string ErrorContext::to_string() const {
    std::ostringstream oss;
    oss << "ErrorContext{operation=\"" << operation << "\", location=\"" << location << "\"";
    
    if (!parameters.empty()) {
        oss << ", parameters={";
        bool first = true;
        for (const auto& [key, value] : parameters) {
            if (!first) oss << ", ";
            oss << key << "=\"" << value << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    return oss.str();
}

// Error implementations
inline Error Error::create(ErrorCode code, ErrorSeverity severity, 
                          std::string_view message, ErrorContext context) {
    return Error{code, severity, std::string(message), std::move(context)};
}

inline Error Error::create(ErrorCode code, ErrorSeverity severity, 
                          std::string_view message) {
    return Error{code, severity, std::string(message), ErrorContext::empty()};
}

inline bool Error::is_success() const noexcept {
    return code == ErrorCode::Success;
}

inline bool Error::is_failure() const noexcept {
    return code != ErrorCode::Success;
}

inline bool Error::operator<(const Error& other) const noexcept {
    if (code != other.code) {
        return static_cast<uint32_t>(code) < static_cast<uint32_t>(other.code);
    }
    if (severity != other.severity) {
        return static_cast<uint8_t>(severity) < static_cast<uint8_t>(other.severity);
    }
    if (message != other.message) {
        return message < other.message;
    }
    return context < other.context;
}

inline std::string Error::to_string() const {
    std::ostringstream oss;
    
    // Error code
    oss << "Error{code=" << static_cast<uint32_t>(code);
    
    // Severity
    oss << ", severity=";
    switch (severity) {
        case ErrorSeverity::Info: oss << "Info"; break;
        case ErrorSeverity::Warning: oss << "Warning"; break;
        case ErrorSeverity::Error: oss << "Error"; break;
        case ErrorSeverity::Fatal: oss << "Fatal"; break;
    }
    
    // Message
    oss << ", message=\"" << message << "\"";
    
    // Context (if not empty)
    if (!(context == ErrorContext::empty())) {
        oss << ", context=" << context.to_string();
    }
    
    oss << "}";
    return oss.str();
}

// ErrorList implementations
inline ErrorList ErrorList::empty() {
    return ErrorList{{}};
}

inline ErrorList ErrorList::single(Error error) {
    return ErrorList{{std::move(error)}};
}

inline ErrorList ErrorList::add(Error error) const {
    ErrorList result = *this;
    result.errors.push_back(std::move(error));
    
    // Maintain deterministic ordering
    std::sort(result.errors.begin(), result.errors.end());
    
    return result;
}

inline ErrorList ErrorList::combine(const ErrorList& other) const {
    ErrorList result = *this;
    result.errors.insert(result.errors.end(), other.errors.begin(), other.errors.end());
    
    // Maintain deterministic ordering
    std::sort(result.errors.begin(), result.errors.end());
    
    return result;
}

inline bool ErrorList::is_empty() const noexcept {
    return errors.empty();
}

inline bool ErrorList::has_errors() const noexcept {
    return !errors.empty();
}

inline size_t ErrorList::count() const noexcept {
    return errors.size();
}

inline bool ErrorList::has_severity(ErrorSeverity min_severity) const noexcept {
    return std::any_of(errors.begin(), errors.end(), 
        [min_severity](const Error& error) {
            return static_cast<uint8_t>(error.severity) >= static_cast<uint8_t>(min_severity);
        });
}

inline std::string ErrorList::to_string() const {
    if (errors.empty()) {
        return "ErrorList{empty}";
    }
    
    std::ostringstream oss;
    oss << "ErrorList{count=" << errors.size() << ", errors=[";
    
    for (size_t i = 0; i < errors.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << errors[i].to_string();
    }
    
    oss << "]}";
    return oss.str();
}

} // namespace nx::core