#include "../include/result.h"
#include <cassert>
#include <string>
#include <iostream>

using namespace nx::core;

// Test helper: simple value type for testing
struct TestValue {
    int data;
    
    TestValue(int d) : data(d) {}
    
    bool operator==(const TestValue& other) const noexcept {
        return data == other.data;
    }
    
    std::string to_string() const {
        return "TestValue(" + std::to_string(data) + ")";
    }
};

void test_result_success_construction() {
    std::cout << "Testing Result success construction..." << std::endl;
    
    // Test with move construction
    auto result1 = ok(TestValue(42));
    assert(result1.is_success());
    assert(!result1.is_failure());
    assert(result1.get_value().data == 42);
    
    // Test with copy construction
    TestValue val(100);
    auto result2 = ok(val);
    assert(result2.is_success());
    assert(!result2.is_failure());
    assert(result2.get_value().data == 100);
    
    // Test with primitive types
    auto result3 = ok(123);
    assert(result3.is_success());
    assert(result3.get_value() == 123);
    
    // Test with string
    auto result4 = ok(std::string("test"));
    assert(result4.is_success());
    assert(result4.get_value() == "test");
    
    std::cout << "✓ Result success construction tests passed" << std::endl;
}

void test_result_failure_construction() {
    std::cout << "Testing Result failure construction..." << std::endl;
    
    // Test with Error object
    auto error = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    auto result1 = fail<TestValue>(error);
    assert(!result1.is_success());
    assert(result1.is_failure());
    assert(result1.get_error().code == ErrorCode::InvalidInput);
    assert(result1.get_error().message == "Test error");
    
    // Test with factory function parameters
    auto result2 = fail<int>(ErrorCode::ResourceNotFound, ErrorSeverity::Warning, "Resource missing");
    assert(result2.is_failure());
    assert(result2.get_error().code == ErrorCode::ResourceNotFound);
    assert(result2.get_error().severity == ErrorSeverity::Warning);
    assert(result2.get_error().message == "Resource missing");
    
    // Test with context
    auto context = ErrorContext::create("test_op", "test_location");
    auto result3 = fail<std::string>(ErrorCode::ProcessingFailed, ErrorSeverity::Fatal, "Processing error", context);
    assert(result3.is_failure());
    assert(result3.get_error().context.operation == "test_op");
    assert(result3.get_error().context.location == "test_location");
    
    std::cout << "✓ Result failure construction tests passed" << std::endl;
}

void test_result_copy_move() {
    std::cout << "Testing Result copy and move operations..." << std::endl;
    
    // Test copy construction - success
    auto original_success = ok(TestValue(42));
    auto copied_success = original_success;
    assert(copied_success.is_success());
    assert(copied_success.get_value().data == 42);
    assert(original_success.is_success()); // Original unchanged
    
    // Test move construction - success
    auto moved_success = std::move(original_success);
    assert(moved_success.is_success());
    assert(moved_success.get_value().data == 42);
    
    // Test copy construction - failure
    auto original_failure = fail<TestValue>(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    auto copied_failure = original_failure;
    assert(copied_failure.is_failure());
    assert(copied_failure.get_error().code == ErrorCode::InvalidInput);
    assert(original_failure.is_failure()); // Original unchanged
    
    // Test move construction - failure
    auto moved_failure = std::move(original_failure);
    assert(moved_failure.is_failure());
    assert(moved_failure.get_error().code == ErrorCode::InvalidInput);
    
    std::cout << "✓ Result copy and move tests passed" << std::endl;
}

void test_result_comparison() {
    std::cout << "Testing Result comparison operations..." << std::endl;
    
    // Test success comparison
    auto success1 = ok(TestValue(42));
    auto success2 = ok(TestValue(42));
    auto success3 = ok(TestValue(100));
    
    assert(success1 == success2);
    assert(success1 != success3);
    
    // Test failure comparison
    auto error1 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    auto error2 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    auto error3 = Error::create(ErrorCode::ResourceNotFound, ErrorSeverity::Warning, "Different error");
    
    auto failure1 = fail<TestValue>(error1);
    auto failure2 = fail<TestValue>(error2);
    auto failure3 = fail<TestValue>(error3);
    
    assert(failure1 == failure2);
    assert(failure1 != failure3);
    
    // Test success vs failure comparison
    assert(success1 != failure1);
    
    std::cout << "✓ Result comparison tests passed" << std::endl;
}

void test_result_serialization() {
    std::cout << "Testing Result serialization..." << std::endl;
    
    // Test success serialization
    auto success = ok(TestValue(42));
    std::string success_str = success.to_string();
    assert(success_str.find("Success") != std::string::npos);
    
    // Test failure serialization
    auto failure = fail<TestValue>(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    std::string failure_str = failure.to_string();
    assert(failure_str.find("Failure") != std::string::npos);
    assert(failure_str.find("Test error") != std::string::npos);
    
    std::cout << "✓ Result serialization tests passed" << std::endl;
}

void test_void_result() {
    std::cout << "Testing VoidResult..." << std::endl;
    
    // Test success
    auto success = VoidResult::ok();
    assert(success.is_success());
    assert(!success.is_failure());
    
    // Test failure
    auto failure = VoidResult::fail(ErrorCode::ProcessingFailed, ErrorSeverity::Error, "Processing failed");
    assert(!failure.is_success());
    assert(failure.is_failure());
    assert(failure.get_error().code == ErrorCode::ProcessingFailed);
    
    // Test comparison
    auto success2 = VoidResult::ok();
    auto failure2 = VoidResult::fail(ErrorCode::ProcessingFailed, ErrorSeverity::Error, "Processing failed");
    
    assert(success == success2);
    assert(failure == failure2);
    assert(success != failure);
    
    // Test serialization
    assert(success.to_string() == "Success()");
    assert(failure.to_string().find("Failure") != std::string::npos);
    
    std::cout << "✓ VoidResult tests passed" << std::endl;
}

void test_multi_result() {
    std::cout << "Testing MultiResult..." << std::endl;
    
    // Test success
    auto success = multi_ok(TestValue(42));
    assert(success.is_success());
    assert(!success.is_failure());
    assert(success.get_value().data == 42);
    
    // Test failure with single error
    auto error = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    auto failure1 = multi_fail<TestValue>(error);
    assert(!failure1.is_success());
    assert(failure1.is_failure());
    assert(failure1.get_errors().count() == 1);
    
    // Test failure with error list
    auto error_list = ErrorList::single(error)
        .add(Error::create(ErrorCode::ResourceNotFound, ErrorSeverity::Warning, "Resource missing"));
    auto failure2 = multi_fail<TestValue>(error_list);
    assert(failure2.is_failure());
    assert(failure2.get_errors().count() == 2);
    
    // Test comparison
    auto success2 = multi_ok(TestValue(42));
    assert(success == success2);
    
    std::cout << "✓ MultiResult tests passed" << std::endl;
}

void test_deterministic_behavior() {
    std::cout << "Testing deterministic behavior..." << std::endl;
    
    // Same inputs should produce identical results
    auto result1 = ok(TestValue(42));
    auto result2 = ok(TestValue(42));
    assert(result1 == result2);
    assert(result1.to_string() == result2.to_string());
    
    // Same errors should produce identical failure results
    auto error1 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    auto error2 = Error::create(ErrorCode::InvalidInput, ErrorSeverity::Error, "Test error");
    auto failure1 = fail<TestValue>(error1);
    auto failure2 = fail<TestValue>(error2);
    assert(failure1 == failure2);
    assert(failure1.to_string() == failure2.to_string());
    
    std::cout << "✓ Deterministic behavior tests passed" << std::endl;
}

int main() {
    std::cout << "Running Result type tests..." << std::endl;
    
    test_result_success_construction();
    test_result_failure_construction();
    test_result_copy_move();
    test_result_comparison();
    test_result_serialization();
    test_void_result();
    test_multi_result();
    test_deterministic_behavior();
    
    std::cout << "\n✅ All Result type tests passed!" << std::endl;
    return 0;
}