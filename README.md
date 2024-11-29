# Proyecto SensorFusion

Este repositorio contiene el código y los scripts necesarios para la creación del nodo del sensor LIDAR del un sistema de ayuda a la conducción autónoma mediante la fusión de sensores (camara OV2460 y LIDAR Garmin Lite v3) utilizando la plataforma Zynq con Vitis.

## Contenidos

- [Instalación](#instalación)
- [Generación del proyecto completo](#generación-del-proyecto-completo)
- [Compilación](#compilación)
- [Licencia](#licencia)

## Instalación

### Clonar el repositorio

Para comenzar, clona este repositorio en tu máquina local:

```bash
git clone https://github.com/alejandro-ayala/sensorFusion.git
```

## Generación del proyecto completo

Sigue los pasos a continuación para generar el proyecto completo, incluyendo la plataforma de hardware y software embebido.

1. **Abrir un terminal de Vitis** e ir a la carpeta `scripts` para generar la plataforma de hardware.

   Ejecuta el siguiente comando para generar la plataforma:

   ```bash
   xsct generate-platform.tcl
   ```

2. **Importar todos los proyectos** en Vitis:

   - Ve a **File -> Import**.
   - Selecciona la opción de **Eclipse workspace**.
   - Desmarca la opción **"Copy projects into workspace"**.
   - Importa los siguientes proyectos:
     - `SensorFusion_System` .Incluye el proyecto del sistema, donde estarán integrados cada uno de los proyectos para los procesadores.
     - `SensorFusion` Incluye el código de la aplicación, en este caso para el procesador PS0 del zynq7000. 
     - `zyboProject` Plataforma HW compilada en el paso anterior.

## Compilación

Una vez importados los proyectos, sigue estos pasos para compilar:

1. **Compilar el proyecto BSP de hardware (`zyboProject`)**:
   - Vitis podría indicar que está "out-of-date". Procede con la compilación del proyecto de hardware. Antes de compilar, abrir el fichero platform.spr para que vitis realice la inicialización correctamente. 

2. **Compilar el proyecto de software (`SensorFusion_System`)**:
   - Procede a compilar el proyecto que contiene el software de la aplicación de fusión de sensores.

## Licencia

Este proyecto está bajo la licencia MIT. Para más detalles, consulta el archivo [LICENSE](LICENSE).
