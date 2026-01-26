#pragma once

#include "nx_batchflow_adapter.h"

// Sealed public engine APIs only
#include "../../../nx-engine-meta/include/nx/meta/MetaEngine.h"
#include "../../../nx-engine-meta/include/nx/meta/MetaRepairRequest.h"
#include "../../../nx-engine-meta/include/nx/meta/MetaRepairResult.h"

namespace nx::batchflow {

/**
 * MetaEngineAdapter
 *
 * Phase 3.B Execution Adapter for nx-engine-meta
 *
 * Properties:
 * - Stateless
 * - Deterministic
 * - One JobNode -> One MetaEngine execution
 * - Media essence treated as read-only by default
 * - No metadata inference
 * - No automatic repair decisions
 * - No override logic
 */
class MetaEngineAdapter final : public ExecutionAdapter {
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

        // 2. Deserialize JobNode parameters into MetaRepairRequest
        auto request_result = nx::meta::parse_meta_repair_request(
            job_node.definition.parameters_blob
        );

        if (!request_result.is_success()) {
            return JobResult{
                JobResult::Status::InvalidInput,
                request_result.get_error().to_string(),
                {}
            };
        }

        const nx::meta::MetaRepairRequest& request = request_result.get_value();

        // 3. Execute exactly one engine operation
        nx::meta::MetaEngine engine;
        nx::meta::MetaRepairResult result = engine.plan_repair(request);

        // 4. Pass engine result through verbatim
        if (result.is_success()) {
            return JobResult{
                JobResult::Status::Success,
                "",  // MetaRepairResult has no message() method
                {}   // MetaRepairResult has no output_artifacts() method
            };
        } else {
            return JobResult{
                JobResult::Status::EngineFailure,
                result.get_error().to_string(),
                {}
            };
        }
    }

    std::string adapter_id() const noexcept override {
        return "nx.batchflow.MetaEngineAdapter.v1";
    }

    /**
     * Structural validation ONLY.
     * No metadata semantics, no repair decisions, no essence modification.
     */
    Result<void> validate_job_node(const JobNode& job_node) const noexcept override {
        const auto& def = job_node.definition;

        if (def.engine_identifier != "nx-engine-meta") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "MetaEngineAdapter: invalid engine_identifier"
            );
        }

        if (def.api_operation != "plan_repair") {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "MetaEngineAdapter: invalid api_operation (expected 'plan_repair')"
            );
        }

        if (def.parameters_blob.empty()) {
            return fail<void>(
                ErrorCode::InvalidInput,
                ErrorSeverity::Error,
                "MetaEngineAdapter: parameters_blob is empty"
            );
        }

        // Adapter does NOT interpret parameters.
        // Engine owns full semantic validation and safety rule enforcement.
        return ok();
    }
};

} // namespace nx::batchflow