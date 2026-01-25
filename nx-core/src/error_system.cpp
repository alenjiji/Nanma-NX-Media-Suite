#include "error_system.h"
#include <sstream>
#include <algorithm>

namespace nx::core {

// ErrorContext implementation
ErrorContext ErrorContext::empty() {
    return ErrorContext{"", "", {}};
}

ErrorContext ErrorContext::create(std::string_view op, std::string_view loc) {
    return ErrorContext{std::string(op), std::string(loc), {}};
}

ErrorContext ErrorContext::with_parameter(std::string_view key, std::string_view value) const {
    ErrorContext result = *this;
    result.parameters[std::string(key)] = std::string(value);
    return result;
}

bool ErrorContext::operator<(const ErrorContext& other) const noexcept {
    if (operation != other.operation) {
        return operation < other.operation;
    }
    if (location != other.location) {
        return location < other.location;
    }
    return parameters < other.parameters;
}

std::string ErrorContext::to_string() const {
    std::ostringstream oss;
    oss << "ErrorContext{operation=\"" << operation << "\", location=\"" << location << "\"";
    
    if (!parameters.empty()) {
        oss << ", parameters={";
        bool first = true;
        for (const auto& [key, value] : parameters) {
            if (!first) oss << ", ";
            oss << key << "=\"" << value << "\"";
            first = false;
        }
        oss << "}";
    }
    
    oss << "}";
    return oss.str();
}

// Error implementation
Error Error::create(ErrorCode code, ErrorSeverity severity, 
                   std::string_view message, ErrorContext context) {
    return Error{code, severity, std::string(message), std::move(context)};
}

Error Error::create(ErrorCode code, ErrorSeverity severity, 
                   std::string_view message) {
    return Error{code, severity, std::string(message), ErrorContext::empty()};
}

bool Error::is_success() const noexcept {
    return code == ErrorCode::Success;
}

bool Error::is_failure() const noexcept {
    return code != ErrorCode::Success;
}

bool Error::operator<(const Error& other) const noexcept {
    if (code != other.code) {
        return static_cast<uint32_t>(code) < static_cast<uint32_t>(other.code);
    }
    if (severity != other.severity) {
        return static_cast<uint8_t>(severity) < static_cast<uint8_t>(other.severity);
    }
    if (message != other.message) {
        return message < other.message;
    }
    return context < other.context;
}

std::string Error::to_string() const {
    std::ostringstream oss;
    
    // Error code
    oss << "Error{code=" << static_cast<uint32_t>(code);
    
    // Severity
    oss << ", severity=";
    switch (severity) {
        case ErrorSeverity::Info: oss << "Info"; break;
        case ErrorSeverity::Warning: oss << "Warning"; break;
        case ErrorSeverity::Error: oss << "Error"; break;
        case ErrorSeverity::Fatal: oss << "Fatal"; break;
    }
    
    // Message
    oss << ", message=\"" << message << "\"";
    
    // Context (if not empty)
    if (!(context == ErrorContext::empty())) {
        oss << ", context=" << context.to_string();
    }
    
    oss << "}";
    return oss.str();
}

// ErrorList implementation
ErrorList ErrorList::empty() {
    return ErrorList{{}};
}

ErrorList ErrorList::single(Error error) {
    return ErrorList{{std::move(error)}};
}

ErrorList ErrorList::add(Error error) const {
    ErrorList result = *this;
    result.errors.push_back(std::move(error));
    
    // Maintain deterministic ordering
    std::sort(result.errors.begin(), result.errors.end());
    
    return result;
}

ErrorList ErrorList::combine(const ErrorList& other) const {
    ErrorList result = *this;
    result.errors.insert(result.errors.end(), other.errors.begin(), other.errors.end());
    
    // Maintain deterministic ordering
    std::sort(result.errors.begin(), result.errors.end());
    
    return result;
}

bool ErrorList::is_empty() const noexcept {
    return errors.empty();
}

bool ErrorList::has_errors() const noexcept {
    return !errors.empty();
}

size_t ErrorList::count() const noexcept {
    return errors.size();
}

bool ErrorList::has_severity(ErrorSeverity min_severity) const noexcept {
    return std::any_of(errors.begin(), errors.end(), 
        [min_severity](const Error& error) {
            return static_cast<uint8_t>(error.severity) >= static_cast<uint8_t>(min_severity);
        });
}

std::string ErrorList::to_string() const {
    if (errors.empty()) {
        return "ErrorList{empty}";
    }
    
    std::ostringstream oss;
    oss << "ErrorList{count=" << errors.size() << ", errors=[";
    
    for (size_t i = 0; i < errors.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << errors[i].to_string();
    }
    
    oss << "]}";
    return oss.str();
}

} // namespace nx::core