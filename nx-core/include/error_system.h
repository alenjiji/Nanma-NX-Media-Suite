#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <map>

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
    static ErrorContext empty();
    
    // Create context with operation and location
    static ErrorContext create(std::string_view op, std::string_view loc);
    
    // Add parameter to context (returns new ErrorContext)
    ErrorContext with_parameter(std::string_view key, std::string_view value) const;
    
    // Comparison operators for deterministic ordering
    bool operator==(const ErrorContext&) const noexcept = default;
    bool operator!=(const ErrorContext&) const noexcept = default;
    bool operator<(const ErrorContext& other) const noexcept;
    
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

} // namespace nx::core