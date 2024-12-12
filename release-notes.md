# Release Notes

## Version 0.2.0 - 2024-12-12

### Feature
- Release v0.2.0 includes:
  - `SystemTasksManager::communicationTask` working.
  - `SystemTasksManager::globalClockSynchronization` working.
  - `SystemTasksManager::image3dMappingTask` working.

## Version 0.1.2 - 2024-12-12

### Feature
- Communication task:
  - Send data over standard CAN.
  - Add timestamp to 3D image snapshot.
  - Add 2 bytes to identify CAN frame (LSB and MSB) and pattern to identify EOF.
- Scripts: Added master node simulator with Image3D reception and send global time.

### Fix
- Global time sync task: fix issues with CAN frame index.

## Version 0.1.1 - 2024-12-07

### Fix
- CI/CD pipeline: Fix generate tag release job when merge is done.

## Version 0.1.0 - 2024-12-01

### New features
- CI/CD pipeline: Add generate tag release job.
- Update HW file with the specific HW for the LIDAR node.

## Version 0.0.2 - 2024-11-29

### New features
- CI/CD integration: Add `version-check.yml` and `release-notes-check.yml`.

## Version 0.0.1 - 2024-11-27

### New features
- DataSerializer: Serialize the Image3D snapshot and split it into CAN messages.
- Fixes: Share Image3D snapshot between tasks using OSAL component.