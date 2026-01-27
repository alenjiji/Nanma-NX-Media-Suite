#include "determinism_guards.h"
#include <cassert>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <random>

using namespace nx::core;

void test_container_determinism_validation() {
    // Valid deterministic containers
    static_assert(DeterministicContainer<std::vector<int>>);
    static_assert(DeterministicContainer<std::map<int, int>>);
    
    // Invalid non-deterministic containers
    static_assert(!DeterministicContainer<std::unordered_map<int, int>>);
    static_assert(!DeterministicContainer<std::unordered_set<int>>);
}

void test_time_type_detection() {
    static_assert(!NonTimeType<std::chrono::seconds>);
    static_assert(!NonTimeType<std::chrono::system_clock::time_point>);
    static_assert(NonTimeType<int>);
    static_assert(NonTimeType<std::string>);
}

void test_random_type_detection() {
    static_assert(!NonRandomType<std::mt19937>);
    static_assert(!NonRandomType<std::minstd_rand>);
    static_assert(NonRandomType<int>);
    static_assert(NonRandomType<std::string>);
}

void test_deterministic_function_marking() {
    auto deterministic_func = []() {
        NX_DETERMINISTIC_FUNCTION;
        return 42;
    };
    
    assert(deterministic_func() == 42);
}

void test_runtime_guards() {
    // These should not crash - they're validation points
    DeterminismGuard::assert_no_time_access();
    DeterminismGuard::assert_no_random_access();
    DeterminismGuard::assert_session_immutable();
    
    validate_deterministic_execution();
}

int main() {
    test_container_determinism_validation();
    test_time_type_detection();
    test_random_type_detection();
    test_deterministic_function_marking();
    test_runtime_guards();
    
    return 0;
}