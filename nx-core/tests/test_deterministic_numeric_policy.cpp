#include "deterministic_numeric_policy.h"
#include <cassert>
#include <vector>
#include <cmath>
#include <cfenv>

using namespace nx::core;

void test_floating_point_environment_control() {
    FloatingPointEnvironment fp_env;
    NumericExecutionContext context(fp_env);
    
    // Test FP environment locking
    bool locked = context.lock_fp_environment();
    assert(locked);
    
    // Verify environment is locked
    assert(context.validate_fp_environment());
    
    // Verify rounding mode is set correctly
    assert(std::fegetround() == FE_TONEAREST);
    
    // Test environment configuration
    auto current_env = context.get_fp_environment();
    assert(current_env.rounding_mode == FE_TONEAREST);
    assert(current_env.fast_math_disabled);
    assert(current_env.fma_controlled);
    assert(current_env.extended_precision_blocked);
}

void test_deterministic_rng() {
    // Test LinearCongruentialRNG determinism
    LinearCongruentialRNG rng1(12345);
    LinearCongruentialRNG rng2(12345);
    
    // Same seed should produce identical sequences
    std::vector<uint64_t> sequence1, sequence2;
    for (int i = 0; i < 100; ++i) {
        sequence1.push_back(rng1.next());
        sequence2.push_back(rng2.next());
    }
    
    assert(sequence1 == sequence2);
    
    // Different seeds should produce different sequences
    LinearCongruentialRNG rng3(54321);
    std::vector<uint64_t> sequence3;
    for (int i = 0; i < 100; ++i) {
        sequence3.push_back(rng3.next());
    }
    
    assert(sequence1 != sequence3);
    
    // Test RNG properties
    assert(rng1.get_algorithm_name() == "LinearCongruential_PCG");
    assert(!rng1.get_algorithm_name().empty());
}

void test_rng_audit_trail() {
    NumericExecutionContext context;
    
    // Create RNG instances and verify audit trail
    auto rng1 = context.create_job_rng(1001);
    auto rng2 = context.create_job_rng(1002);
    
    auto audit_trail = context.get_rng_audit_trail();
    assert(audit_trail.size() == 2);
    
    assert(audit_trail[0].seed == 1001);
    assert(audit_trail[1].seed == 1002);
    assert(audit_trail[0].algorithm == "LinearCongruential_PCG");
    assert(audit_trail[1].algorithm == "LinearCongruential_PCG");
}

void test_numeric_result_validation() {
    NumericExecutionContext context;
    
    // Test valid numeric values
    assert(context.validate_numeric_result(1.0f));
    assert(context.validate_numeric_result(0.0));
    assert(context.validate_numeric_result(-1.5));
    assert(context.validate_numeric_result(42));
    
    // Test invalid numeric values
    float nan_value = std::numeric_limits<float>::quiet_NaN();
    float inf_value = std::numeric_limits<float>::infinity();
    
    assert(!context.validate_numeric_result(nan_value));
    assert(!context.validate_numeric_result(inf_value));
    assert(!context.validate_numeric_result(-inf_value));
}

void test_deterministic_reduction() {
    std::vector<double> values = {1.1, 2.2, 3.3, 4.4, 5.5};
    
    // Test serial sum
    double sum1 = DeterministicReduction::serial_sum(values);
    double sum2 = DeterministicReduction::serial_sum(values);
    assert(sum1 == sum2);  // Should be bit-identical
    
    // Test tree reduction with addition
    auto add_func = [](double a, double b) { return a + b; };
    double tree_sum1 = DeterministicReduction::tree_reduce(values, add_func);
    double tree_sum2 = DeterministicReduction::tree_reduce(values, add_func);
    assert(tree_sum1 == tree_sum2);  // Should be bit-identical
    
    // Test with different sizes
    std::vector<double> small_values = {1.0, 2.0};
    double small_sum = DeterministicReduction::serial_sum(small_values);
    assert(small_sum == 3.0);
    
    std::vector<double> empty_values;
    double empty_sum = DeterministicReduction::serial_sum(empty_values);
    assert(empty_sum == 0.0);
}

void test_numeric_guards() {
    NumericExecutionContext context;
    context.lock_fp_environment();
    
    // Test FP environment guard
    NumericGuards::assert_fp_environment_locked(context);
    
    // Test NaN guard
    try {
        NumericGuards::assert_no_nan(1.0f);  // Should not throw
    } catch (...) {
        assert(false);  // Should not reach here
    }
    
    try {
        float nan_val = std::numeric_limits<float>::quiet_NaN();
        NumericGuards::assert_no_nan(nan_val);  // Should throw
        assert(false);  // Should not reach here
    } catch (const std::runtime_error&) {
        // Expected
    }
    
    // Test infinity guard
    try {
        NumericGuards::assert_no_infinity(1.0f);  // Should not throw
    } catch (...) {
        assert(false);  // Should not reach here
    }
    
    try {
        float inf_val = std::numeric_limits<float>::infinity();
        NumericGuards::assert_no_infinity(inf_val);  // Should throw
        assert(false);  // Should not reach here
    } catch (const std::runtime_error&) {
        // Expected
    }
    
    // Test RNG guard
    LinearCongruentialRNG rng(12345);
    NumericGuards::assert_deterministic_rng(rng);  // Should not throw
}

void test_cross_run_determinism() {
    // Test that multiple runs produce identical results
    std::vector<std::vector<uint64_t>> run_results;
    
    for (int run = 0; run < 3; ++run) {
        NumericExecutionContext context;
        context.lock_fp_environment();
        
        auto rng = context.create_job_rng(9999);
        
        std::vector<uint64_t> results;
        for (int i = 0; i < 50; ++i) {
            results.push_back(rng->next());
        }
        
        run_results.push_back(results);
    }
    
    // All runs should produce identical results
    for (size_t i = 1; i < run_results.size(); ++i) {
        assert(run_results[0] == run_results[i]);
    }
}

void test_fp_environment_isolation() {
    // Test that FP environment changes are contained
    NumericExecutionContext context1;
    NumericExecutionContext context2;
    
    // Lock first context
    assert(context1.lock_fp_environment());
    assert(context1.validate_fp_environment());
    
    // Second context should be able to lock independently
    assert(context2.lock_fp_environment());
    assert(context2.validate_fp_environment());
    
    // Both should remain valid
    assert(context1.validate_fp_environment());
    assert(context2.validate_fp_environment());
}

void test_rng_independence() {
    NumericExecutionContext context;
    
    // Create independent RNG streams
    auto rng1 = context.create_job_rng(1111);
    auto rng2 = context.create_job_rng(2222);
    
    // Generate sequences from both
    std::vector<uint64_t> seq1, seq2;
    for (int i = 0; i < 20; ++i) {
        seq1.push_back(rng1->next());
        seq2.push_back(rng2->next());
    }
    
    // Sequences should be different (different seeds)
    assert(seq1 != seq2);
    
    // But each should be reproducible
    auto rng1_copy = context.create_job_rng(1111);
    auto rng2_copy = context.create_job_rng(2222);
    
    std::vector<uint64_t> seq1_copy, seq2_copy;
    for (int i = 0; i < 20; ++i) {
        seq1_copy.push_back(rng1_copy->next());
        seq2_copy.push_back(rng2_copy->next());
    }
    
    assert(seq1 == seq1_copy);
    assert(seq2 == seq2_copy);
}

int main() {
    test_floating_point_environment_control();
    test_deterministic_rng();
    test_rng_audit_trail();
    test_numeric_result_validation();
    test_deterministic_reduction();
    test_numeric_guards();
    test_cross_run_determinism();
    test_fp_environment_isolation();
    test_rng_independence();
    
    return 0;
}