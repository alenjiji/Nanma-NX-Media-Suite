# NX-Core API Contract Rules - Canonical Definition

## Overview

This document defines the **MANDATORY** contract rules that ALL NX-Core APIs must obey. These rules ensure deterministic, explicit, testable, and stable API boundaries that resist architectural drift over time.

**AUTHORITY**: Nanma NX-MediaSuite Architectural Principles document ONLY.

**SCOPE**: All NX-Core public headers, Engine↔Core interaction, BatchFlow↔Engine interaction, API boundaries, automation boundaries, and CI guarantees.

**VIOLATION OF THESE RULES INVALIDATES ARCHITECTURAL GUARANTEES.**

## The Seven Contract Rules

### Rule 1: Function Signature Rules

**REQUIREMENT**: All NX-Core API functions MUST:
- Take explicit inputs only (no hidden globals, statics, singletons)
- Return explicit outputs only (no hidden logging, external mutation)
- Have deterministic signatures that fully describe their behavior

**FORBIDDEN**:
- Hidden inputs from global state
- Hidden outputs via logging or external mutation
- Ambiguous parameter types

**COMPLIANT EXAMPLES**:
```cpp
// ✅ GOOD: Explicit input and output
Result<ProcessedData> process_data(const InputData& input) noexcept;

// ✅ GOOD: All dependencies explicit
Result<ValidationResult> validate_with_rules(
    const InputData& data, 
    const ValidationRules& rules) noexcept;
```

**NON-COMPLIANT EXAMPLES**:
```cpp
// ❌ BAD: Hidden global state dependency
Result<ProcessedData> process_data(); // Where does input come from?

// ❌ BAD: Hidden side effects
Result<ProcessedData> process_and_log(const InputData& input); // Logs internally

// ❌ BAD: Hidden configuration dependency
Result<ProcessedData> process_data(const InputData& input); // Uses global config
```

### Rule 2: Result-Based Error Flow

**REQUIREMENT**: Functions that can fail MUST return `Result<T>`, `VoidResult`, or `MultiResult<T>`.

**FORBIDDEN**:
- Exceptions for control flow
- Sentinel values (nullptr, -1, empty string)
- Implicit success/failure states

**COMPLIANT EXAMPLES**:
```cpp
// ✅ GOOD: Explicit error handling
Result<ProcessedData> process_data(const InputData& input) noexcept;

// ✅ GOOD: Void operation with error handling
VoidResult validate_input(const InputData& input) noexcept;

// ✅ GOOD: Aggregated error handling
MultiResult<std::vector<ProcessedData>> process_batch(
    const std::vector<InputData>& inputs) noexcept;

// ✅ GOOD: Infallible operation (no Result needed)
std::string format_data(const ProcessedData& data) noexcept;
```

**NON-COMPLIANT EXAMPLES**:
```cpp
// ❌ BAD: Exception-based error handling
ProcessedData process_data(const InputData& input); // throws on error

// ❌ BAD: Sentinel value error handling
ProcessedData* process_data(const InputData& input); // returns nullptr on error

// ❌ BAD: Boolean success flag
bool process_data(const InputData& input, ProcessedData& output);

// ❌ BAD: Error code return
int process_data(const InputData& input, ProcessedData& output);
```

### Rule 3: Ownership & Lifetime

**REQUIREMENT**: Ownership must be explicit through:
- Value semantics (`T`)
- Const reference (`const T&`)
- Move semantics (`T&&`)
- Unique ownership (`std::unique_ptr<T>`)
- Explicit shared ownership (`std::shared_ptr<T>`)

**FORBIDDEN**:
- Raw owning pointers (`T*`)
- Ambiguous shared ownership without explicit `std::shared_ptr`
- Unclear lifetime dependencies

**COMPLIANT EXAMPLES**:
```cpp
// ✅ GOOD: Value semantics
Result<ProcessedData> process_by_value(InputData input) noexcept;

// ✅ GOOD: Const reference (no ownership transfer)
Result<ProcessedData> process_by_ref(const InputData& input) noexcept;

// ✅ GOOD: Move semantics (ownership transfer)
Result<ProcessedData> process_by_move(InputData&& input) noexcept;

// ✅ GOOD: Explicit unique ownership
std::unique_ptr<ProcessedData> create_data() noexcept;

// ✅ GOOD: Explicit shared ownership
std::shared_ptr<const ProcessedData> get_shared_data() noexcept;
```

**NON-COMPLIANT EXAMPLES**:
```cpp
// ❌ BAD: Raw owning pointer
ProcessedData* create_data(); // Who owns this?

// ❌ BAD: Ambiguous lifetime
ProcessedData& get_data(); // Lifetime unclear

// ❌ BAD: Hidden shared ownership
ProcessedData* get_shared_data(); // Shared but not explicit
```

### Rule 4: Immutability at Boundaries

**REQUIREMENT**: API boundaries must preserve immutability:
- Inputs should be immutable by default (`const T&` or `T`)
- Outputs must not expose internal mutable state
- Any mutation must be explicit and local

**COMPLIANT EXAMPLES**:
```cpp
// ✅ GOOD: Immutable input
Result<ProcessedData> process_data(const InputData& input) noexcept;

// ✅ GOOD: Value input (immutable copy)
Result<ProcessedData> process_data(InputData input) noexcept;

// ✅ GOOD: Safe output (value)
Result<ProcessedData> get_processed_data() noexcept;

// ✅ GOOD: Explicit mutation
VoidResult update_data(ProcessedData& data, const UpdateSpec& spec) noexcept;
```

**NON-COMPLIANT EXAMPLES**:
```cpp
// ❌ BAD: Mutable reference input
Result<ProcessedData> process_data(InputData& input); // Might mutate

// ❌ BAD: Exposing internal mutable state
ProcessedData& get_internal_data(); // Exposes internals

// ❌ BAD: Hidden mutation
Result<ProcessedData> process_data(const InputData& input); // Mutates global state
```

### Rule 5: Deterministic Behavior

**REQUIREMENT**: APIs must guarantee: Same inputs → Same outputs

**FORBIDDEN**:
- Dependency on wall-clock time
- Dependency on system state
- Dependency on thread scheduling
- Non-deterministic side effects

**COMPLIANT EXAMPLES**:
```cpp
// ✅ GOOD: Pure function
Result<ProcessedData> process_data(const InputData& input) noexcept;

// ✅ GOOD: Explicit time dependency
Result<ProcessedData> process_with_time(
    const InputData& input, 
    LogicalTime time) noexcept;

// ✅ GOOD: Explicit randomness
Result<ProcessedData> process_with_seed(
    const InputData& input, 
    uint64_t seed) noexcept;
```

**NON-COMPLIANT EXAMPLES**:
```cpp
// ❌ BAD: Wall-clock time dependency
Result<ProcessedData> process_with_timestamp(const InputData& input);

// ❌ BAD: System state dependency
Result<ProcessedData> process_with_system_info(const InputData& input);

// ❌ BAD: Non-deterministic randomness
Result<ProcessedData> process_with_random(const InputData& input);

// ❌ BAD: Thread-dependent behavior
Result<ProcessedData> process_with_thread_id(const InputData& input);
```

### Rule 6: Serialization Stability

**REQUIREMENT**: Types crossing API boundaries must be:
- Comparable (`operator==`, `operator!=`)
- Serializable (`to_string()` method)
- Stable across platforms and versions

**COMPLIANT EXAMPLES**:
```cpp
struct ApiCompliantType {
    std::string data;
    int32_t value;
    
    // Required: Comparison operators
    bool operator==(const ApiCompliantType&) const noexcept = default;
    bool operator!=(const ApiCompliantType&) const noexcept = default;
    
    // Required: Serialization
    std::string to_string() const {
        return data + ":" + std::to_string(value);
    }
};
```

**NON-COMPLIANT EXAMPLES**:
```cpp
struct NonCompliantType {
    std::string data;
    int32_t value;
    
    // ❌ BAD: No comparison operators
    // ❌ BAD: No serialization method
};
```

### Rule 7: Versioning & Extensibility

**REQUIREMENT**: APIs must be designed for evolution without breaking determinism:
- Stable function signatures
- Additive changes only
- No "future convenience" hooks

**COMPLIANT EXAMPLES**:
```cpp
// ✅ GOOD: Stable signature
Result<ProcessedData> process_data_v1(const InputData& input) noexcept;

// ✅ GOOD: Additive extension
Result<ProcessedData> process_data_v2(
    const InputData& input,
    const ProcessingOptions& options = ProcessingOptions::default_options()) noexcept;

// ✅ GOOD: Version-specific types
struct ProcessingOptionsV2 {
    ProcessingOptionsV1 v1_options;
    AdditionalOptions v2_options;
    
    static ProcessingOptionsV2 from_v1(const ProcessingOptionsV1& v1);
};
```

**NON-COMPLIANT EXAMPLES**:
```cpp
// ❌ BAD: Breaking signature change
Result<DifferentData> process_data(const DifferentInput& input); // Breaking change

// ❌ BAD: Future convenience hook
Result<ProcessedData> process_data(
    const InputData& input,
    void* future_options = nullptr); // Unclear contract

// ❌ BAD: Version-dependent behavior
Result<ProcessedData> process_data(
    const InputData& input,
    int api_version); // Non-deterministic
```

## Contract Enforcement

### Compile-Time Validation

Use the provided concepts and macros to enforce contract compliance:

```cpp
#include "api_contract.h"

// Validate function signature compliance
NX_CORE_API_FUNCTION(my_function, const InputType&, Result<OutputType>);

// Validate API version
NX_CORE_API_VERSION(1, 0);

// Use concepts directly
template<typename T>
requires ApiSerializable<T>
Result<std::string> serialize_data(const T& data) noexcept {
    return ok(data.to_string());
}
```

### Runtime Validation

Use runtime validation helpers for testing:

```cpp
// Validate serialization compliance
assert(validate_serializable(my_data));

// Validate deterministic serialization
assert(validate_deterministic_serialization(data1, data2));
```

## Integration with NX-Core Systems

### With Identity System
```cpp
Result<JobID> create_job_id(const JobSpec& spec) noexcept {
    if (!spec.is_valid()) {
        return fail<JobID>(ErrorCode::InvalidInput, ErrorSeverity::Error, "Invalid job spec");
    }
    return ok(JobID::generate(spec.to_string()));
}
```

### With Error System
```cpp
Result<ProcessedData> process_with_validation(const InputData& input) noexcept {
    auto validation_result = validate_input(input);
    if (validation_result.is_failure()) {
        return fail<ProcessedData>(validation_result.get_error());
    }
    
    return perform_processing(input);
}
```

### With Result System
```cpp
MultiResult<std::vector<ProcessedData>> process_batch_with_errors(
    const std::vector<InputData>& inputs) noexcept {
    
    std::vector<ProcessedData> results;
    ErrorList errors = ErrorList::empty();
    
    for (const auto& input : inputs) {
        auto result = process_single(input);
        if (result.is_success()) {
            results.push_back(result.get_value());
        } else {
            errors = errors.add(result.get_error());
        }
    }
    
    if (errors.has_errors()) {
        return multi_fail<std::vector<ProcessedData>>(errors);
    }
    
    return multi_ok(std::move(results));
}
```

## Best Practices

### API Design Checklist

Before implementing any NX-Core API function, verify:

1. **Explicit Dependencies**: All inputs are explicit parameters
2. **Result-Based Errors**: Fallible functions return Result types
3. **Clear Ownership**: Parameter ownership is unambiguous
4. **Immutable Boundaries**: Inputs are immutable, outputs are safe
5. **Deterministic Logic**: Same inputs always produce same outputs
6. **Serializable Types**: All types implement comparison and serialization
7. **Version Stability**: Function signature is stable and extensible

### Common Patterns

**Configuration Pattern**:
```cpp
struct ProcessingConfig {
    std::string operation_name;
    std::map<std::string, std::string> parameters;
    
    bool operator==(const ProcessingConfig&) const noexcept = default;
    bool operator!=(const ProcessingConfig&) const noexcept = default;
    std::string to_string() const;
    
    static ProcessingConfig default_config();
    ProcessingConfig with_parameter(std::string_view key, std::string_view value) const;
};

Result<ProcessedData> process_with_config(
    const InputData& input,
    const ProcessingConfig& config = ProcessingConfig::default_config()) noexcept;
```

**Builder Pattern**:
```cpp
class ProcessingRequestBuilder {
public:
    ProcessingRequestBuilder& with_input(InputData input);
    ProcessingRequestBuilder& with_option(std::string_view key, std::string_view value);
    ProcessingRequestBuilder& with_timeout(std::chrono::milliseconds timeout);
    
    Result<ProcessingRequest> build() const noexcept;
};
```

**Factory Pattern**:
```cpp
Result<std::unique_ptr<Processor>> create_processor(
    const ProcessorConfig& config) noexcept;
```

## Violation Consequences

Violating these contract rules results in:

1. **Compile-time errors** from concept validation
2. **Test failures** in contract compliance tests
3. **Architectural guarantee invalidation**
4. **Non-deterministic behavior** in production
5. **CI/CD pipeline failures**
6. **Audit trail corruption**

## Migration Guide

When updating existing APIs to comply with these rules:

1. **Identify violations** using the provided concepts
2. **Wrap return values** in appropriate Result types
3. **Make dependencies explicit** through parameters
4. **Add serialization methods** to boundary types
5. **Remove side effects** from pure functions
6. **Update tests** to validate contract compliance
7. **Document breaking changes** and migration paths

## Conclusion

These API contract rules form the foundation of NX-Core's architectural guarantees. They ensure that all APIs are deterministic, explicit, testable, and stable over time. Strict adherence to these rules is mandatory for maintaining the integrity of the Nanma NX-MediaSuite architecture.