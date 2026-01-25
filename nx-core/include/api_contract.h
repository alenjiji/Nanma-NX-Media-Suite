#pragma once

/**
 * NX-Core API Contract Rules - Canonical Definition
 * 
 * This header defines the MANDATORY contract rules that ALL NX-Core APIs must obey.
 * These rules ensure deterministic, explicit, testable, and stable API boundaries
 * that resist architectural drift over time.
 * 
 * SCOPE: All NX-Core public headers, Engine↔Core interaction, BatchFlow↔Engine
 * interaction, API boundaries, automation boundaries, and CI guarantees.
 * 
 * AUTHORITY: Nanma NX-MediaSuite Architectural Principles document ONLY.
 * 
 * VIOLATION OF THESE RULES INVALIDATES ARCHITECTURAL GUARANTEES.
 */

#include "result.h"
#include "error_system.h"
#include <type_traits>
#include <string_view>
#include <memory>

namespace nx::core::contract {

/**
 * CONTRACT RULE 1: FUNCTION SIGNATURE RULES
 * 
 * All NX-Core API functions MUST:
 * - Take explicit inputs only (no hidden globals, statics, singletons)
 * - Return explicit outputs only (no hidden logging, external mutation)
 * - Have deterministic signatures that fully describe their behavior
 * 
 * FORBIDDEN:
 * - Hidden inputs from global state
 * - Hidden outputs via logging or external mutation
 * - Ambiguous parameter types
 */

// COMPLIANT: Explicit inputs and outputs
template<typename InputType, typename OutputType>
concept ExplicitFunction = requires(InputType input) {
    // Function must take explicit input and return explicit Result
    { function(input) } -> std::same_as<Result<OutputType>>;
};

/**
 * CONTRACT RULE 2: RESULT-BASED ERROR FLOW
 * 
 * Functions that can fail MUST return Result<T>, VoidResult, or MultiResult<T>.
 * 
 * FORBIDDEN:
 * - Exceptions for control flow
 * - Sentinel values (nullptr, -1, empty string)
 * - Implicit success/failure states
 */

// Type trait to enforce Result-based returns for fallible functions
template<typename T>
struct is_valid_api_return : std::false_type {};

template<typename T>
struct is_valid_api_return<Result<T>> : std::true_type {};

template<>
struct is_valid_api_return<VoidResult> : std::true_type {};

template<typename T>
struct is_valid_api_return<MultiResult<T>> : std::true_type {};

// For infallible functions, direct value return is allowed for non-pointer, non-bool types
template<typename T>
struct is_infallible_return : std::bool_constant<!std::is_pointer_v<T> && !std::is_same_v<T, bool>> {};

template<typename ReturnType>
concept ValidApiReturn = is_valid_api_return<ReturnType>::value || is_infallible_return<ReturnType>::value;

/**
 * CONTRACT RULE 3: OWNERSHIP & LIFETIME
 * 
 * Ownership must be explicit through:
 * - Value semantics (T)
 * - Const reference (const T&)
 * - Move semantics (T&&)
 * - Unique ownership (std::unique_ptr<T>)
 * 
 * FORBIDDEN:
 * - Raw owning pointers (T*)
 * - Ambiguous shared ownership without explicit std::shared_ptr
 * - Unclear lifetime dependencies
 */

template<typename T>
concept ValidOwnership = 
    std::is_same_v<T, std::decay_t<T>> ||                    // Value
    std::is_lvalue_reference_v<const T> ||                   // Const reference
    std::is_rvalue_reference_v<T> ||                         // Move
    std::is_same_v<T, std::unique_ptr<typename T::element_type>> || // Unique ptr
    std::is_same_v<T, std::shared_ptr<typename T::element_type>>;   // Shared ptr (explicit only)

// Raw pointers are forbidden for ownership
template<typename T>
concept ForbiddenOwnership = std::is_pointer_v<T>;

/**
 * CONTRACT RULE 4: IMMUTABILITY AT BOUNDARIES
 * 
 * API boundaries must preserve immutability:
 * - Inputs should be immutable by default (const T& or T)
 * - Outputs must not expose internal mutable state
 * - Any mutation must be explicit and local
 */

template<typename T>
concept ImmutableInput = 
    std::is_same_v<T, std::decay_t<T>> ||           // Value (immutable copy)
    std::is_same_v<T, const std::decay_t<T>&>;      // Const reference

template<typename T>
concept SafeOutput = 
    std::is_same_v<T, std::decay_t<T>> ||           // Value (safe copy)
    is_valid_api_return<T>::value ||                // Result types (safe)
    is_infallible_return<T>::value;                 // Infallible types (safe)

/**
 * CONTRACT RULE 5: DETERMINISTIC BEHAVIOR
 * 
 * APIs must guarantee: Same inputs → Same outputs
 * 
 * FORBIDDEN:
 * - Dependency on wall-clock time
 * - Dependency on system state
 * - Dependency on thread scheduling
 * - Non-deterministic side effects
 */

// Marker concept for deterministic functions
template<typename Func>
concept DeterministicFunction = requires {
    // Function must be marked as deterministic (compile-time check)
    typename Func::is_deterministic;
};

/**
 * CONTRACT RULE 6: SERIALIZATION STABILITY
 * 
 * Types crossing API boundaries must be:
 * - Comparable (operator==, operator!=)
 * - Serializable (to_string() method)
 * - Stable across platforms and versions
 */

template<typename T>
concept ApiSerializable = requires(const T& t) {
    { t == t } -> std::same_as<bool>;
    { t != t } -> std::same_as<bool>;
    { t.to_string() } -> std::same_as<std::string>;
};

/**
 * CONTRACT RULE 7: VERSIONING & EXTENSIBILITY
 * 
 * APIs must be designed for evolution without breaking determinism:
 * - Stable function signatures
 * - Additive changes only
 * - No "future convenience" hooks
 */

// Version stability marker
template<uint32_t MajorVersion, uint32_t MinorVersion>
struct ApiVersion {
    static constexpr uint32_t major = MajorVersion;
    static constexpr uint32_t minor = MinorVersion;
};

// Current NX-Core API version
using CurrentApiVersion = ApiVersion<1, 0>;

/**
 * COMPLETE API CONTRACT VALIDATION
 * 
 * Concept that validates a function signature against ALL contract rules
 */
template<typename Func, typename Input, typename Output>
concept CompliantApiFunction = 
    ValidOwnership<Input> &&
    !ForbiddenOwnership<Input> &&
    ImmutableInput<Input> &&
    ValidApiReturn<Output> &&
    SafeOutput<Output> &&
    ApiSerializable<std::decay_t<Input>> &&
    ApiSerializable<std::decay_t<Output>>;

/**
 * EXAMPLE COMPLIANT FUNCTION SIGNATURES
 * 
 * These examples demonstrate correct API contract usage:
 */

namespace examples {

// COMPLIANT: Simple transformation with explicit error handling
struct ProcessingConfig {
    std::string operation_name;
    int32_t parameter_value;
    
    bool operator==(const ProcessingConfig&) const noexcept = default;
    bool operator!=(const ProcessingConfig&) const noexcept = default;
    std::string to_string() const { return operation_name + ":" + std::to_string(parameter_value); }
};

struct ProcessingResult {
    std::string result_data;
    int32_t result_code;
    
    bool operator==(const ProcessingResult&) const noexcept = default;
    bool operator!=(const ProcessingResult&) const noexcept = default;
    std::string to_string() const { return result_data + ":" + std::to_string(result_code); }
};

// COMPLIANT: Explicit inputs, Result-based error handling, deterministic
Result<ProcessingResult> process_data(const ProcessingConfig& config) noexcept;

// COMPLIANT: Void operation with explicit error handling
VoidResult validate_configuration(const ProcessingConfig& config) noexcept;

// COMPLIANT: Batch operation with aggregated error handling
MultiResult<std::vector<ProcessingResult>> process_batch(
    const std::vector<ProcessingConfig>& configs) noexcept;

// COMPLIANT: Infallible operation (no Result needed)
std::string format_result(const ProcessingResult& result) noexcept;

// COMPLIANT: Move semantics for efficiency
Result<ProcessingResult> transform_data(ProcessingConfig&& config) noexcept;

/**
 * NON-COMPLIANT EXAMPLES (FORBIDDEN)
 */

// FORBIDDEN: Exception-based error handling
// ProcessingResult process_data_bad(const ProcessingConfig& config); // throws

// FORBIDDEN: Sentinel value error handling
// ProcessingResult* process_data_bad(const ProcessingConfig& config); // returns nullptr on error

// FORBIDDEN: Hidden global state dependency
// Result<ProcessingResult> process_data_bad(); // no explicit input

// FORBIDDEN: Hidden side effects
// Result<ProcessingResult> process_data_bad(const ProcessingConfig& config); // logs internally

// FORBIDDEN: Raw owning pointer
// ProcessingResult* create_result_bad(); // unclear ownership

// FORBIDDEN: Mutable reference output
// void get_result_bad(ProcessingResult& out); // exposes internal state

// FORBIDDEN: Non-deterministic behavior
// Result<ProcessingResult> process_with_timestamp(); // depends on wall-clock time

} // namespace examples

/**
 * CONTRACT ENFORCEMENT MACROS
 * 
 * Use these macros to enforce contract compliance at compile time
 */

#define NX_CORE_API_FUNCTION(func_name, input_type, output_type) \
    static_assert(CompliantApiFunction<decltype(func_name), input_type, output_type>, \
                  #func_name " violates NX-Core API contract rules")

#define NX_CORE_API_VERSION(major, minor) \
    static_assert(major >= CurrentApiVersion::major, \
                  "API version cannot be lower than current version"); \
    using ApiVersionTag = ApiVersion<major, minor>

/**
 * RUNTIME CONTRACT VALIDATION
 * 
 * Helper functions for runtime validation of contract compliance
 */

template<typename T>
bool validate_serializable(const T& value) noexcept {
    try {
        // Test serialization
        std::string serialized = value.to_string();
        
        // Test comparison
        bool equal = (value == value);
        bool not_equal = (value != value);
        
        return !serialized.empty() && equal && !not_equal;
    } catch (...) {
        return false;
    }
}

template<typename T>
bool validate_deterministic_serialization(const T& value1, const T& value2) noexcept {
    if (!(value1 == value2)) return true; // Different values, no constraint
    
    try {
        return value1.to_string() == value2.to_string();
    } catch (...) {
        return false;
    }
}

} // namespace nx::core::contract