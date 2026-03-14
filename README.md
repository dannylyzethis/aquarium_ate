# Aquarium Controller Migration Notes

## Active Firmware
- Primary firmware is `aquarium_project_top` targeting **Arduino GIGA R1 WiFi**.
- Cloud telemetry/alerts now run directly on the GIGA onboard WiFi.

## Build
- Compile with:
  - `arduino-cli compile --fqbn arduino:mbed_giga:giga aquarium_project_top`

## Secrets
- Create `aquarium_project_top/secrets.h` from `aquarium_project_top/secrets.example.h`.
- Keep `secrets.h` local only (gitignored).

## Legacy Modules
- `wifi_module_for_aquarium` is now a legacy/archival path.
- It is not required for single-board GIGA deployments.
