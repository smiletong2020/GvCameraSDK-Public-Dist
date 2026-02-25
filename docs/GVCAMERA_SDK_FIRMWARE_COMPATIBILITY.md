# GVCAMERA SDK Firmware Compatibility Matrix

## Token Count Convention (Mandatory)
- Read docs/DEVICE_PROTOCOL_SCHEMA_RULES.md before schema edits.
- Use payload_count as the single counting reference.
- payload_count excludes stx, <type>, edx.
- wire_token_count = payload_count + 3.

## Purpose
- Record SDK and firmware compatibility in one place.
- Prevent release-time ambiguity about required firmware versions.

## Rules
- Every SDK release must add or update one matrix row.
- If a feature requires newer firmware, add a feature-gate row.
- Matrix updates must be synchronized with:
  - `src/GvCameraSDK/layer_application/common/GvCameraVersion.h`
  - `docs/GvCameraSDK-Release-Notes.md`
  - `CHANGELOG.md`

## Schema Isolation Rule (Mandatory)
- V1 and V2 wire schemas are independent contracts.
- Do not auto-switch/fallback between V1 and V2 in runtime parsing.
- Do not pad missing fields with default values to match token count.
- If token order/length differs from the selected schema, fail decode.

## Matrix

| SDK Version | Minimum Supported FW | Recommended FW | Tested FW | Notes |
|---|---|---|---|---|
| 1.0.0 | TBD | TBD | TBD | Baseline release |

## Feature Gates

| Feature | Minimum FW | Code Reference | Notes |
|---|---|---|---|
| Extended projector code-domain payload (`resolution/min/max angle`) | 1.0.8 | `GV_FW_EXT_CODE_DOMAIN_MIN_VERSION_STR` | Older firmware uses V1 payload |
| GV parameter exchange (`get/set/save gv`) | 1.0.8 | `GV_FW_GV_PARAMS_V2_MIN_VERSION_STR` | `<= 1.0.7` does not exchange GV payload with firmware |

## Release Checklist (Firmware)
- Update matrix row for the target SDK version
- Confirm tested firmware list is complete
- Confirm feature-gate versions match code macros
- Add compatibility summary in release notes

