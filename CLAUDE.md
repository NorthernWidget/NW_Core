# NW_Core

Shared foundation for the Northern Widget sensor libraries: the NW-Device-Specification Schema 1 device protocol (`NW_Device`), the readings store with statistics (`NW_Readings`, `NW_ReadingsConfig`), the report decoder (`NW_Report`), and the missing-value sentinel (`NW_Error.h`). Apis_Library, Walrus_Library and Haar_Library build on it.

## Standards

Follow the NW library standards for all work here: the root `CLAUDE.md` one level above `github/` has the checklist and code conventions, mirrored at [NW-Device-Specification/RELEASING.md](https://github.com/NorthernWidget/NW-Device-Specification/blob/master/RELEASING.md). The design and the line between what lives here and what stays in a library is [LIBRARY-DESIGN.md](https://github.com/NorthernWidget/NW-Device-Specification/blob/master/LIBRARY-DESIGN.md), section 0 and section 11.

## Working rules

- Core is extracted from what is identical across the libraries, never designed ahead of them. A change here is verified by every consumer's harness (`extras/test/run.sh` in Apis_Library, Walrus_Library, Haar_Library, T9602_Library and MCP23018) staying byte-identical, plus Core's own harness and a compile for `arduino:avr:uno` and `NorthernWidget:avr:NW1284p`.
- `extras/test/` holds the stubs (`Arduino.h`, `Wire.h`), the shared harness support (`NW_TestSupport.h`) and the shared runner (`run_library.sh`) that every library harness includes; changing them changes every harness.
- Formatting: the Arduino IDE auto-format (2-space indent, attached braces); lines under about 80 columns where practical.
- Unversioned (0.0.0) and unregistered until the 2026 overhaul ends; no CITATION.cff or .zenodo.json until then.

## Hard rule

**Never** create a git tag, GitHub release, push to a shared remote, or submit to any external registry (Zenodo, Arduino Library Manager, etc.) unless explicitly asked in the current message. If in doubt, ask.
