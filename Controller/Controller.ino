#include <Bluepad32.h>

// ==========================================
// 1. DEFINICIÓN DE PINES (ESP32 -> TB6612FNG)
// ==========================================
const int STBY = 4;

// Motor Derecho (A) -> Joystick Derecho
const int PWMA = 16;
const int AIN1 = 18;
const int AIN2 = 19;

// Motor Izquierdo (B) -> Joystick Izquierdo
const int PWMB = 17;
const int BIN1 = 22;
const int BIN2 = 23;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("Mando PS4 conectado\n");
            myControllers[i] = ctl;
            break;
        }
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("Mando PS4 desconectado\n");
            myControllers[i] = nullptr;
            frenarTodo();
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);
    
    pinMode(STBY, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);

    digitalWrite(STBY, HIGH);
    frenarTodo();

    BP32.setup(&onConnectedController, &onDisconnectedController);
    Serial.println("Esperando conexión del control de PS4...");
}

void loop() {
    BP32.update();

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        ControllerPtr miControl = myControllers[i];

        if (miControl && miControl->isConnected()) {
            
            // Los joysticks en Bluepad32 devuelven valores entre -511 y 512
            // Hacia ARRIBA es negativo, hacia ABAJO es positivo.
            int joyIzqY = miControl->axisY();   // Eje Y del Joystick Izquierdo
            int joyDerY = miControl->axisRY();  // Eje Y del Joystick Derecho

            // ====================================================
            // 1. CONTROL MOTOR IZQUIERDO
            // ====================================================
            // Zona muerta (Deadzone): ignorar valores menores a 30
            if (abs(joyIzqY) < 30) {
                digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW); analogWrite(PWMB, 0);
            } else {
                // Convertir el valor del joystick (30 a 512) a velocidad PWM (0 a 255)
                int pwmIzq = map(abs(joyIzqY), 30, 512, 0, 255);
                pwmIzq = constrain(pwmIzq, 0, 255); // Seguridad extra

                if (joyIzqY < -30) { // Joystick hacia ARRIBA
                    digitalWrite(BIN1, HIGH); digitalWrite(BIN2, LOW); analogWrite(PWMB, pwmIzq);
                } else if (joyIzqY > 30) { // Joystick hacia ABAJO
                    digitalWrite(BIN1, LOW); digitalWrite(BIN2, HIGH); analogWrite(PWMB, pwmIzq);
                }
            }

            // ====================================================
            // 2. CONTROL MOTOR DERECHO
            // ====================================================
            if (abs(joyDerY) < 30) {
                digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW); analogWrite(PWMA, 0);
            } else {
                int pwmDer = map(abs(joyDerY), 30, 512, 0, 255);
                pwmDer = constrain(pwmDer, 0, 255);

                if (joyDerY < -30) { // Joystick hacia ARRIBA
                    digitalWrite(AIN1, HIGH); digitalWrite(AIN2, LOW); analogWrite(PWMA, pwmDer);
                } else if (joyDerY > 30) { // Joystick hacia ABAJO
                    digitalWrite(AIN1, LOW); digitalWrite(AIN2, HIGH); analogWrite(PWMA, pwmDer);
                }
            }
        }
    }
    delay(15);
}

void frenarTodo() {
    digitalWrite(AIN1, LOW); digitalWrite(AIN2, LOW); analogWrite(PWMA, 0);
    digitalWrite(BIN1, LOW); digitalWrite(BIN2, LOW); analogWrite(PWMB, 0);
}