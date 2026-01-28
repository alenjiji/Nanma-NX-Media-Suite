# Phase 15 — Qt UI Architecture (Index)

Phase 15 defines the complete, frozen architectural contract for the Qt UI
in Nanma NX-MediaSuite.

The UI is a terminal, replaceable consumer of CLI behavior.
It owns no semantics, no execution, and no authoritative state.

All documents listed below are normative and FROZEN.

---

## Phase 15 Documents

### Architecture
- [Phase 15.1 — Qt UI Architecture & Invariants](phase_15_1_qt_architecture_definition.md)
- [Phase 15 Mental Model — Development Proceedings](phase_15_mental_model.md)

### Widget Rules
- [Phase 15.3 — Widget → Adapter Wiring Rules](phase_15_3_widget_adapter_wiring.md)
- [Phase 15.3-C — UI Error Display Rules](phase_15_3_c_ui_error_display_rules.md)
- [Phase 15.3-C — UI Error Display Rules (Normative)](Phase_15.3-C_Freeze%20normative_UI_error_display_rules_with_verbatim_CLI_preservation.md)

### Replaceability & Enforcement
- [Phase 15.4-A — UI Deletion Test (Replaceability Proof)](phase_15_4_a_ui_deletion_test.md)
- [Phase 15.4-B — CI Enforcement Rules](phase_15_4_b_ci_enforcement_rules.md)  

---

## Authority Rules

- CLI is the sole canonical behavioral contract
- Python mirrors CLI exactly
- UI adds no semantics
- Adapters are stateless CLI bridges
- UI deletion must not affect system behavior

Any change violating these documents is an architectural failure.
