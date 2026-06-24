# Coding Standard Profile

This is a lightweight project coding standard inspired by defense/space practices. It is not a formal MISRA/ECSS compliance claim.

## C++ Rules

| Rule | Rationale |
|---|---|
| Use fixed-width integer types for wire data. | Binary compatibility |
| Avoid raw owning pointers. | Lifetime safety |
| Avoid unchecked buffer access. | Robustness |
| Packet decode shall validate size before reading. | Memory safety |
| Packet decode shall return explicit failure. | Fault containment |
| Use `[[nodiscard]]` for functions returning validation results. | Prevent ignored failures |
| Avoid hidden global mutable state. | Testability |
| Keep protocol, gateway, math, and utility code separated. | Maintainability |
| Log validation failures without crashing. | Operational availability |
| Keep magic numbers in named constants. | Reviewability |

## Java Rules

- Use DTO validation at API boundary.
- Keep alert logic in service layer, not controller.
- Use database migration scripts for schema.
- Use environment configuration for thresholds and API keys.

## Python/Shell Rules

- Tools must return non-zero exit code on failure.
- Test scripts should be repeatable.
- Sample payload files should be versioned.

## Documentation Rule

Every non-trivial feature should map to at least one requirement ID in the RTM.
