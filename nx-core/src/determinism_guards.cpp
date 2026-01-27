#include "determinism_guards.h"
#include <cassert>
#include <map>

namespace nx::core {

// Runtime validation functions
void validate_deterministic_execution() {
    DeterminismGuard::assert_no_time_access();
    DeterminismGuard::assert_no_random_access();
    DeterminismGuard::assert_session_immutable();
}

// Template instantiation guards
template<typename Container>
void validate_container_determinism() {
    static_assert(DeterministicContainer<Container>, 
                  "Non-deterministic containers forbidden");
}

// Explicit instantiations for common containers
template void validate_container_determinism<std::vector<int>>();
template void validate_container_determinism<std::map<int, int>>();

} // namespace nx::core