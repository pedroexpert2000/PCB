#include <Bluepad32.h>


// 1. DEFINICION DE PINES

const int STBY = 4;  // Habilita el puente H

// Motor Derecho = Joystick Derecho
const int PWMA = 16;  // Control de velocidad PWM
const int AIN1 = 18;  // Direccion
const int AIN2 = 19;  // Direccion

// Motor Izquierdo = Joystick Izquierdo
const int PWMB = 17;  // Control de velocidad PWM
const int BIN1 = 22;  // Direccion
const int BIN2 = 23;  // Direccion

// Guarda los controles conectados
ControllerPtr myControllers[BP32_MAX_GAMEPADS];


// 2. CONEXION DEL MANDO


// Guarda el mando cuando se conecta
void onConnectedController(ControllerPtr ctl) {
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("Mando PS4 conectado\n");
            myControllers[i] = ctl;
            break;
        }
    }
}

// Elimina el mando y detiene los motores al desconectarse
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



// 3. CONFIGURACION INICIAL

void setup() {
    Serial.begin(115200);

    // Pines utilizados como salidas hacia el puente H
    pinMode(STBY, OUTPUT);
    pinMode(PWMA, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);

    // Habilita el puente H e inicia con motores detenidos
    digitalWrite(STBY, HIGH);
    frenarTodo();

    // Inicializa la comunicacion con el mando
    BP32.setup(&onConnectedController, &onDisconnectedController);

    Serial.println("Esperando conexion del control de PS4...");
}


// 4. PROGRAMA PRINCIPAL


void loop() {

    // Actualiza los datos recibidos por Bluetooth
    BP32.update();

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        ControllerPtr miControl = myControllers[i];

        if (miControl && miControl->isConnected()) {

            // Lectura vertical de ambos joysticks (-511 a 512)
            int joyIzqY = miControl->axisY();
            int joyDerY = miControl->axisRY();


            // CONTROL MOTOR IZQUIERDO
            

            // Zona muerta para evitar movimientos involuntarios
            if (abs(joyIzqY) < 30) {
                digitalWrite(BIN1, LOW);
                digitalWrite(BIN2, LOW);
                analogWrite(PWMB, 0);

            } else {

                // Mapea el joystick al rango PWM de 0 a 255
                int pwmIzq = map(abs(joyIzqY), 30, 512, 0, 255);
                pwmIzq = constrain(pwmIzq, 0, 255);

                // Joystick hacia arriba
                if (joyIzqY < -30) {
                    digitalWrite(BIN1, HIGH);
                    digitalWrite(BIN2, LOW);
                    analogWrite(PWMB, pwmIzq);

                // Joystick hacia abajo
                } else if (joyIzqY > 30) {
                    digitalWrite(BIN1, LOW);
                    digitalWrite(BIN2, HIGH);
                    analogWrite(PWMB, pwmIzq);
                }
            }


            
            // CONTROL MOTOR DERECHO
      

            // Zona muerta para evitar movimientos involuntarios
            if (abs(joyDerY) < 30) {
                digitalWrite(AIN1, LOW);
                digitalWrite(AIN2, LOW);
                analogWrite(PWMA, 0);

            } else {

                // Mapea el joystick al rango PWM de 0 a 255
                int pwmDer = map(abs(joyDerY), 30, 512, 0, 255);
                pwmDer = constrain(pwmDer, 0, 255);

                // Joystick hacia arriba
                if (joyDerY < -30) {
                    digitalWrite(AIN1, HIGH);
                    digitalWrite(AIN2, LOW);
                    analogWrite(PWMA, pwmDer);

                // Joystick hacia abajo
                } else if (joyDerY > 30) {
                    digitalWrite(AIN1, LOW);
                    digitalWrite(AIN2, HIGH);
                    analogWrite(PWMA, pwmDer);
                }
            }
        }
    }

    // Pequena espera entre cada lectura
    delay(15);
}



// 5. DETENER MOTORES


// Coloca ambos motores en estado detenido
void frenarTodo() {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
    analogWrite(PWMA, 0);

    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
    analogWrite(PWMB, 0);
}