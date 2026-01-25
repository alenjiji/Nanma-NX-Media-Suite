#include "../include/error_system.h"
#include <cassert>
#include <iostream>

using namespace nx::core;

void test_error_code_stability() {
    std::cout << "Testing ErrorCode stability...\n";
    
    // Error codes must have stable numeric values
    assert(static_cast<uint32_t>(ErrorCode::Success) == 0);
    assert(static_cast<uint32_t>(ErrorCode::UnknownError) == 1000);
    assert(static_cast<uint32_t>(ErrorCode::InvalidInput) == 1001);
    assert(static_cast<uint32_t>(ErrorCode::ResourceNotFound) == 2000);
    assert(static_cast<uint32_t>(ErrorCode::ProcessingFailed) == 3000);
    assert(static_cast<uint32_t>(ErrorCode::ValidationFailed) == 4000);
    assert(static_cast<uint32_t>(ErrorCode::SystemError) == 5000);
    
    std::cout << "✓ ErrorCode values are stable\n";
}

void test_error_context_determinism() {
    std::cout << "Testing ErrorContext determinism...\n";
    
    // Same context creation produces identical results
    ErrorContext ctx1 = ErrorContext::create("test_op", "test_location");
    ErrorContext ctx2 = ErrorContext::create("test_op", "test_location");
    assert(ctx1 == ctx2);
    assert(ctx1.to_string() == ctx2.to_string());
    
    // Parameter addition is deterministic
    ErrorContext ctx3 = ctx1.with_parameter("key1", "value1");
    ErrorContext ctx4 = ctx2.with_parameter("key1", "value1");
    assert(ctx3 == ctx4);
    assert(ctx3.to_string() == ctx4.to_string());
    
    std::cout << "✓ ErrorContext operations are deterministic\n";
}

void test_error_context_ordering() {
    std::cout << "Testing ErrorContext ordering...\n";
    
    ErrorContext ctx1 = ErrorContext::create("a", "location");
    ErrorContext ctx2 = ErrorContext::create("b", "location");
    ErrorContext ctx3 = ErrorContext::create("a", "z_location");
    
    // Verify ordering by operation first, then location
    assert(ctx1 < ctx2);  // "a" < "b"
    assert(ctx1 < ctx3);  // same operation "a", "location" < "z_location"
    assert(ctx3 < ctx2);  // "a" < "b" (operation takes precedence)
    
    std::cout << "✓ ErrorContext ordering is consistent\n";
}

void test_error_determinism() {
    std::cout << "Testing Error determinism...\n";
    
    // Same error creation produces identical results
    Error err1 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test message");
    Error err2 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test message");
    assert(err1 == err2);
    assert(err1.to_string() == err2.to_string());
    
    // With context
    ErrorContext ctx = ErrorContext::create("test", "location");
    Error err3 = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Fatal, "Failed", ctx);
    Error err4 = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Fatal, "Failed", ctx);
    assert(err3 == err4);
    assert(err3.to_string() == err4.to_string());
    
    std::cout << "✓ Error operations are deterministic\n";
}

void test_error_ordering() {
    std::cout << "Testing Error ordering...\n";
    
    Error err1 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Warning, "Message A");
    Error err2 = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Warning, "Message A");
    Error err3 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Message A");
    Error err4 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Warning, "Message B");
    
    // Ordering: code first, then severity, then message, then context
    assert(err1 < err2);  // InvalidInput(1001) < ProcessingFailed(3000)
    assert(err1 < err3);  // Same code, Warning(1) < Error(2)
    assert(err1 < err4);  // Same code/severity, "Message A" < "Message B"
    
    std::cout << "✓ Error ordering is consistent\n";
}

void test_error_success_failure() {
    std::cout << "Testing Error success/failure detection...\n";
    
    Error success = Error::create(ErrorCode::Success, ErrorSeverity::Info, "OK");
    Error failure = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Failed");
    
    assert(success.is_success());
    assert(!success.is_failure());
    assert(!failure.is_success());
    assert(failure.is_failure());
    
    // Test success() function
    assert(nx::core::success().is_success());
    assert(!nx::core::success().is_failure());
    
    std::cout << "✓ Error success/failure detection works correctly\n";
}

void test_error_list_determinism() {
    std::cout << "Testing ErrorList determinism...\n";
    
    Error err1 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Warning, "Error 1");
    Error err2 = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Error, "Error 2");
    
    // Same operations produce identical results
    ErrorList list1 = ErrorList::empty().add(err1).add(err2);
    ErrorList list2 = ErrorList::empty().add(err1).add(err2);
    assert(list1 == list2);
    assert(list1.to_string() == list2.to_string());
    
    // Order of addition doesn't matter (deterministic sorting)
    ErrorList list3 = ErrorList::empty().add(err2).add(err1);
    assert(list1 == list3);  // Should be equal due to sorting
    
    std::cout << "✓ ErrorList operations are deterministic\n";
}

void test_error_list_aggregation() {
    std::cout << "Testing ErrorList aggregation...\n";
    
    Error err1 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Warning, "Warning");
    Error err2 = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Error, "Error");
    Error err3 = Error::create(ErrorCode::SystemError, ErrorSeverity::Fatal, "Fatal");
    
    ErrorList list1 = ErrorList::single(err1).add(err2);
    ErrorList list2 = ErrorList::single(err3);
    ErrorList combined = list1.combine(list2);
    
    assert(combined.count() == 3);
    assert(combined.has_errors());
    assert(!combined.is_empty());
    
    // Test severity checking
    assert(combined.has_severity(ErrorSeverity::Warning));
    assert(combined.has_severity(ErrorSeverity::Error));
    assert(combined.has_severity(ErrorSeverity::Fatal));
    // All errors are Warning or higher, so Info check should return true
    assert(combined.has_severity(ErrorSeverity::Info));
    
    std::cout << "✓ ErrorList aggregation works correctly\n";
}

void test_serialization_stability() {
    std::cout << "Testing serialization stability...\n";
    
    ErrorContext ctx = ErrorContext::create("test_operation", "test_location")
                          .with_parameter("param1", "value1")
                          .with_parameter("param2", "value2");
    
    Error error = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Error, 
                               "Processing failed", ctx);
    
    // Serialization must be stable
    std::string serialized1 = error.to_string();
    std::string serialized2 = error.to_string();
    assert(serialized1 == serialized2);
    
    // Same error produces same serialization
    Error error_copy = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Error, 
                                    "Processing failed", ctx);
    assert(error.to_string() == error_copy.to_string());
    
    std::cout << "✓ Serialization is stable and deterministic\n";
}

void test_error_immutability() {
    std::cout << "Testing Error immutability...\n";
    
    ErrorContext original_ctx = ErrorContext::create("original", "location");
    Error original_error = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Warning, 
                                        "Original message", original_ctx);
    
    // Adding parameter to context creates new context
    ErrorContext modified_ctx = original_ctx.with_parameter("new_key", "new_value");
    assert(original_ctx != modified_ctx);
    
    // Original context unchanged
    assert(original_ctx.parameters.empty());
    assert(modified_ctx.parameters.size() == 1);
    
    // ErrorList operations create new lists
    ErrorList original_list = ErrorList::single(original_error);
    Error new_error = Error::create(ErrorCode::ProcessingFailed, ErrorSeverity::Error, "New");
    ErrorList modified_list = original_list.add(new_error);
    
    assert(original_list.count() == 1);
    assert(modified_list.count() == 2);
    
    std::cout << "✓ Error types are immutable\n";
}

int main() {
    std::cout << "Running NX-Core Error System Tests\n";
    std::cout << "==================================\n";
    
    test_error_code_stability();
    test_error_context_determinism();
    test_error_context_ordering();
    test_error_determinism();
    test_error_ordering();
    test_error_success_failure();
    test_error_list_determinism();
    test_error_list_aggregation();
    test_serialization_stability();
    test_error_immutability();
    
    std::cout << "\n✓ All tests passed!\n";
    std::cout << "Deterministic error system validated.\n";
    
    return 0;
}