import can
import struct
import time
import threading

# Configuración del bus CAN
channel = 'PCAN_USBBUS1'  # Cambia según tu configuración
bitrate = 250000
updateGlobalClkPeriod = 3

bus = can.interface.Bus(interface='pcan', channel=channel, bitrate=bitrate)

def send_periodic_messages():
    """ Envía mensajes CAN periódicamente con marca de tiempo. """
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
            time.sleep(updateGlobalClkPeriod)  # Tiempo entre envíos periódicos
        except can.CanError as e:
            print(f"Error enviando mensaje: {e}")

def receive_messages():
    """ Recibe mensajes del bus CAN e imprime su contenido. """
    while True:
        message = bus.recv(timeout=1)  # Espera hasta 1 segundo por un mensaje
        if message:
            timestamp = f"{message.timestamp:.3f}s"
            msg_id = f"0x{message.arbitration_id:X}"
            data = " ".join(f"{byte:02X}" for byte in message.data)
            print(f"[Timestamp: {timestamp}] ID: {msg_id} | Data: {data}")

# Crear y ejecutar los hilos
send_thread = threading.Thread(target=send_periodic_messages, daemon=True)
receive_thread = threading.Thread(target=receive_messages, daemon=True)

send_thread.start()
receive_thread.start()

# Mantener el script en ejecución
try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    print("Saliendo del programa...")
