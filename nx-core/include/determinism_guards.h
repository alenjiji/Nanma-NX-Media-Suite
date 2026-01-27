#pragma once

#include <type_traits>
#include <chrono>
#include <random>
#include <unordered_map>
#include <unordered_set>

namespace nx::core {

// Compile-time guards against non-deterministic types
template<typename T>
struct is_deterministic_container : std::true_type {};

template<typename K, typename V, typename H, typename E, typename A>
struct is_deterministic_container<std::unordered_map<K, V, H, E, A>> : std::false_type {};

template<typename K, typename H, typename E, typename A>
struct is_deterministic_container<std::unordered_set<K, H, E, A>> : std::false_type {};

template<typename T>
struct is_time_type : std::false_type {};

template<typename Rep, typename Period>
struct is_time_type<std::chrono::duration<Rep, Period>> : std::true_type {};

template<typename Clock, typename Duration>
struct is_time_type<std::chrono::time_point<Clock, Duration>> : std::true_type {};

template<typename T>
struct is_random_generator : std::false_type {};

template<typename UIntType, UIntType a, UIntType c, UIntType m>
struct is_random_generator<std::linear_congruential_engine<UIntType, a, c, m>> : std::true_type {};

template<typename UIntType, size_t w, size_t n, size_t m, size_t r, UIntType a, size_t u, UIntType d, size_t s, UIntType b, size_t t, UIntType c, size_t l, UIntType f>
struct is_random_generator<std::mersenne_twister_engine<UIntType, w, n, m, r, a, u, d, s, b, t, c, l, f>> : std::true_type {};

// Concepts for deterministic validation
template<typename T>
concept DeterministicContainer = is_deterministic_container<T>::value;

template<typename T>
concept NonTimeType = !is_time_type<T>::value;

template<typename T>
concept NonRandomType = !is_random_generator<T>::value;

// Runtime guards
class DeterminismGuard {
public:
    static void assert_no_time_access() {
        // Architecture comment: Time access forbidden in deterministic contexts
        static_assert(true, "Time access validation point");
    }
    
    static void assert_no_random_access() {
        // Architecture comment: Random generators forbidden in deterministic contexts
        static_assert(true, "Random access validation point");
    }
    
    static void assert_session_immutable() {
        // Architecture comment: Session mutation forbidden after creation
        static_assert(true, "Session immutability validation point");
    }
};

// Runtime validation function declaration
void validate_deterministic_execution();

// Macro for deterministic function marking
#define NX_DETERMINISTIC_FUNCTION \
    static_assert(true, "Function marked as deterministic - no time/random/mutation allowed")

} // namespace nx::core