# Portfolio Guide for Defense/Space Interview

## 30-Second Explanation

> This is a mini satellite ground control system. A C++ satellite simulator sends binary UDP telemetry packets. A C++ gateway validates packet size, magic, version, and CRC, then forwards valid telemetry to a Java Spring Boot mission server. The server persists telemetry, creates alerts, and exposes APIs used by a React dashboard and a C# operator console. Python and shell tools support replay, load, and smoke testing. I also prepared SRS, ICD, SDD, VTP, RTM, and risk documents to imitate defense/space development practices.

## What Not To Overclaim

Do not say:

- “This is real satellite software.”
- “This is certified aerospace code.”
- “I implemented CCSDS/ECSS fully.”

Say:

- “This is a portfolio project that follows the structure of mission software.”
- “I focused on interface control, validation, traceability, and testability.”
- “The goal was to connect my hardware production experience with ground-system software.”

## Best Interview Angle

Your previous experience:

- BOM extraction
- datasheet management
- material purchasing communication
- PCB Gerber generation
- SMT outsourcing
- debugging assembled boards

Connection:

> After experiencing hardware production flow, I wanted to understand how device data becomes operational software. This project simulates that chain from binary telemetry to mission server and operator display.
