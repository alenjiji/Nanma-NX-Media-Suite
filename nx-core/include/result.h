#pragma once

#include "error_system.h"
#include <type_traits>
#include <utility>

namespace nx::core {

/**
 * Deterministic Result type for Nanma NX-MediaSuite
 * 
 * Provides explicit success/failure representation for all NX-Core APIs:
 * - Value-based result type (no inheritance, no polymorphism)
 * - Explicit success/failure states (mutually exclusive)
 * - Integrates with Error/ErrorList system
 * - Deterministic behavior (same inputs → same result)
 * - Immutable once created
 * - Serializable with stable representation
 * 
 * Used by all processing engines, BatchFlow execution, API boundaries,
 * CI validation, and Monitor status reporting.
 */

/**
 * Result<T> - Explicit success/failure container
 * 
 * Represents either:
 * - Success: contains value of type T
 * - Failure: contains Error describing the failure
 * 
 * Success and failure are mutually exclusive.
 * No implicit conversions or truthiness operators.
 */
template<typename T>
struct Result final {
    static_assert(!std::is_reference_v<T>, "Result cannot hold reference types");
    static_assert(!std::is_void_v<T>, "Use VoidResult for void operations");
    
private:
    bool is_success_state;
    union {
        T value;
        Error error;
    };
    
public:
    // Construct successful result
    explicit Result(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>)
        : is_success_state(true), value(std::move(val)) {}
    
    explicit Result(const T& val) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : is_success_state(true), value(val) {}
    
    // Construct failed result
    explicit Result(Error err) noexcept
        : is_success_state(false), error(std::move(err)) {}
    
    // Copy constructor
    Result(const Result& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : is_success_state(other.is_success_state) {
        if (is_success_state) {
            new(&value) T(other.value);
        } else {
            new(&error) Error(other.error);
        }
    }
    
    // Move constructor
    Result(Result&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : is_success_state(other.is_success_state) {
        if (is_success_state) {
            new(&value) T(std::move(other.value));
        } else {
            new(&error) Error(std::move(other.error));
        }
    }
    
    // Copy assignment
    Result& operator=(const Result& other) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>) {
        if (this != &other) {
            this->~Result();
            new(this) Result(other);
        }
        return *this;
    }
    
    // Move assignment
    Result& operator=(Result&& other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>) {
        if (this != &other) {
            this->~Result();
            new(this) Result(std::move(other));
        }
        return *this;
    }
    
    // Destructor
    ~Result() noexcept {
        if (is_success_state) {
            value.~T();
        } else {
            error.~Error();
        }
    }
    
    // Explicit state checking
    bool is_success() const noexcept { return is_success_state; }
    bool is_failure() const noexcept { return !is_success_state; }
    
    // Access success value (undefined behavior if not successful)
    const T& get_value() const noexcept { return value; }
    T& get_value() noexcept { return value; }
    
    // Access failure error (undefined behavior if not failed)
    const Error& get_error() const noexcept { return error; }
    
    // Comparison operators for deterministic ordering
    bool operator==(const Result& other) const noexcept {
        if (is_success_state != other.is_success_state) return false;
        if (is_success_state) {
            return value == other.value;
        } else {
            return error == other.error;
        }
    }
    
    bool operator!=(const Result& other) const noexcept {
        return !(*this == other);
    }
    
    // Serialization support
    std::string to_string() const {
        if (is_success_state) {
            return "Success(<value>)";
        } else {
            return "Failure(" + error.to_string() + ")";
        }
    }
};

/**
 * Factory functions for explicit Result construction
 */

// Create successful result
template<typename T>
Result<std::decay_t<T>> ok(T&& value) noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T>) {
    return Result<std::decay_t<T>>(std::forward<T>(value));
}

// Create failed result
template<typename T>
Result<T> fail(Error error) noexcept {
    return Result<T>(std::move(error));
}

template<typename T>
Result<T> fail(ErrorCode code, ErrorSeverity severity, std::string_view message) {
    return Result<T>(Error::create(code, severity, message));
}

template<typename T>
Result<T> fail(ErrorCode code, ErrorSeverity severity, std::string_view message, ErrorContext context) {
    return Result<T>(Error::create(code, severity, message, std::move(context)));
}

/**
 * VoidResult - Result type for operations that don't return values
 * 
 * Represents success/failure for operations that only need to indicate
 * completion status without carrying a value.
 */
struct VoidResult final {
private:
    bool is_success_state;
    Error error; // Only valid when is_success_state is false
    
public:
    // Static factory functions
    static VoidResult ok() noexcept {
        return VoidResult{true, Error{ErrorCode::Success, ErrorSeverity::Info, "", ErrorContext::empty()}};
    }
    
    static VoidResult fail(Error err) noexcept {
        return VoidResult{false, std::move(err)};
    }
    
    static VoidResult fail(ErrorCode code, ErrorSeverity severity, std::string_view message) {
        return VoidResult{false, Error::create(code, severity, message)};
    }
    
    static VoidResult fail(ErrorCode code, ErrorSeverity severity, std::string_view message, ErrorContext context) {
        return VoidResult{false, Error::create(code, severity, message, std::move(context))};
    }
    
    // Explicit state checking
    bool is_success() const noexcept { return is_success_state; }
    bool is_failure() const noexcept { return !is_success_state; }
    
    // Access failure error (undefined behavior if not failed)
    const Error& get_error() const noexcept { return error; }
    
    // Comparison operators
    bool operator==(const VoidResult& other) const noexcept {
        if (is_success_state != other.is_success_state) return false;
        if (is_success_state) return true;
        return error == other.error;
    }
    
    bool operator!=(const VoidResult& other) const noexcept {
        return !(*this == other);
    }
    
    // Serialization support
    std::string to_string() const {
        if (is_success_state) {
            return "Success()";
        } else {
            return "Failure(" + error.to_string() + ")";
        }
    }
    
private:
    VoidResult(bool success, Error err) noexcept
        : is_success_state(success), error(std::move(err)) {}
};

/**
 * Factory functions for VoidResult
 */

// Create successful void result
inline VoidResult ok() noexcept {
    return VoidResult::ok();
}

// Create failed void result
inline VoidResult fail(Error error) noexcept {
    return VoidResult::fail(std::move(error));
}

inline VoidResult fail(ErrorCode code, ErrorSeverity severity, std::string_view message) {
    return VoidResult::fail(code, severity, message);
}

inline VoidResult fail(ErrorCode code, ErrorSeverity severity, std::string_view message, ErrorContext context) {
    return VoidResult::fail(code, severity, message, std::move(context));
}

/**
 * MultiResult<T> - Result type with aggregated error support
 * 
 * Represents either:
 * - Success: contains value of type T
 * - Failure: contains ErrorList with one or more errors
 * 
 * Used when operations may accumulate multiple errors.
 */
template<typename T>
struct MultiResult final {
    static_assert(!std::is_reference_v<T>, "MultiResult cannot hold reference types");
    static_assert(!std::is_void_v<T>, "Use VoidResult for void operations");
    
private:
    bool is_success_state;
    union {
        T value;
        ErrorList errors;
    };
    
public:
    // Construct successful result
    explicit MultiResult(T&& val) noexcept(std::is_nothrow_move_constructible_v<T>)
        : is_success_state(true), value(std::move(val)) {}
    
    explicit MultiResult(const T& val) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : is_success_state(true), value(val) {}
    
    // Construct failed result
    explicit MultiResult(ErrorList errs) noexcept
        : is_success_state(false), errors(std::move(errs)) {}
    
    // Copy constructor
    MultiResult(const MultiResult& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
        : is_success_state(other.is_success_state) {
        if (is_success_state) {
            new(&value) T(other.value);
        } else {
            new(&errors) ErrorList(other.errors);
        }
    }
    
    // Move constructor
    MultiResult(MultiResult&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : is_success_state(other.is_success_state) {
        if (is_success_state) {
            new(&value) T(std::move(other.value));
        } else {
            new(&errors) ErrorList(std::move(other.errors));
        }
    }
    
    // Copy assignment
    MultiResult& operator=(const MultiResult& other) noexcept(std::is_nothrow_copy_constructible_v<T> && std::is_nothrow_copy_assignable_v<T>) {
        if (this != &other) {
            this->~MultiResult();
            new(this) MultiResult(other);
        }
        return *this;
    }
    
    // Move assignment
    MultiResult& operator=(MultiResult&& other) noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>) {
        if (this != &other) {
            this->~MultiResult();
            new(this) MultiResult(std::move(other));
        }
        return *this;
    }
    
    // Destructor
    ~MultiResult() noexcept {
        if (is_success_state) {
            value.~T();
        } else {
            errors.~ErrorList();
        }
    }
    
    // Explicit state checking
    bool is_success() const noexcept { return is_success_state; }
    bool is_failure() const noexcept { return !is_success_state; }
    
    // Access success value (undefined behavior if not successful)
    const T& get_value() const noexcept { return value; }
    T& get_value() noexcept { return value; }
    
    // Access failure errors (undefined behavior if not failed)
    const ErrorList& get_errors() const noexcept { return errors; }
    
    // Comparison operators
    bool operator==(const MultiResult& other) const noexcept {
        if (is_success_state != other.is_success_state) return false;
        if (is_success_state) {
            return value == other.value;
        } else {
            return errors == other.errors;
        }
    }
    
    bool operator!=(const MultiResult& other) const noexcept {
        return !(*this == other);
    }
    
    // Serialization support
    std::string to_string() const {
        if (is_success_state) {
            return "Success(<value>)";
        } else {
            return "Failure(" + errors.to_string() + ")";
        }
    }
};

/**
 * Factory functions for MultiResult
 */

// Create successful multi-result
template<typename T>
MultiResult<std::decay_t<T>> multi_ok(T&& value) noexcept(std::is_nothrow_constructible_v<std::decay_t<T>, T>) {
    return MultiResult<std::decay_t<T>>(std::forward<T>(value));
}

// Create failed multi-result
template<typename T>
MultiResult<T> multi_fail(ErrorList errors) noexcept {
    return MultiResult<T>(std::move(errors));
}

template<typename T>
MultiResult<T> multi_fail(Error error) noexcept {
    return MultiResult<T>(ErrorList::single(std::move(error)));
}

} // namespace nx::core