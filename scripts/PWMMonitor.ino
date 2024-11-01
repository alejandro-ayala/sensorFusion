// Pines
const int pwmPin = 2;  // Pin donde se conecta la señal PWM (asegúrate de que sea digital)

// Variables para medir la duración de los pulsos
unsigned long highTime;  // Duración del pulso alto en microsegundos
unsigned long lowTime;   // Duración del pulso bajo en microsegundos

void setup() {
  Serial.begin(9600);  // Inicializamos el puerto serial para ver resultados
  pinMode(pwmPin, INPUT);  // Configuramos el pin como entrada
}

void loop() {
  // Leer la duración del pulso alto y bajo
  highTime = pulseIn(pwmPin, HIGH);
  lowTime = pulseIn(pwmPin, LOW);

  // Verificar que tenemos lecturas válidas (esto puede fallar si no hay señal PWM)
  if (highTime > 0 && lowTime > 0) {
    // Calcular el período total de la señal PWM
    unsigned long period = highTime + lowTime;

    // Calcular la frecuencia (Hz) = 1 / período en segundos
    float frequency = 1000000.0 / period;  // Convertimos el período de microsegundos a segundos

    // Calcular el ciclo de trabajo (duty cycle) en porcentaje
    float dutyCycle = (highTime * 100.0) / period;

    // Imprimir los resultados
    Serial.print("Frecuencia: ");
    Serial.print(frequency);
    Serial.print(" Hz, ");
    
    Serial.print("Ciclo de trabajo: ");
    Serial.print(dutyCycle);
    Serial.println(" %");
  } else {
    Serial.println("No se detecta señal PWM.");
  }

  delay(500);  // Esperamos un poco antes de la siguiente medición
}
