#include "../include/logical_clock.h"
#include <cassert>
#include <iostream>

using namespace nx::core;

void test_logical_time_determinism() {
    std::cout << "Testing LogicalTime determinism...\n";
    
    // Same operations produce identical results
    LogicalTime t1 = LogicalTime::origin();
    LogicalTime t2 = LogicalTime::origin();
    assert(t1 == t2);
    assert(t1.to_string() == t2.to_string());
    
    LogicalTime t3 = t1.advance(100);
    LogicalTime t4 = t2.advance(100);
    assert(t3 == t4);
    assert(t3.to_string() == t4.to_string());
    
    std::cout << "✓ LogicalTime operations are deterministic\n";
}

void test_logical_time_ordering() {
    std::cout << "Testing LogicalTime ordering...\n";
    
    LogicalTime t0 = LogicalTime::origin();
    LogicalTime t1 = t0.advance(1);
    LogicalTime t2 = t1.advance(1);
    
    // Verify ordering
    assert(t0 < t1);
    assert(t1 < t2);
    assert(t0 < t2); // Transitivity
    
    assert(t2 > t1);
    assert(t1 > t0);
    assert(t2 > t0);
    
    assert(t0 <= t1);
    assert(t1 >= t0);
    assert(t0 != t1);
    
    std::cout << "✓ LogicalTime ordering is consistent\n";
}

void test_logical_time_immutability() {
    std::cout << "Testing LogicalTime immutability...\n";
    
    LogicalTime original = LogicalTime::at_tick(42);
    LogicalTime advanced = original.advance(10);
    
    // Original unchanged
    assert(original.ticks == 42);
    assert(advanced.ticks == 52);
    assert(original != advanced);
    
    std::cout << "✓ LogicalTime is immutable\n";
}

void test_sequence_index_determinism() {
    std::cout << "Testing SequenceIndex determinism...\n";
    
    // Same operations produce identical results
    SequenceIndex s1 = SequenceIndex::first();
    SequenceIndex s2 = SequenceIndex::first();
    assert(s1 == s2);
    
    SequenceIndex s3 = s1.advance(50);
    SequenceIndex s4 = s2.advance(50);
    assert(s3 == s4);
    
    std::cout << "✓ SequenceIndex operations are deterministic\n";
}

void test_sequence_index_ordering() {
    std::cout << "Testing SequenceIndex ordering...\n";
    
    SequenceIndex s0 = SequenceIndex::first();
    SequenceIndex s1 = s0.next();
    SequenceIndex s2 = s1.next();
    
    // Verify ordering
    assert(s0 < s1);
    assert(s1 < s2);
    assert(s0 < s2); // Transitivity
    
    assert(s2 > s1);
    assert(s1 > s0);
    
    std::cout << "✓ SequenceIndex ordering is consistent\n";
}

void test_logical_clock_determinism() {
    std::cout << "Testing LogicalClock determinism...\n";
    
    // Same operations produce identical results
    LogicalClock c1 = LogicalClock::at_origin();
    LogicalClock c2 = LogicalClock::at_origin();
    assert(c1 == c2);
    assert(c1.now() == c2.now());
    
    LogicalClock c3 = c1.advance(25);
    LogicalClock c4 = c2.advance(25);
    assert(c3 == c4);
    assert(c3.now() == c4.now());
    
    std::cout << "✓ LogicalClock operations are deterministic\n";
}

void test_logical_clock_explicit_advancement() {
    std::cout << "Testing LogicalClock explicit advancement...\n";
    
    LogicalClock clock = LogicalClock::at_origin();
    LogicalTime initial_time = clock.now();
    
    // Clock doesn't advance implicitly
    LogicalTime still_same = clock.now();
    assert(initial_time == still_same);
    
    // Clock advances only when explicitly requested
    LogicalClock advanced_clock = clock.tick();
    LogicalTime advanced_time = advanced_clock.now();
    
    // Original clock unchanged
    assert(clock.now() == initial_time);
    // New clock has advanced
    assert(advanced_time > initial_time);
    assert(advanced_time.ticks == initial_time.ticks + 1);
    
    std::cout << "✓ LogicalClock advances only explicitly\n";
}

void test_reproducibility() {
    std::cout << "Testing reproducibility across runs...\n";
    
    // Simulate identical sequence of operations
    auto run_sequence = []() {
        LogicalClock clock = LogicalClock::at_origin();
        clock = clock.advance(10);
        clock = clock.tick();
        clock = clock.advance(5);
        return clock.now();
    };
    
    LogicalTime result1 = run_sequence();
    LogicalTime result2 = run_sequence();
    
    // Results must be identical
    assert(result1 == result2);
    assert(result1.ticks == 16); // 0 + 10 + 1 + 5
    
    std::cout << "✓ Operations are reproducible across runs\n";
}

void test_serialization() {
    std::cout << "Testing serialization...\n";
    
    LogicalTime time = LogicalTime::at_tick(12345);
    SequenceIndex seq = SequenceIndex::at_position(67890);
    LogicalClock clock = LogicalClock::at_time(time);
    
    std::string time_str = time.to_string();
    std::string seq_str = seq.to_string();
    std::string clock_str = clock.to_string();
    
    // Verify serialization format
    assert(time_str.find("12345") != std::string::npos);
    assert(seq_str.find("67890") != std::string::npos);
    assert(clock_str.find("12345") != std::string::npos);
    
    std::cout << "✓ Serialization works correctly\n";
}

int main() {
    std::cout << "Running NX-Core Logical Clock Tests\n";
    std::cout << "===================================\n";
    
    test_logical_time_determinism();
    test_logical_time_ordering();
    test_logical_time_immutability();
    test_sequence_index_determinism();
    test_sequence_index_ordering();
    test_logical_clock_determinism();
    test_logical_clock_explicit_advancement();
    test_reproducibility();
    test_serialization();
    
    std::cout << "\n✓ All tests passed!\n";
    std::cout << "Deterministic logical clock system validated.\n";
    
    return 0;
}