# NX-Core Result System Documentation

## Overview

The Result system provides deterministic, explicit success/failure representation for all NX-Core APIs. It eliminates ambiguous return values, exceptions as control flow, and implicit success/failure states.

## Design Principles

### Value-Based Design
- All Result types are value types (no inheritance, no polymorphism)
- No virtual functions or dynamic dispatch
- Immutable once created
- No global state dependencies

### Explicit Success/Failure
- Success and failure states are mutually exclusive
- No implicit truthiness (no `operator bool()`)
- No implicit conversions
- Clear factory functions for construction

### Error Integration
- Integrates with NX-Core Error system (Unit 0.3)
- Failure states carry Error or ErrorList objects
- No separate error mechanism

### Deterministic Behavior
- Same inputs produce identical results
- Same errors produce identical failure results
- Stable serialization format
- Consistent ordering for aggregated errors

## Result Types

### Result<T>
Primary result type for operations returning a value of type T.

```cpp
// Success construction
auto success = ok(42);
auto success2 = ok(std::string(\"hello\"));

// Failure construction
auto failure = fail<int>(ErrorCode::InvalidInput, ErrorSeverity::Error, \"Invalid value\");

// State checking
if (result.is_success()) {
    int value = result.get_value();
} else {
    Error error = result.get_error();
}
```

**Constraints:**
- T cannot be a reference type
- T cannot be void (use VoidResult instead)

### VoidResult
Result type for operations that don't return values.

```cpp
// Success construction
auto success = ok();

// Failure construction
auto failure = fail(ErrorCode::ProcessingFailed, ErrorSeverity::Error, \"Processing failed\");

// State checking
if (result.is_success()) {
    // Operation succeeded
} else {
    Error error = result.get_error();
}
```

### MultiResult<T>
Result type supporting aggregated errors from multiple failure sources.

```cpp
// Success construction
auto success = multi_ok(42);

// Failure construction with single error
auto failure1 = multi_fail<int>(error);

// Failure construction with error list
auto error_list = ErrorList::single(error1).add(error2);
auto failure2 = multi_fail<int>(error_list);

// State checking
if (result.is_success()) {
    int value = result.get_value();
} else {
    ErrorList errors = result.get_errors();
    for (const auto& error : errors.errors) {
        // Handle each error
    }
}
```

## Factory Functions

### Success Construction
```cpp
// For Result<T>
auto result = ok(value);

// For VoidResult
auto result = ok();

// For MultiResult<T>
auto result = multi_ok(value);
```

### Failure Construction
```cpp
// With Error object
auto result = fail<T>(error);

// With error parameters
auto result = fail<T>(ErrorCode::InvalidInput, ErrorSeverity::Error, \"Message\");

// With context
auto context = ErrorContext::create(\"operation\", \"location\");
auto result = fail<T>(ErrorCode::ProcessingFailed, ErrorSeverity::Fatal, \"Failed\", context);
```

## Usage Patterns

### API Design
```cpp
// Function returning a result
Result<ProcessedData> process_data(const InputData& input) {
    if (!validate_input(input)) {
        return fail<ProcessedData>(ErrorCode::InvalidInput, ErrorSeverity::Error, \"Invalid input data\");
    }
    
    ProcessedData data = perform_processing(input);
    return ok(std::move(data));
}

// Calling code
auto result = process_data(input);
if (result.is_success()) {
    ProcessedData data = result.get_value();
    // Use data
} else {
    Error error = result.get_error();
    // Handle error
}
```

### Error Propagation
```cpp
Result<FinalResult> complex_operation(const Input& input) {
    auto step1_result = perform_step1(input);
    if (step1_result.is_failure()) {
        return fail<FinalResult>(step1_result.get_error());
    }
    
    auto step2_result = perform_step2(step1_result.get_value());
    if (step2_result.is_failure()) {
        return fail<FinalResult>(step2_result.get_error());
    }
    
    return ok(step2_result.get_value());
}
```

### Batch Operations with MultiResult
```cpp
MultiResult<BatchResults> process_batch(const std::vector<Input>& inputs) {
    ErrorList accumulated_errors = ErrorList::empty();
    BatchResults results;
    
    for (const auto& input : inputs) {
        auto result = process_single(input);
        if (result.is_failure()) {
            accumulated_errors = accumulated_errors.add(result.get_error());
        } else {
            results.add(result.get_value());
        }
    }
    
    if (accumulated_errors.has_errors()) {
        return multi_fail<BatchResults>(accumulated_errors);
    }
    
    return multi_ok(std::move(results));
}
```

## Serialization

All Result types support deterministic serialization:

```cpp
auto success = ok(42);
std::string serialized = success.to_string(); // \"Success(<value>)\"

auto failure = fail<int>(ErrorCode::InvalidInput, ErrorSeverity::Error, \"Bad input\");
std::string serialized = failure.to_string(); // \"Failure(Error: InvalidInput - Bad input)\"
```

## Comparison and Ordering

Result types support equality comparison for deterministic behavior:

```cpp
auto result1 = ok(42);
auto result2 = ok(42);
assert(result1 == result2); // Same success values are equal

auto failure1 = fail<int>(ErrorCode::InvalidInput, ErrorSeverity::Error, \"Message\");
auto failure2 = fail<int>(ErrorCode::InvalidInput, ErrorSeverity::Error, \"Message\");
assert(failure1 == failure2); // Same errors produce equal failures
```

## Integration with NX-Core Systems

### With Identity System
```cpp
Result<JobID> create_job(const JobSpec& spec) {
    if (!spec.is_valid()) {
        return fail<JobID>(ErrorCode::InvalidInput, ErrorSeverity::Error, \"Invalid job spec\");
    }
    
    JobID id = JobID::generate(spec.to_string());
    return ok(id);
}
```

### With Logical Clock
```cpp
Result<LogicalTime> advance_clock(LogicalClock& clock) {
    auto time = clock.advance();
    return ok(time);
}
```

## Best Practices

### Do
- Always use explicit factory functions (`ok()`, `fail()`)
- Check result state before accessing values or errors
- Propagate errors through the call stack
- Use MultiResult for operations that can accumulate multiple errors
- Serialize results for audit trails and debugging

### Don't
- Use exceptions for normal control flow when Result is available
- Ignore failure states
- Access values without checking success state
- Access errors without checking failure state
- Use Result for types that can't be stored in unions (references, etc.)

## Thread Safety

Result types are immutable once created and safe for concurrent read access. However, they are not thread-safe for concurrent modification (which is impossible due to immutability).

## Performance Considerations

- Result types use unions for efficient storage
- No dynamic allocation for the Result itself
- Move semantics supported for efficient value transfer
- Template-based design enables compiler optimizations