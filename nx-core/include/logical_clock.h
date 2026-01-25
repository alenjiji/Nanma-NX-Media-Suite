#pragma once

#include <cstdint>
#include <string>

namespace nx::core {

/**
 * Deterministic logical clock system for Nanma NX-MediaSuite
 * 
 * Provides deterministic time and ordering that:
 * - Does NOT depend on wall-clock time or system clocks
 * - Advances only through explicit increments
 * - Is fully reproducible across runs, machines, and architectures
 * - Can be replayed exactly for audit and debugging
 * 
 * Used by BatchFlow scheduling, job execution ordering, audit timelines,
 * and monitor event correlation.
 */

/**
 * LogicalTime - Deterministic logical time value
 * 
 * Represents a point in logical time that advances only through explicit
 * increments. Time starts at 0 and advances deterministically.
 * 
 * Immutable once created - thread-safe by construction.
 */
struct LogicalTime final {
    uint64_t ticks;
    
    // Create LogicalTime at origin (tick 0)
    static LogicalTime origin() noexcept;
    
    // Create LogicalTime at specific tick value
    static LogicalTime at_tick(uint64_t tick) noexcept;
    
    // Advance time by specified number of ticks (returns new LogicalTime)
    LogicalTime advance(uint64_t delta_ticks) const noexcept;
    
    // Comparison operators for deterministic ordering
    bool operator==(const LogicalTime&) const noexcept = default;
    bool operator!=(const LogicalTime&) const noexcept = default;
    bool operator<(const LogicalTime& other) const noexcept;
    bool operator<=(const LogicalTime& other) const noexcept;
    bool operator>(const LogicalTime& other) const noexcept;
    bool operator>=(const LogicalTime& other) const noexcept;
    
    // Serialization support
    std::string to_string() const;
};

/**
 * SequenceIndex - Deterministic ordering/sequence position
 * 
 * Represents position in a deterministic sequence. Used for ordering
 * operations, events, and processing steps in a reproducible manner.
 * 
 * Immutable once created - thread-safe by construction.
 */
struct SequenceIndex final {
    uint64_t index;
    
    // Create SequenceIndex at beginning (index 0)
    static SequenceIndex first() noexcept;
    
    // Create SequenceIndex at specific position
    static SequenceIndex at_position(uint64_t position) noexcept;
    
    // Get next sequence position (returns new SequenceIndex)
    SequenceIndex next() const noexcept;
    
    // Advance sequence by specified steps (returns new SequenceIndex)
    SequenceIndex advance(uint64_t steps) const noexcept;
    
    // Comparison operators for deterministic ordering
    bool operator==(const SequenceIndex&) const noexcept = default;
    bool operator!=(const SequenceIndex&) const noexcept = default;
    bool operator<(const SequenceIndex& other) const noexcept;
    bool operator<=(const SequenceIndex& other) const noexcept;
    bool operator>(const SequenceIndex& other) const noexcept;
    bool operator>=(const SequenceIndex& other) const noexcept;
    
    // Serialization support
    std::string to_string() const;
};

/**
 * LogicalClock - Deterministic clock that advances only through explicit operations
 * 
 * Provides controlled advancement of logical time. Clock state is immutable -
 * advancement operations return new clock instances.
 * 
 * No global state, no implicit advancement, fully deterministic.
 */
struct LogicalClock final {
    LogicalTime current_time;
    
    // Create clock at origin (time 0)
    static LogicalClock at_origin() noexcept;
    
    // Create clock at specific time
    static LogicalClock at_time(LogicalTime time) noexcept;
    
    // Advance clock by one tick (returns new LogicalClock)
    LogicalClock tick() const noexcept;
    
    // Advance clock by specified ticks (returns new LogicalClock)
    LogicalClock advance(uint64_t ticks) const noexcept;
    
    // Get current logical time (immutable)
    LogicalTime now() const noexcept;
    
    // Comparison operators
    bool operator==(const LogicalClock&) const noexcept = default;
    bool operator!=(const LogicalClock&) const noexcept = default;
    bool operator<(const LogicalClock& other) const noexcept;
    
    // Serialization support
    std::string to_string() const;
};

} // namespace nx::core