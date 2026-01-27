#pragma once

#include <cfenv>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

namespace nx::core {

/**
 * Deterministic numeric execution policy for Nanma NX-MediaSuite
 * 
 * ARCHITECTURAL MANDATE:
 * - All numeric computation must be bit-identical across runs
 * - Same inputs → same outputs regardless of hardware, OS, compiler
 * - Floating-point environment must be explicitly controlled
 * - Random number generation must be deterministic and auditable
 * 
 * SCOPE:
 * - Applies to ALL runtime execution paths
 * - NX-BatchFlow, NX-Convert Pro, NX-AudioLab, NX-VideoTrans, NX-MetaFix
 * - Foundational policy - not component-specific
 * 
 * DETERMINISM GUARANTEE:
 * If numeric behavior is not deterministic, the entire execution engine is invalid
 */

/**
 * Floating-point environment configuration
 * 
 * MANDATORY CONTROL:
 * - Explicit rounding mode setting
 * - Fast-math optimizations disabled
 * - Extended precision leakage prevention
 * - Fused multiply-add control
 */
struct FloatingPointEnvironment {
    int rounding_mode = FE_TONEAREST;       // OWNED: Explicit rounding mode
    bool fast_math_disabled = true;         // OWNED: Fast-math optimization control
    bool fma_controlled = true;             // OWNED: Fused multiply-add control
    bool extended_precision_blocked = true; // OWNED: x87 80-bit precision prevention
    
    bool operator==(const FloatingPointEnvironment& other) const = default;
};

/**
 * Deterministic random number generator interface
 * 
 * DETERMINISM REQUIREMENTS:
 * - Explicit algorithm implementation
 * - Explicit seed acceptance and recording
 * - Same seed → identical output sequence
 * - No shared mutable global state
 * - Auditable seed usage
 */
class DeterministicRNG {
public:
    virtual ~DeterministicRNG() = default;
    
    /**
     * Initialize RNG with explicit seed
     * 
     * @param seed Deterministic seed value
     */
    virtual void seed(uint64_t seed) = 0;
    
    /**
     * Generate next random value
     * 
     * DETERMINISM GUARANTEE:
     * - Same seed sequence → identical output sequence
     * - No timing or thread dependencies
     * - Hardware-independent results
     * 
     * @return Next random value in sequence
     */
    virtual uint64_t next() = 0;
    
    /**
     * Get current seed for auditability
     * 
     * @return Current seed value
     */
    virtual uint64_t get_seed() const = 0;
    
    /**
     * Get RNG algorithm name for traceability
     * 
     * @return Algorithm identifier
     */
    virtual std::string get_algorithm_name() const = 0;
};

/**
 * Linear Congruential Generator implementation
 * 
 * DETERMINISTIC PROPERTIES:
 * - Well-defined algorithm with explicit constants
 * - Identical behavior across platforms
 * - Fast and simple for deterministic needs
 * - Not cryptographically secure (not required for media processing)
 */
class LinearCongruentialRNG : public DeterministicRNG {
public:
    explicit LinearCongruentialRNG(uint64_t initial_seed = 1);
    
    void seed(uint64_t seed) override;
    uint64_t next() override;
    uint64_t get_seed() const override;
    std::string get_algorithm_name() const override;

private:
    uint64_t current_seed_;
    static constexpr uint64_t MULTIPLIER = 6364136223846793005ULL;
    static constexpr uint64_t INCREMENT = 1442695040888963407ULL;
};

/**
 * Numeric execution context for deterministic computation
 * 
 * CONTEXT OWNERSHIP:
 * - Owns floating-point environment state
 * - Owns RNG instances for job isolation
 * - Provides numeric validation utilities
 * - Enforces deterministic computation rules
 */
class NumericExecutionContext {
public:
    /**
     * Create numeric context with controlled environment
     * 
     * @param fp_env Floating-point environment configuration
     */
    explicit NumericExecutionContext(const FloatingPointEnvironment& fp_env = FloatingPointEnvironment{});
    
    /**
     * Lock floating-point environment for deterministic execution
     * 
     * MANDATORY SETUP:
     * - Sets explicit rounding mode
     * - Disables fast-math optimizations
     * - Prevents extended precision leakage
     * - Must be called before any numeric computation
     * 
     * @return true if environment locked successfully
     */
    bool lock_fp_environment();
    
    /**
     * Validate floating-point environment hasn't changed
     * 
     * @return true if environment matches locked state
     */
    bool validate_fp_environment() const;
    
    /**
     * Create deterministic RNG for job
     * 
     * ISOLATION GUARANTEE:
     * - Each job gets independent RNG stream
     * - No shared mutable state between jobs
     * - Seed is recorded for auditability
     * 
     * @param job_seed Deterministic seed for this job
     * @return Unique RNG instance for job
     */
    std::unique_ptr<DeterministicRNG> create_job_rng(uint64_t job_seed);
    
    /**
     * Record RNG usage for audit trail
     * 
     * @param job_id Job identifier
     * @param seed Seed used for job
     * @param algorithm Algorithm name
     */
    void record_rng_usage(const std::string& job_id, uint64_t seed, const std::string& algorithm);
    
    /**
     * Get RNG usage audit trail
     * 
     * @return Vector of RNG usage records
     */
    struct RNGUsageRecord {
        std::string job_id;         // OWNED: Job that used RNG
        uint64_t seed;              // OWNED: Seed value used
        std::string algorithm;      // OWNED: RNG algorithm name
        
        bool operator==(const RNGUsageRecord& other) const = default;
    };
    
    std::vector<RNGUsageRecord> get_rng_audit_trail() const;
    
    /**
     * Validate numeric computation result
     * 
     * VALIDATION RULES:
     * - No NaN propagation without explicit handling
     * - No infinity without explicit handling
     * - No undefined behavior reliance
     * 
     * @param value Numeric result to validate
     * @return true if value is valid for deterministic execution
     */
    template<typename T>
    bool validate_numeric_result(T value) const;
    
    /**
     * Get current floating-point environment state
     * 
     * @return Current FP environment configuration
     */
    FloatingPointEnvironment get_fp_environment() const;

private:
    FloatingPointEnvironment fp_env_;                   // OWNED: FP environment configuration
    bool fp_environment_locked_;                        // OWNED: Lock state tracking
    std::vector<RNGUsageRecord> rng_audit_trail_;       // OWNED: RNG usage audit trail
    
    // Validate FP environment matches expected state
    bool check_fp_environment_state() const;
};

/**
 * Deterministic reduction utilities
 * 
 * REDUCTION DETERMINISM:
 * - Fixed iteration order for all reductions
 * - No order-dependent parallel reductions
 * - Explicit accumulation order
 * - No associativity assumptions
 */
class DeterministicReduction {
public:
    /**
     * Serial sum with deterministic order
     * 
     * @param values Vector of values to sum
     * @return Sum computed in deterministic order
     */
    template<typename T>
    static T serial_sum(const std::vector<T>& values);
    
    /**
     * Tree reduction with fixed topology
     * 
     * DETERMINISM GUARANTEE:
     * - Fixed tree structure regardless of input size
     * - Same reduction order across runs
     * - Parallel-safe with deterministic combining
     * 
     * @param values Vector of values to reduce
     * @param combine_func Combining function
     * @return Reduced value
     */
    template<typename T, typename CombineFunc>
    static T tree_reduce(const std::vector<T>& values, CombineFunc combine_func);
};

/**
 * Numeric guard utilities for runtime validation
 * 
 * GUARD RESPONSIBILITIES:
 * - Detect numeric environment violations
 * - Validate deterministic computation rules
 * - Provide assertion points for numeric invariants
 */
class NumericGuards {
public:
    /**
     * Assert floating-point environment is locked
     * 
     * @param context Numeric execution context to validate
     */
    static void assert_fp_environment_locked(const NumericExecutionContext& context);
    
    /**
     * Assert no NaN values in computation
     * 
     * @param value Value to check for NaN
     */
    template<typename T>
    static void assert_no_nan(T value);
    
    /**
     * Assert no infinity values in computation
     * 
     * @param value Value to check for infinity
     */
    template<typename T>
    static void assert_no_infinity(T value);
    
    /**
     * Assert deterministic RNG usage
     * 
     * @param rng RNG instance to validate
     */
    static void assert_deterministic_rng(const DeterministicRNG& rng);
};

// Template implementations

template<typename T>
bool NumericExecutionContext::validate_numeric_result(T value) const {
    if constexpr (std::is_floating_point_v<T>) {
        // Check for NaN
        if (std::isnan(value)) {
            return false;  // NaN must be handled explicitly
        }
        
        // Check for infinity
        if (std::isinf(value)) {
            return false;  // Infinity must be handled explicitly
        }
    }
    
    return true;
}

template<typename T>
T DeterministicReduction::serial_sum(const std::vector<T>& values) {
    T sum = T{};
    for (const auto& value : values) {
        sum += value;
    }
    return sum;
}

template<typename T, typename CombineFunc>
T DeterministicReduction::tree_reduce(const std::vector<T>& values, CombineFunc combine_func) {
    if (values.empty()) {
        return T{};
    }
    
    if (values.size() == 1) {
        return values[0];
    }
    
    // Create working copy for tree reduction
    std::vector<T> working_values = values;
    
    // Reduce in fixed tree topology
    while (working_values.size() > 1) {
        std::vector<T> next_level;
        next_level.reserve((working_values.size() + 1) / 2);
        
        for (size_t i = 0; i < working_values.size(); i += 2) {
            if (i + 1 < working_values.size()) {
                next_level.push_back(combine_func(working_values[i], working_values[i + 1]));
            } else {
                next_level.push_back(working_values[i]);
            }
        }
        
        working_values = std::move(next_level);
    }
    
    return working_values[0];
}

template<typename T>
void NumericGuards::assert_no_nan(T value) {
    if constexpr (std::is_floating_point_v<T>) {
        if (std::isnan(value)) {
            throw std::runtime_error("NaN detected in deterministic computation");
        }
    }
}

template<typename T>
void NumericGuards::assert_no_infinity(T value) {
    if constexpr (std::is_floating_point_v<T>) {
        if (std::isinf(value)) {
            throw std::runtime_error("Infinity detected in deterministic computation");
        }
    }
}

} // namespace nx::core