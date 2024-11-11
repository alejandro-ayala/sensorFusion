#include <Wire.h>

#define LIDAR_ADDR 0x62      // Dirección I2C ficticia del LIDAR Garmin LITE v3

// Registros ficticios
const uint8_t REG_CONFIG = 0x00;         // Registro de configuración
const uint8_t REG_STATUS = 0x01;          // Registro de configuración
const uint8_t REG_SERIAL_NUMBER = 0x16;   // Registro de número de serie
const uint8_t REG_COMMAND = 0x40;
const uint8_t REG_TEST_MODE_DATA = 0xD2;
const uint8_t REG_ACQ_SETTINGS =  0x5D;
const uint8_t REG_DISTANCE = 0x8F;        // Registro de lectura de distancia
bool debugEnabled = false;
static const uint32_t simulatedSerialNumber = 0xABCD; // Número de serie ficticio
volatile uint8_t lastRegisterRequested = 0x00;   // Almacena el último registro solicitado
uint8_t testCycle = 0;
static uint16_t testData = 0;
static bool testMode = false;
uint32_t distanceSample = 0;
char buffer[50];

void setup() {
  Wire.begin(LIDAR_ADDR);        // Inicializar Arduino como esclavo I2C con dirección LIDAR_ADDR
  Wire.onRequest(requestEvent);  // Evento para solicitud de datos desde el maestro
  Wire.onReceive(receiveEvent);  // Evento para recepción de datos desde el maestro
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println("Arduino configurado como esclavo I2C simulando LIDAR Garmin LITE v3.");
}

void loop() {
  // El código principal puede estar vacío ya que todo el trabajo se hace en los eventos I2C
  delay(1000);
}

// Evento que se ejecuta cuando el maestro solicita datos
void requestEvent() {
  uint8_t status = 0x00;
  uint8_t acqSetting = 0xC0;
  uint8_t simulatedDistance[2];
  uint16_t distance;
  if(debugEnabled)Serial.print("requestEvent -->");
  if(debugEnabled)Serial.println(lastRegisterRequested, HEX);
  switch (lastRegisterRequested) {
    case REG_STATUS: 
      if(debugEnabled)Serial.println("switch --> REG_STATUS");
      
      Wire.write((uint8_t*)&status, 1);
    break;
    case REG_SERIAL_NUMBER: 
      if(debugEnabled)Serial.println("switch --> REG_SERIAL_NUMBER");
      Wire.write((uint8_t*)&simulatedSerialNumber, sizeof(simulatedSerialNumber));
    break;
    case REG_ACQ_SETTINGS: 
      if(debugEnabled)Serial.println("switch --> REG_ACQ_SETTINGS");
      
      Wire.write((uint8_t*)&acqSetting, 1);
    break;
    case REG_DISTANCE: 
      if(debugEnabled)Serial.println("switch --> REG_DISTANCE");
      
      simulatedDistance[0] = random(0, 256);
      simulatedDistance[1] = random(0, 256);
      distance = (simulatedDistance[0] << 8 | simulatedDistance[1]);
      sprintf(buffer, "distance [%d]: ", distanceSample);
      Serial.print(buffer);
      Serial.println(distance, HEX);
      Wire.write((uint8_t*)&simulatedDistance, sizeof(simulatedDistance));
      distanceSample++;
    break;
    case REG_TEST_MODE_DATA: 
      if(debugEnabled)Serial.println("switch --> REG_TEST_MODE_DATA");
      if(testMode) {
        Wire.write((uint8_t*)&testData, 2);
        if(debugEnabled)Serial.print("TestData: ");
        if(debugEnabled)Serial.println(testData);
        testData++;
        if(testCycle >= 9) {
          if(debugEnabled)Serial.println("Test mode disabled");
          testCycle = 0;
          testMode = false;
        }
      } else {
        if(debugEnabled)Serial.println("Modo test no activado");
      }
    break;
    default:
      if(debugEnabled)Serial.println("switch --> default");
      Wire.write((uint8_t)0);
    break;
  }



  if(debugEnabled)Serial.println("switch done");
}


// Evento que se ejecuta cuando el maestro envía datos
void receiveEvent(int bytes) {
  Serial.println("receiveEvent");
  if (bytes == 1) {
    // Leer el registro solicitado y almacenarlo
    lastRegisterRequested = Wire.read();
    if(debugEnabled)Serial.print("Registro de solicitado: ");
    if(debugEnabled)Serial.println(lastRegisterRequested, HEX);
  } else if (bytes == 2)
  {
    uint8_t configRegister = Wire.read();
    uint8_t configValue = Wire.read();
    if(debugEnabled)Serial.print("Configurando registro: ");
    if(debugEnabled)Serial.print(configRegister, HEX);
    if(debugEnabled)Serial.print("-- Con valor: ");
    if(debugEnabled)Serial.println(configValue, HEX);

    if(configRegister == REG_COMMAND && configValue == 0x07)
    {
      if(debugEnabled)Serial.println("Modo test activado");
      testMode = true;
    }
    
  }
  else
  {
    if(debugEnabled)Serial.println("Recibiendo múltiples bytes:");
    
    // Leer todos los bytes disponibles y mostrarlos
    while (Wire.available() > 0) {
      uint8_t receivedByte = Wire.read();
      if(debugEnabled)Serial.print("Byte recibido: ");
      if(debugEnabled)Serial.println(receivedByte, HEX);
    }    
  }
}

