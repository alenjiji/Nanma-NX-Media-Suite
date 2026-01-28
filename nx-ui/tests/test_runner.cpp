// PHASE 15 ADAPTER COMPLIANCE TEST RUNNER
// Proves all adapters are stateless CLI bridges with no inference.

#include <iostream>

// Test function declarations
void run_monitor_adapter_tests();
void run_batch_adapter_tests();
void run_batch_introspection_adapter_tests();

int main() {
    std::cout << "Running Phase 15 Adapter Compliance Tests...\n";
    
    try {
        std::cout << "Testing MonitorAdapter...\n";
        run_monitor_adapter_tests();
        
        std::cout << "Testing BatchAdapter...\n";
        run_batch_adapter_tests();
        
        std::cout << "Testing BatchIntrospectionAdapter...\n";
        run_batch_introspection_adapter_tests();
        
        std::cout << "All adapter compliance tests passed!\n";
        std::cout << "✅ Adapters do not infer defaults\n";
        std::cout << "✅ Adapters do not retry\n";
        std::cout << "✅ Adapters do not modify CLI output\n";
        std::cout << "✅ CLI errors propagate unchanged\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}