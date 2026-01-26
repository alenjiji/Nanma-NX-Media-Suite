#pragma once

#include "nx_batchflow_adapter.h"

// Sealed public engine APIs only
#include "../../../nx-engine-convert/include/nx/convert/TranscodeEngine.h"
#include "../../../nx-engine-convert/include/nx/convert/TranscodeRequest.h"
#include "../../../nx-engine-convert/include/nx/convert/TranscodeResult.h"

namespace nx::batchflow {

/**
 * ConvertEngineAdapter
 *
 * Phase 3.B Golden Reference Adapter for nx-engine-convert
 *
 * Properties:
 * - Stateless
 * - Deterministic
 * - One JobNode -> One TranscodeEngine execution
 * - No media semantics (lossless vs lossy decisions)
 * - No defaults
 * - No interpretation
 */
class ConvertEngineAdapter final : public ExecutionAdapter {
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

        // 2. Deserialize JobNode parameters into TranscodeRequest
        //    NOTE: TranscodeRequest::from_serialized() does not exist in current API
        //    This adapter cannot proceed without proper deserialization method
        //    Adapter Law prevents inventing or defaulting parameters
        return JobResult{
            JobResult::Status::InvalidInput,
            "ConvertEngineAdapter: TranscodeRequest::from_serialized() method not available. "
            "Cannot deserialize parameters_blob without inventing values. "
            "Engine API must provide deserialization method.",
            {}
        };
    }

    std::string adapter_id() const noexcept override {
        return "nx.batchflow.ConvertEngineAdapter.v1";
    }

    /**
     * Structural validation ONLY.
     * No media semantics, no lossless/lossy decisions, no filesystem logic.
     */
    Result<void> validate_job_node(const JobNode& job_node) const noexcept override {
        const auto& def = job_node.definition;

        if (def.engine_identifier != "nx-engine-convert") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "ConvertEngineAdapter: invalid engine_identifier"
            );
        }

        if (def.api_operation != "prepare") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "ConvertEngineAdapter: invalid api_operation (expected 'prepare')"
            );
        }

        if (def.parameters_blob.empty()) {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "ConvertEngineAdapter: parameters_blob is empty"
            );
        }

        // Adapter does NOT interpret parameters.
        // Engine owns full semantic validation.
        return ok();
    }
};

} // namespace nx::batchflow