#pragma once

#include "nx_batchflow_adapter.h"

// Sealed public engine APIs only
#include "../../../nx-engine-audio/include/nx/audio/AudioEngine.h"
#include "../../../nx-engine-audio/include/nx/audio/AudioRequest.h"
#include "../../../nx-engine-audio/include/nx/audio/AudioResult.h"

namespace nx::batchflow {

/**
 * AudioEngineAdapter
 *
 * Phase 3.B Golden Reference Adapter
 *
 * Properties:
 * - Stateless
 * - Deterministic
 * - One JobNode -> One AudioEngine execution
 * - No DSP decisions
 * - No defaults
 * - No interpretation
 */
class AudioEngineAdapter final : public ExecutionAdapter {
public:
    JobResult execute(const JobNode& job_node) const override {
        // 1. Structural validation ONLY
        auto validation = validate_job_node(job_node);
        if (!validation.is_success()) {
            return JobResult{
                JobResult::Status::InvalidInput,
                validation.get_error().to_string(),
                {}
            };
        }

        // 2. Deserialize JobNode parameters into AudioRequest
        //    NOTE: AudioRequest::from_serialized() does not exist in current API
        //    This adapter cannot proceed without proper deserialization method
        //    Adapter Law prevents inventing or defaulting parameters
        return JobResult{
            JobResult::Status::InvalidInput,
            "AudioEngineAdapter: AudioRequest::from_serialized() method not available. "
            "Cannot deserialize parameters_blob without inventing values. "
            "Engine API must provide deserialization method.",
            {}
        };
    }

    std::string adapter_id() const noexcept override {
        return "nx.batchflow.AudioEngineAdapter.v1";
    }

    /**
     * Structural validation ONLY.
     * No media semantics, no policy, no filesystem checks.
     */
    Result<void> validate_job_node(const JobNode& job_node) const noexcept override {
        const auto& def = job_node.definition;

        if (def.engine_identifier != "nx-engine-audio") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "AudioEngineAdapter: invalid engine_identifier"
            );
        }

        if (def.api_operation != "prepare") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "AudioEngineAdapter: invalid api_operation (expected 'prepare')"
            );
        }

        if (def.parameters_blob.empty()) {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "AudioEngineAdapter: parameters_blob is empty"
            );
        }

        // Adapter does NOT interpret parameters.
        // Engine owns full semantic validation.
        return ok();
    }
};

} // namespace nx::batchflow