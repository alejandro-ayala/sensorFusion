import can
import time
import threading
import struct

# Configuración del bus CAN
channel = 'PCAN_USBBUS1'  # Cambia según tu configuración
bitrate = 250000
updateGlobalClkPeriod = 3
bus = can.interface.Bus(interface='pcan', channel=channel, bitrate=bitrate)

# Función para recibir tramas CAN y mostrarlas en pantalla
def receive_can_messages():
    print("Esperando mensajes CAN (Formato estándar)...")
    while True:
        msg = bus.recv()  # Recibe un mensaje
        if msg:
            id_format = 'standard' if not msg.is_extended_id else 'extended'
            if id_format == 'standard':  # Filtrar solo CAN estándar
                print(f"Mensaje recibido: ID={msg.arbitration_id:X}, "
                      f"Length={msg.dlc}, "
                      f"Data={msg.data.hex().upper()}")

# Función para enviar mensajes periódicos en formato estándar
def send_periodic_messages():
    

    while True:
        current_time_ns = time.time_ns()
        seconds = current_time_ns // 1_000_000_000  # Segundos actuales
        nanoseconds = current_time_ns % 1_000_000_000  # Porción de nanosegundos
        print(f"Segundos: {seconds}, Nanosegundos: {nanoseconds}")

        # Serializa segundos y nanosegundos en 4 bytes (big-endian)
        sec_bytes = struct.pack('>I', seconds)  # 4 bytes para los segundos
        ns_bytes = struct.pack('>I', nanoseconds)  # 4 bytes para los nanosegundos
        
        syncMessage = can.Message(
            arbitration_id=0x17,  # ID en formato estándar
            data=[0x20, 0xFE, 0x00, 0x13] + list(sec_bytes),
            is_extended_id=False  # CAN estándar
        )
        followUpMessage = can.Message(
            arbitration_id=0x17,  # ID en formato estándar
            data=[0x28, 0xF1, 0x00, 0x00] + list(ns_bytes), 
            is_extended_id=False  # CAN estándar
        )       
        try:
            bus.send(syncMessage)
            print("syncMessage")
            time.sleep(0.1)  # Pequeño retraso entre mensajes consecutivos
            bus.send(followUpMessage)
            print("followUpMessage")
            time.sleep(updateGlobalClkPeriod)  # Tiempo entre envíos periódicos (ajústalo según sea necesario)
        except can.CanError as e:
            print(f"Error enviando mensaje: {e}")

# Hilos separados para recibir y enviar mensajes
receive_thread = threading.Thread(target=receive_can_messages, daemon=True)
send_thread = threading.Thread(target=send_periodic_messages, daemon=True)

# Inicia los hilos
receive_thread.start()
send_thread.start()

# Mantén el programa corriendo
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Programa terminado.")
