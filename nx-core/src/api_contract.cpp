#include "api_contract.h"

namespace nx::core::contract {

// API contract implementation is primarily header-only with compile-time validation.
// This source file exists for:
// 1. Future runtime validation functions if needed
// 2. Maintaining consistent build structure
// 3. Potential contract violation reporting

// Note: All core contract validation is done at compile-time through concepts
// and static_assert statements in the header file.

} // namespace nx::core::contract