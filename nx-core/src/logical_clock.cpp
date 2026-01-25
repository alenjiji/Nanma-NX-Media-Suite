#include "logical_clock.h"
#include <sstream>

namespace nx::core {

// LogicalTime implementation
LogicalTime LogicalTime::origin() noexcept {
    return LogicalTime{0};
}

LogicalTime LogicalTime::at_tick(uint64_t tick) noexcept {
    return LogicalTime{tick};
}

LogicalTime LogicalTime::advance(uint64_t delta_ticks) const noexcept {
    return LogicalTime{ticks + delta_ticks};
}

bool LogicalTime::operator<(const LogicalTime& other) const noexcept {
    return ticks < other.ticks;
}

bool LogicalTime::operator<=(const LogicalTime& other) const noexcept {
    return ticks <= other.ticks;
}

bool LogicalTime::operator>(const LogicalTime& other) const noexcept {
    return ticks > other.ticks;
}

bool LogicalTime::operator>=(const LogicalTime& other) const noexcept {
    return ticks >= other.ticks;
}

std::string LogicalTime::to_string() const {
    return "LogicalTime(" + std::to_string(ticks) + ")";
}

// SequenceIndex implementation
SequenceIndex SequenceIndex::first() noexcept {
    return SequenceIndex{0};
}

SequenceIndex SequenceIndex::at_position(uint64_t position) noexcept {
    return SequenceIndex{position};
}

SequenceIndex SequenceIndex::next() const noexcept {
    return SequenceIndex{index + 1};
}

SequenceIndex SequenceIndex::advance(uint64_t steps) const noexcept {
    return SequenceIndex{index + steps};
}

bool SequenceIndex::operator<(const SequenceIndex& other) const noexcept {
    return index < other.index;
}

bool SequenceIndex::operator<=(const SequenceIndex& other) const noexcept {
    return index <= other.index;
}

bool SequenceIndex::operator>(const SequenceIndex& other) const noexcept {
    return index > other.index;
}

bool SequenceIndex::operator>=(const SequenceIndex& other) const noexcept {
    return index >= other.index;
}

std::string SequenceIndex::to_string() const {
    return "SequenceIndex(" + std::to_string(index) + ")";
}

// LogicalClock implementation
LogicalClock LogicalClock::at_origin() noexcept {
    return LogicalClock{LogicalTime::origin()};
}

LogicalClock LogicalClock::at_time(LogicalTime time) noexcept {
    return LogicalClock{time};
}

LogicalClock LogicalClock::tick() const noexcept {
    return LogicalClock{current_time.advance(1)};
}

LogicalClock LogicalClock::advance(uint64_t ticks) const noexcept {
    return LogicalClock{current_time.advance(ticks)};
}

LogicalTime LogicalClock::now() const noexcept {
    return current_time;
}

bool LogicalClock::operator<(const LogicalClock& other) const noexcept {
    return current_time < other.current_time;
}

std::string LogicalClock::to_string() const {
    return "LogicalClock(" + current_time.to_string() + ")";
}

} // namespace nx::core