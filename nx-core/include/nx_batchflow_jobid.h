#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nx::batchflow {

/// Forward declaration
class JobId;

/// ArtifactId represents a deterministic identifier for input/output artifacts
/// Used in JobId computation to ensure job identity includes all dependencies
class ArtifactId {
public:
    /// Create ArtifactId from deterministic string identifier
    explicit ArtifactId(std::string id) : id_(std::move(id)) {}
    
    /// Get the identifier string
    const std::string& id() const noexcept { return id_; }
    
    /// Equality comparison
    bool operator==(const ArtifactId& other) const noexcept { return id_ == other.id_; }
    bool operator!=(const ArtifactId& other) const noexcept { return id_ != other.id_; }
    
    /// Ordering for deterministic sorting
    bool operator<(const ArtifactId& other) const noexcept { return id_ < other.id_; }
    
    /// String representation
    std::string to_string() const { return id_; }

private:
    std::string id_;
};

/// JobDefinition contains all components that determine job identity
/// Used to compute deterministic JobId through content hashing
struct JobDefinition {
    std::string engine_identifier;        // Target engine name
    std::string api_operation;           // Specific engine API operation
    std::string parameters_blob;         // Canonicalized parameter data
    std::vector<ArtifactId> input_artifacts;   // Declared input dependencies
    std::vector<ArtifactId> output_artifacts;  // Declared output products
    
    /// Create job definition with all required components
    JobDefinition(std::string engine_id,
                  std::string operation,
                  std::string params_blob,
                  std::vector<ArtifactId> inputs,
                  std::vector<ArtifactId> outputs)
        : engine_identifier(std::move(engine_id))
        , api_operation(std::move(operation))
        , parameters_blob(std::move(params_blob))
        , input_artifacts(std::move(inputs))
        , output_artifacts(std::move(outputs))
    {
        // Sort artifact lists for deterministic ordering
        std::sort(input_artifacts.begin(), input_artifacts.end());
        std::sort(output_artifacts.begin(), output_artifacts.end());
    }
};

/// JobId implementation with deterministic content-based identity
class JobId {
public:
    /// Create JobId from job definition using deterministic content hashing
    /// This is the primary way to create JobIds - ensures deterministic identity
    static JobId from_job_definition(const JobDefinition& definition);
    
    /// Create JobId from pre-computed content hash
    /// Use only when hash is already computed deterministically
    static JobId from_content_hash(const std::string& content_hash) {
        return JobId(content_hash);
    }
    
    /// Get the underlying hash string
    const std::string& hash() const noexcept { return hash_; }
    
    /// Equality comparison for deterministic identity
    bool operator==(const JobId& other) const noexcept { return hash_ == other.hash_; }
    bool operator!=(const JobId& other) const noexcept { return hash_ != other.hash_; }
    
    /// Ordering for deterministic container usage
    bool operator<(const JobId& other) const noexcept { return hash_ < other.hash_; }
    
    /// String representation for serialization
    std::string to_string() const { return hash_; }

private:
    explicit JobId(std::string hash) : hash_(std::move(hash)) {}
    std::string hash_;
};

/// Deterministic content-hash utility for JobId computation
/// Produces stable hashes across machines and executions
class JobIdHasher {
public:
    /// Compute deterministic JobId from job definition
    /// Hash includes: engine_identifier + api_operation + parameters_blob + input_artifacts + output_artifacts
    /// Result is stable across machines and executions
    static JobId compute_job_id(const JobDefinition& definition);
    
    /// Compute deterministic hash from canonicalized string content
    /// Uses SHA-256 for cryptographic stability and collision resistance
    static std::string compute_content_hash(const std::string& canonical_content);

private:
    /// Create canonical string representation of job definition
    /// Format ensures deterministic serialization regardless of platform
    static std::string canonicalize_job_definition(const JobDefinition& definition);
    
    /// Serialize artifact list to canonical string format
    static std::string serialize_artifacts(const std::vector<ArtifactId>& artifacts);
    
    /// Simple SHA-256 implementation for deterministic hashing
    /// Platform-independent implementation ensures identical results across systems
    static std::string sha256_hash(const std::string& input);
};

/// Implementation of JobId methods
inline JobId JobId::from_job_definition(const JobDefinition& definition) {
    return JobIdHasher::compute_job_id(definition);
}

/// Implementation of JobIdHasher methods
inline JobId JobIdHasher::compute_job_id(const JobDefinition& definition) {
    // Create canonical string representation
    std::string canonical = canonicalize_job_definition(definition);
    
    // Compute deterministic hash
    std::string hash = compute_content_hash(canonical);
    
    return JobId::from_content_hash(hash);
}

inline std::string JobIdHasher::compute_content_hash(const std::string& canonical_content) {
    return sha256_hash(canonical_content);
}

inline std::string JobIdHasher::canonicalize_job_definition(const JobDefinition& definition) {
    std::ostringstream canonical;
    
    // Format: engine_identifier|api_operation|parameters_blob|inputs|outputs
    // Using | as separator since it's unlikely to appear in identifiers
    canonical << definition.engine_identifier << "|"
              << definition.api_operation << "|"
              << definition.parameters_blob << "|"
              << serialize_artifacts(definition.input_artifacts) << "|"
              << serialize_artifacts(definition.output_artifacts);
    
    return canonical.str();
}

inline std::string JobIdHasher::serialize_artifacts(const std::vector<ArtifactId>& artifacts) {
    std::ostringstream serialized;
    
    // Format: [artifact1,artifact2,artifact3]
    serialized << "[";
    for (size_t i = 0; i < artifacts.size(); ++i) {
        if (i > 0) serialized << ",";
        serialized << artifacts[i].to_string();
    }
    serialized << "]";
    
    return serialized.str();
}

inline std::string JobIdHasher::sha256_hash(const std::string& input) {
    // Self-contained SHA-256 implementation for deterministic hashing
    // Platform-independent, byte-stable results
    
    // SHA-256 constants
    static const uint32_t K[64] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };
    
    // Initial hash values
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    // Convert input to bytes
    std::vector<uint8_t> data(input.begin(), input.end());
    
    // Pre-processing: adding padding bits
    uint64_t original_length = data.size() * 8;
    data.push_back(0x80);
    
    // Pad to 448 bits (56 bytes) mod 512
    while ((data.size() % 64) != 56) {
        data.push_back(0x00);
    }
    
    // Append original length as 64-bit big-endian
    for (int i = 7; i >= 0; --i) {
        data.push_back((original_length >> (i * 8)) & 0xFF);
    }
    
    // Process message in 512-bit chunks
    for (size_t chunk = 0; chunk < data.size(); chunk += 64) {
        uint32_t w[64];
        
        // Break chunk into sixteen 32-bit big-endian words
        for (int i = 0; i < 16; ++i) {
            w[i] = (data[chunk + i * 4] << 24) |
                   (data[chunk + i * 4 + 1] << 16) |
                   (data[chunk + i * 4 + 2] << 8) |
                   (data[chunk + i * 4 + 3]);
        }
        
        // Extend the sixteen 32-bit words into sixty-four 32-bit words
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = ((w[i-15] >> 7) | (w[i-15] << 25)) ^
                          ((w[i-15] >> 18) | (w[i-15] << 14)) ^
                          (w[i-15] >> 3);
            uint32_t s1 = ((w[i-2] >> 17) | (w[i-2] << 15)) ^
                          ((w[i-2] >> 19) | (w[i-2] << 13)) ^
                          (w[i-2] >> 10);
            w[i] = w[i-16] + s0 + w[i-7] + s1;
        }
        
        // Initialize working variables
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h_temp = h[7];
        
        // Compression function main loop
        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = ((e >> 6) | (e << 26)) ^
                          ((e >> 11) | (e << 21)) ^
                          ((e >> 25) | (e << 7));
            uint32_t ch = (e & f) ^ (~e & g);
            uint32_t temp1 = h_temp + S1 + ch + K[i] + w[i];
            uint32_t S0 = ((a >> 2) | (a << 30)) ^
                          ((a >> 13) | (a << 19)) ^
                          ((a >> 22) | (a << 10));
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;
            
            h_temp = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }
        
        // Add compressed chunk to current hash value
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h_temp;
    }
    
    // Convert hash to hex string
    std::ostringstream result;
    for (int i = 0; i < 8; ++i) {
        result << std::hex << std::setfill('0') << std::setw(8) << h[i];
    }
    
    return result.str();
}

} // namespace nx::batchflow