#pragma once

#include "nx_batchflow_adapter.h"

// Sealed public engine APIs only
#include "../../../nx-engine-video/include/nx/video/VideoEngine.h"
#include "../../../nx-engine-video/include/nx/video/VideoRequest.h"
#include "../../../nx-engine-video/include/nx/video/VideoResult.h"

namespace nx::batchflow {

/**
 * VideoEngineAdapter
 *
 * Phase 3.B Execution Adapter for nx-engine-video
 *
 * Properties:
 * - Stateless
 * - Deterministic
 * - One JobNode -> One VideoEngine execution
 * - No color, HDR, scaling, or GOP interpretation
 * - No pipeline modification
 * - No inferred defaults
 */
class VideoEngineAdapter final : public ExecutionAdapter {
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

        // 2. Deserialize JobNode parameters into VideoRequest
        //    NOTE: VideoRequest::from_serialized() does not exist in current API
        //    This adapter cannot proceed without proper deserialization method
        //    Adapter Law prevents inventing or defaulting parameters
        return JobResult{
            JobResult::Status::InvalidInput,
            "VideoEngineAdapter: VideoRequest::from_serialized() method not available. "
            "Cannot deserialize parameters_blob without inventing values. "
            "Engine API must provide deserialization method.",
            {}
        };
    }

    std::string adapter_id() const noexcept override {
        return "nx.batchflow.VideoEngineAdapter.v1";
    }

    /**
     * Structural validation ONLY.
     * No video semantics, no color interpretation, no pipeline decisions.
     */
    Result<void> validate_job_node(const JobNode& job_node) const noexcept override {
        const auto& def = job_node.definition;

        if (def.engine_identifier != "nx-engine-video") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "VideoEngineAdapter: invalid engine_identifier"
            );
        }

        if (def.api_operation != "prepare") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "VideoEngineAdapter: invalid api_operation (expected 'prepare')"
            );
        }

        if (def.parameters_blob.empty()) {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "VideoEngineAdapter: parameters_blob is empty"
            );
        }

        // Adapter does NOT interpret parameters.
        // Engine owns full semantic validation.
        return ok();
    }
};

} // namespace nx::batchflow