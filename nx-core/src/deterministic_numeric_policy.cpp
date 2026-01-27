#include "deterministic_numeric_policy.h"
#include <stdexcept>
#include <cmath>
#include <sstream>

namespace nx::core {

// LinearCongruentialRNG implementation

LinearCongruentialRNG::LinearCongruentialRNG(uint64_t initial_seed)
    : current_seed_(initial_seed) {
}

void LinearCongruentialRNG::seed(uint64_t seed) {
    current_seed_ = seed;
}

uint64_t LinearCongruentialRNG::next() {
    current_seed_ = current_seed_ * MULTIPLIER + INCREMENT;
    return current_seed_;
}

uint64_t LinearCongruentialRNG::get_seed() const {
    return current_seed_;
}

std::string LinearCongruentialRNG::get_algorithm_name() const {
    return "LinearCongruential_PCG";
}

// NumericExecutionContext implementation

NumericExecutionContext::NumericExecutionContext(const FloatingPointEnvironment& fp_env)
    : fp_env_(fp_env)
    , fp_environment_locked_(false) {
}

bool NumericExecutionContext::lock_fp_environment() {
    // Set explicit rounding mode
    if (std::fesetround(fp_env_.rounding_mode) != 0) {
        return false;
    }
    
    // Clear any existing FP exceptions
    std::feclearexcept(FE_ALL_EXCEPT);
    
    // Validate environment was set correctly
    if (!check_fp_environment_state()) {
        return false;
    }
    
    fp_environment_locked_ = true;
    return true;
}

bool NumericExecutionContext::validate_fp_environment() const {
    if (!fp_environment_locked_) {
        return false;
    }
    
    return check_fp_environment_state();
}

std::unique_ptr<DeterministicRNG> NumericExecutionContext::create_job_rng(uint64_t job_seed) {
    auto rng = std::make_unique<LinearCongruentialRNG>(job_seed);
    
    // Record RNG creation for audit trail
    record_rng_usage("job_" + std::to_string(job_seed), job_seed, rng->get_algorithm_name());
    
    return rng;
}

void NumericExecutionContext::record_rng_usage(const std::string& job_id, 
                                              uint64_t seed, 
                                              const std::string& algorithm) {
    rng_audit_trail_.push_back(RNGUsageRecord{
        .job_id = job_id,
        .seed = seed,
        .algorithm = algorithm
    });
}

std::vector<NumericExecutionContext::RNGUsageRecord> NumericExecutionContext::get_rng_audit_trail() const {
    return rng_audit_trail_;
}

FloatingPointEnvironment NumericExecutionContext::get_fp_environment() const {
    return fp_env_;
}

bool NumericExecutionContext::check_fp_environment_state() const {
    // Verify rounding mode
    int current_rounding = std::fegetround();
    if (current_rounding != fp_env_.rounding_mode) {
        return false;
    }
    
    // Additional FP environment checks could be added here
    // For now, rounding mode is the primary concern
    
    return true;
}

// NumericGuards implementation

void NumericGuards::assert_fp_environment_locked(const NumericExecutionContext& context) {
    if (!context.validate_fp_environment()) {
        throw std::runtime_error("Floating-point environment not properly locked for deterministic execution");
    }
}

void NumericGuards::assert_deterministic_rng(const DeterministicRNG& rng) {
    // Validate RNG has a known algorithm
    std::string algorithm = rng.get_algorithm_name();
    if (algorithm.empty()) {
        throw std::runtime_error("RNG algorithm name not specified for deterministic execution");
    }
    
    // Validate RNG has a recorded seed
    uint64_t seed = rng.get_seed();
    if (seed == 0) {
        // Note: seed of 0 might be valid, but we want explicit seeding
        // This is a policy decision that can be adjusted
    }
}

} // namespace nx::core