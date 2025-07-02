# Release Notes

## Version 2.0.0 - 2025-06-22

### New features
- Implementación inicial de **ImageClassifierManager** para inferencia con TensorFlow Lite Micro.
- Nueva tarea **image3dCapturerTask** en `SystemTasksManager` para captura sincronizada de imágenes.
- Nuevo controlador CAN (**PsCanController**) con soporte para múltiples baudrates, manejo por IRQ y reset automático ante estado bus-off.
- Codificación CBOR para ensamblado y serialización de snapshots de imagen.
- Medición de tiempos de ejecución con `GTimer`.
- Cambio del controlador CAN de polling a IRQ para mayor eficiencia.

### Improvements
- Añadido mutex para acceso seguro en recepción CAN y Logger.
- Refactorización de tareas de comunicación para optimizar lectura de frames CAN.
- Ajuste y mejora en precisión del campo de visión (FoV) del Lidar.
- Limpieza de código y eliminación de headers de prueba.
- Eliminación de valores atípicos en nube de puntos 3D.

### Fixes
- Corrección de bloqueos en comunicación I2C.
- Prevención de condiciones de carrera en subsistemas de comunicación.
- Inicialización correcta de punteros en `TimeBaseManager`.
- Corrección de errores en el proceso de medición con Lidar.
- Mejoras en sincronización de tareas y notificaciones de fin de frame.


## Version 1.0.0 - 2025-05-01

### 🚀 Features
- Implement `ImageClassifierManager` using TensorFlow Lite Micro
- Add `SystemTasksManager::image3dCapturerTask` for coordinated image capture
- Implement new `PsCanController` abstraction layer with flexible baudrate support
- Rework I2C communication logic to polling-based for increased stability
- Introduce `MsgGateway` component to decouple data flow
- Enable CBOR encoding for structured sensor data transmission
- Upgrade timestamp fields to 64-bit to support longer runtimes
- Add TensorFlow Lite Micro library integration

### 🛠️ Improvements
- Refactor `CanController`: remove blocking loops, add mutex in RX/TX
- Improve Lidar measurement accuracy and configuration
- Reduce communication task latency
- Cleanup redundant test headers and simplify `main.cpp`
- Add mutex to `Logger` for thread safety

### 🐛 Fixes
- Fix bug in Lidar distance measurement
- Prevent I2C communication deadlocks
- Resolve race conditions in Logger and communication subsystems
- Correct uninitialized pointer in `TimeBaseManager`

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