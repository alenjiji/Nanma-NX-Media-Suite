#include "identity.h"
#include <iomanip>
#include <sstream>
#include <cstring>

// Simple SHA-256 implementation for deterministic hashing
// Platform-independent, no external dependencies
namespace {

// SHA-256 constants
constexpr uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

uint32_t rotr(uint32_t x, int n) {
    return (x >> n) | (x << (32 - n));
}

uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t sigma0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

uint32_t sigma1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

uint32_t gamma0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

uint32_t gamma1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

// Convert bytes to big-endian uint32_t
uint32_t bytes_to_uint32_be(const uint8_t* bytes) {
    return (static_cast<uint32_t>(bytes[0]) << 24) |
           (static_cast<uint32_t>(bytes[1]) << 16) |
           (static_cast<uint32_t>(bytes[2]) << 8) |
           static_cast<uint32_t>(bytes[3]);
}

// Convert uint32_t to big-endian bytes
void uint32_to_bytes_be(uint32_t value, uint8_t* bytes) {
    bytes[0] = static_cast<uint8_t>(value >> 24);
    bytes[1] = static_cast<uint8_t>(value >> 16);
    bytes[2] = static_cast<uint8_t>(value >> 8);
    bytes[3] = static_cast<uint8_t>(value);
}

nx::core::HashBytes sha256(std::string_view input) {
    // Initial hash values
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    // Prepare message
    std::vector<uint8_t> message(input.begin(), input.end());
    uint64_t original_length = message.size();
    
    // Padding
    message.push_back(0x80);
    while ((message.size() % 64) != 56) {
        message.push_back(0x00);
    }
    
    // Append length in bits as big-endian 64-bit
    uint64_t bit_length = original_length * 8;
    for (int i = 7; i >= 0; --i) {
        message.push_back(static_cast<uint8_t>(bit_length >> (i * 8)));
    }
    
    // Process message in 512-bit chunks
    for (size_t chunk = 0; chunk < message.size(); chunk += 64) {
        uint32_t w[64];
        
        // Copy chunk into first 16 words
        for (int i = 0; i < 16; ++i) {
            w[i] = bytes_to_uint32_be(&message[chunk + i * 4]);
        }
        
        // Extend into remaining 48 words
        for (int i = 16; i < 64; ++i) {
            w[i] = gamma1(w[i-2]) + w[i-7] + gamma0(w[i-15]) + w[i-16];
        }
        
        // Initialize working variables
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], h_var = h[7];
        
        // Main loop
        for (int i = 0; i < 64; ++i) {
            uint32_t t1 = h_var + sigma1(e) + ch(e, f, g) + K[i] + w[i];
            uint32_t t2 = sigma0(a) + maj(a, b, c);
            h_var = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }
        
        // Add chunk's hash to result
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += h_var;
    }
    
    // Convert to byte array
    nx::core::HashBytes result;
    for (int i = 0; i < 8; ++i) {
        uint32_to_bytes_be(h[i], &result[i * 4]);
    }
    
    return result;
}

} // anonymous namespace

namespace nx::core {

// Identity base class implementation
bool Identity::operator==(const Identity& other) const noexcept {
    return hash_ == other.hash_;
}

bool Identity::operator!=(const Identity& other) const noexcept {
    return hash_ != other.hash_;
}

bool Identity::operator<(const Identity& other) const noexcept {
    return hash_ < other.hash_;
}

std::string Identity::to_string() const {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (uint8_t byte : hash_) {
        oss << std::setw(2) << static_cast<unsigned>(byte);
    }
    return oss.str();
}

HashBytes Identity::compute_hash(std::string_view input) {
    return sha256(input);
}

// JobID implementation
JobID JobID::from_content(std::string_view content) {
    return JobID(compute_hash(content));
}

// RunID implementation
RunID RunID::from_content(std::string_view content) {
    return RunID(compute_hash(content));
}

// NodeID implementation
NodeID NodeID::from_content(std::string_view content) {
    return NodeID(compute_hash(content));
}

// ArtifactID implementation
ArtifactID ArtifactID::from_content(std::string_view content) {
    return ArtifactID(compute_hash(content));
}

} // namespace nx::core