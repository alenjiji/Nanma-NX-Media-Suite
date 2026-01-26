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
    static LogicalTime origin() noexcept {
        return LogicalTime{0};
    }
    
    // Create LogicalTime at specific tick value
    static LogicalTime at_tick(uint64_t tick) noexcept {
        return LogicalTime{tick};
    }
    
    // Advance time by specified number of ticks (returns new LogicalTime)
    LogicalTime advance(uint64_t delta_ticks) const noexcept {
        return LogicalTime{ticks + delta_ticks};
    }
    
    // Comparison operators for deterministic ordering
    bool operator==(const LogicalTime&) const noexcept = default;
    bool operator!=(const LogicalTime&) const noexcept = default;
    
    bool operator<(const LogicalTime& other) const noexcept {
        return ticks < other.ticks;
    }
    
    bool operator<=(const LogicalTime& other) const noexcept {
        return ticks <= other.ticks;
    }
    
    bool operator>(const LogicalTime& other) const noexcept {
        return ticks > other.ticks;
    }
    
    bool operator>=(const LogicalTime& other) const noexcept {
        return ticks >= other.ticks;
    }
    
    // Serialization support
    std::string to_string() const {
        return "LogicalTime(" + std::to_string(ticks) + ")";
    }
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
    static SequenceIndex first() noexcept {
        return SequenceIndex{0};
    }
    
    // Create SequenceIndex at specific position
    static SequenceIndex at_position(uint64_t position) noexcept {
        return SequenceIndex{position};
    }
    
    // Get next sequence position (returns new SequenceIndex)
    SequenceIndex next() const noexcept {
        return SequenceIndex{index + 1};
    }
    
    // Advance sequence by specified steps (returns new SequenceIndex)
    SequenceIndex advance(uint64_t steps) const noexcept {
        return SequenceIndex{index + steps};
    }
    
    // Comparison operators for deterministic ordering
    bool operator==(const SequenceIndex&) const noexcept = default;
    bool operator!=(const SequenceIndex&) const noexcept = default;
    
    bool operator<(const SequenceIndex& other) const noexcept {
        return index < other.index;
    }
    
    bool operator<=(const SequenceIndex& other) const noexcept {
        return index <= other.index;
    }
    
    bool operator>(const SequenceIndex& other) const noexcept {
        return index > other.index;
    }
    
    bool operator>=(const SequenceIndex& other) const noexcept {
        return index >= other.index;
    }
    
    // Serialization support
    std::string to_string() const {
        return "SequenceIndex(" + std::to_string(index) + ")";
    }
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
    static LogicalClock at_origin() noexcept {
        return LogicalClock{LogicalTime::origin()};
    }
    
    // Create clock at specific time
    static LogicalClock at_time(LogicalTime time) noexcept {
        return LogicalClock{time};
    }
    
    // Advance clock by one tick (returns new LogicalClock)
    LogicalClock tick() const noexcept {
        return LogicalClock{current_time.advance(1)};
    }
    
    // Advance clock by specified ticks (returns new LogicalClock)
    LogicalClock advance(uint64_t ticks) const noexcept {
        return LogicalClock{current_time.advance(ticks)};
    }
    
    // Get current logical time (immutable)
    LogicalTime now() const noexcept {
        return current_time;
    }
    
    // Comparison operators
    bool operator==(const LogicalClock&) const noexcept = default;
    bool operator!=(const LogicalClock&) const noexcept = default;
    
    bool operator<(const LogicalClock& other) const noexcept {
        return current_time < other.current_time;
    }
    
    // Serialization support
    std::string to_string() const {
        return "LogicalClock{" + current_time.to_string() + "}";
    }
};

} // namespace nx::core