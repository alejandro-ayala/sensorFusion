import can
import time
import threading
import struct
from typing import Optional, Dict, DefaultDict
from collections import defaultdict
import cbor2
import os

# Configuración del bus CAN
channel = 'PCAN_USBBUS1'  # Cambia según tu configuración
bitrate = 250000
updateGlobalClkPeriod = 3
bus = can.interface.Bus(interface='pcan', channel=channel, bitrate=bitrate)
fakeTimeStamp = 0x34567811

class Image3DSnapshot:
    def __init__(self, byte_sequence):
        if isinstance(byte_sequence, str):
            self.byte_sequence = bytes.fromhex(byte_sequence)
        else:
            self.byte_sequence = byte_sequence        
        self.msg_id = None
        self.img_size = None
        self.img_buffer = None
        self.timestamp = None
        self.deserialized_data = None

    def deserialize(self):
        """Deserializa la secuencia de bytes CBOR y extrae los campos relevantes."""
        try:
            # Deserializar la secuencia de bytes usando cbor2
            self.deserialized_data = cbor2.loads(self.byte_sequence)

            # Extraer los campos
            self.msg_id = self.deserialized_data.get('msgId', None)
            self.img_size = self.deserialized_data.get('imgSize', None)
            self.img_buffer = self.deserialized_data.get('m_image3d', None)
            self.timestamp = self.deserialized_data.get('timestamp', None)

        except Exception as e:
            print(f"Error deserializando la secuencia de bytes: {e}")

    def print_fields(self):
        """Imprime los campos deserializados."""
        if self.deserialized_data:
            print(f"msgId: {self.msg_id}")
            print(f"imgSize: {self.img_size}")
            print(f"timestamp: {self.timestamp}")

            if self.img_buffer:
                # Mostrar algunos bytes del buffer de imagen para verificar
                print(f"imgBuffer (primeros 100 bytes): {self.img_buffer[:100]}")
        else:
            print("No se ha deserializado la secuencia de bytes todavía.")
            
    def write_to_file(self, file_path: str):
        """Escribe los datos deserializados en un fichero.

        Args:
            file_path (str): Ruta del fichero donde se guardarán los datos.
        """
        if not self.deserialized_data:
            print("No se ha deserializado la secuencia de bytes todavía.")
            return

        try:
            with open(file_path, 'w') as file:
                file.write(f"msgId: {self.msg_id}\n")
                file.write(f"imgSize: {self.img_size}\n")
                file.write(f"timestamp: {self.timestamp}\n")

            if self.img_buffer:
             
                file.write(f"imgBuffer (dict): {str(self.img_buffer)}\n")
            #    else:
            #        # Si img_buffer no es un diccionario, manejarlo como en el ejemplo anterior
            #        img_buffer_bytes = bytearray(self.img_buffer)  # Convertir lista a bytearray
            #        file.write(f"imgBuffer (hex): {img_buffer_bytes.hex()}\n")
                    
            print(f"Datos escritos exitosamente en el fichero: {file_path}")
        except Exception as e:
            print(f"Error escribiendo en el fichero: {e}")
        
class CANMessage:
    """Clase para representar un mensaje CAN."""
    def __init__(self, message_id: int, data: bytes):
        self.message_id = message_id  # ID del mensaje CAN
        self.data = data  # Datos del mensaje CAN (8 bytes)

        # Parsing de los bytes según la especificación
        self.image_id = self.data[0]
        self.frame_index = (self.data[2] << 8) | self.data[1]
        #print(f"Mensaje agregado. ImagenID: {self.image_id} -- frameIndex: {self.frame_index}")

    def __repr__(self):
        return (f"CANMessage(ID: {self.message_id}, Data: {' '.join(f'{b:02X}' for b in self.data)}, "
                f"ImageID: {self.image_id}, CborID: {self.cbor_id}, FrameIndex: {self.frame_index})")

class CBORAssembler:
    """Clase para ensamblar tramas CBOR a partir de mensajes CAN."""
    def __init__(self):
        # Diccionario para almacenar las tramas agrupadas por (image_id, cbor_id)
        self.tramas: Dict[MessageKey, Dict[FrameIndex, DataPayload]] = defaultdict(dict)

    def add_message(self, can_message: CANMessage):
        """Agrega un mensaje CAN al ensamblador."""
        key   = can_message.image_id
        index = can_message.frame_index
        data  = can_message.data[3:8]  # Bytes 3 a 7 contienen la carga útil

        if index in self.tramas[key]:
            print(f"Advertencia: Índice de trama duplicado {index} para {key}. Sobrescribiendo la trama existente.")

        self.tramas[key][index] = data
        #print(f"Mensaje agregado: {can_message}")
       
    def assemble_data(self, image_id: int) -> Optional[bytearray]:
        """Ensambla y retorna los datos completos para una imagen específica.
        
        Args:
            image_id (int): Identificador de la imagen cuyos datos se ensamblarán.

        Returns:
            Optional[bytearray]: Datos ensamblados o None si la imagen no está completa.
        """
        if image_id not in self.tramas:
            print(f"Error: No hay datos para la imagen con ID {image_id}.")
            return None

        # Obtener las tramas de la imagen específica
        image_frames = self.tramas[image_id]

        # Ordenar por índice de las tramas
        sorted_indices = sorted(image_frames.keys())

        # Ensamblar los datos en orden
        assembled_data = bytearray()
        for index in sorted_indices:
            assembled_data.extend(image_frames[index])
            
        #print(f"Tamaño total de los datos ensamblados para la imagen {image_id}: {len(assembled_data)} bytes")

        return assembled_data
    
# Función para recibir tramas CAN y mostrarlas en pantalla
def receive_can_messages():
    print("Esperando mensajes CAN (Formato estándar)...")
    assembler = CBORAssembler()
    while True:
        msg = bus.recv()  # Recibe un mensaje
        if msg:
            id_format = 'standard' if not msg.is_extended_id else 'extended'
            if id_format == 'standard':  # Filtrar solo CAN estándar
                #print(f"Mensaje recibido: ID={msg.arbitration_id:X}, "
                #      f"Length={msg.dlc}, "
                #      f"Data={msg.data.hex().upper()}")
                assembler.add_message(CANMessage(msg.arbitration_id, msg.data))
                if(msg.data[1] == 0xFF and msg.data[2] == 0xFF):
                    print(f"Received 3D image: ID={msg.data[0]}")
                    assembledData = assembler.assemble_data(msg.data[0])
                    snapshot = Image3DSnapshot(assembledData.hex())
                    snapshot.deserialize()
                    snapshot.print_fields()
                    snapshot.write_to_file(os.path.join(".", f"image_{msg.data[0]}.txt"))
# Función para enviar mensajes periódicos en formato estándar
def send_periodic_messages():
    

    while True:
        current_time_ns = time.time_ns()
        seconds = current_time_ns // 1_000_000_000  # Segundos actuales
        nanoseconds = current_time_ns % 1_000_000_000  # Porción de nanosegundos
        #print(f"Segundos: {seconds}, Nanosegundos: {nanoseconds}")

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
            #print("syncMessage")
            time.sleep(0.1)  # Pequeño retraso entre mensajes consecutivos
            bus.send(followUpMessage)
            #print("followUpMessage")
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
