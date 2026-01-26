# NX-CLI - Phase 4 CLI Adapter

## Architecture

The CLI adapter is a **strict Layer 3 component** that provides headless access to NX-MediaSuite engines without adding any business logic, defaults, or convenience features.

## Design Principles

### Pure Adapter Pattern
- Maps CLI arguments 1:1 to engine operations
- No orchestration beyond argument → engine mapping
- No defaults, heuristics, or convenience logic
- Deterministic, audit-grade executions

### Explicit Everything
- All parameters must be explicitly specified
- No automatic codec selection or stream analysis
- No hidden processing or implicit operations
- Forbidden behaviors result in hard errors

## Usage

### Basic Structure
```bash
nx <component> <operation> [--explicit-flags-only]
```

### Convert Component
```bash
# Transcode with explicit policies
nx convert transcode \
  --input input.mov \
  --output output.mp4 \
  --container mp4 \
  --video passthrough \
  --audio encode:aac

# Analyze media (read-only)
nx convert analyze --input input.mov

# Verify results
nx convert verify --input input.mov --output output.mp4
```

### MetaFix Component
```bash
# Repair container structure
nx metafix repair \
  --input broken.mov \
  --output fixed.mov \
  --mode header-rebuild

# Validate compliance
nx metafix validate \
  --input test.mov \
  --profile broadcast

# Copy metadata categories
nx metafix metadata-copy \
  --source source.mov \
  --target target.mov \
  --categories technical,rights

# Merge metadata with precedence
nx metafix metadata-merge \
  --inputs file1.mov,file2.mov \
  --output merged.mov \
  --precedence file1.mov,file2.mov

# Normalize metadata schema
nx metafix metadata-normalize \
  --input input.mov \
  --output normalized.mov \
  --schema ebu-r128
```

### Supported Flags

#### Required for Transcode
- `--input <path>` - Input file path
- `--output <path>` - Output file path  
- `--container <type>` - Target container (mp4, mov, mkv, avi)
- `--video <policy>` - Video policy (passthrough, encode:codec)
- `--audio <policy>` - Audio policy (passthrough, encode:codec)

#### Optional (Engine will reject if streams exist but policy unspecified)
- `--subtitle <policy>` - Subtitle policy (passthrough, drop)
- `--metadata <policy>` - Metadata policy (passthrough, strip)
- `--dry-run` - Print resolved engine call without execution
- `--json` - Machine-readable output

## Error Codes

- `1` - CLI_USAGE_ERROR (Invalid flags/syntax)
- `2` - CLI_ENUM_ERROR (Unknown enum value)
- `3` - ENGINE_REJECTED (Engine validation failure)
- `4` - EXEC_FAILED (Runtime failure)

## Validation Layers

1. **CLI Parse Validation** - Syntax and required flags
2. **CLI Semantic Validation** - Enum validation only
3. **Engine Validation** - All real validation happens in engines

## Forbidden Behaviors

The CLI will **reject** these with hard errors:
- `--video encode` (missing codec)
- `--audio encode` (missing codec)
- `--container auto`
- Any implicit mapping or defaults

## Implementation Status

- ✅ Core CLI structure and argument parsing
- ✅ Convert component with transcode/analyze/verify operations
- ✅ MetaFix component with repair/validate/metadata operations
- ✅ Strict validation according to specification
- ✅ Deterministic output and error handling
- ⏳ Engine integration (placeholder implementations)
- ⏳ Additional components (audio, video, batch, monitor)

## Building

```bash
cd nx-cli
cmake -B build -S . -DBUILD_CLI_TESTS=ON
cmake --build build
./build/nx --help
```

## Testing

```bash
cd build
ctest
```

The CLI adapter strictly follows the Phase 4 specification and maintains the same deterministic, auditable characteristics as the underlying core system.