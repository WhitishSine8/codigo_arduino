//Laboratorio de Percepción Práctica #2
//Integrantes:
//Miguel Angel Villanueva Ramirez 1743596
//Irma Liliana Saucedo Martinez 1730895

//Librerías
#include <WiFi.h>
#include <DHT.h>
#include <Servo.h>

Servo ObjServo;

//Señales para los sensores
//DHT22
#define DHTPIN 4 //Pin al que se conectará el sensor
#define DHTTYPE DHT22 //Se define el tipo de sensor

//Fotorresistencia
#define LDR 34 //Pin analógico al que se conectará el sensor

//Configuración para el servidor
const char* ssid = "Totalplay-A29C";
const char* password = "A29C0FC0UUrgw7T4";
WiFiServer server(80); //Nota: investigar porque es 80

//Inicializamos el sensor DHT
DHT dht(DHTPIN, DHTTYPE);

//Variable para almacenar la solicitud HTTP
String header;

//Lectura de sensores
String SensoresEstado = "off";

//Para el Slider
//String valueString = String(0);
//int positon1 = 0;
//int positon2 = 0;

//Matriz m
double m[5][4] = 
{{-4.091161306277228, 0.34154925044493994, -0.19923055381405957, 0.033012220378430775},
 {-1.4202067331470096, -5.522937316999443, 1.0926903186709243, 4.675362200778682},
 {-1.1177504379413985, -1.5853078294834617, 2.25901751542432, -0.9538025036242919},
 {3.9549636884937103, -0.26860625588497694, -0.18846197835260178, -3.758786208162456},
 {0.4822201913450948, -4.180483112166716, -0.4361186076152109, 0.5599547293956691}};

//Matriz o
float o[5] = {-8.83920290863723, 4.748015376911499, 2.0873651110413824, 
-5.316140191895733, -6.929852236637968};

//Actuadores
const int LEDV1 = 18;
const int LEDV2 = 19;
const int LEDA = 21;
const int LEDR1 = 22;
const int LEDR2 = 23;
const int BUZ = 5;
const int servo = 15;

//Mediciones
float Temperatura;
float Humedad;
float nivel;
float normTemp;
float normHum;
float normniv;
float yy;
int LDR_val;

//Setup del programa
void setup() {
  Serial.begin(115200);
  dht.begin();
  //Inicializar las variables de salida como salidas (valga la redundancia).
  pinMode(LEDV1, OUTPUT);
  pinMode(LEDV2, OUTPUT);
  pinMode(LEDA, OUTPUT);
  pinMode(LEDR1, OUTPUT);
  pinMode(LEDR2, OUTPUT);
  pinMode(BUZ, OUTPUT);

  //Iniciamos las variables de salida en nivel bajo
  digitalWrite(LEDV1, LOW);
  digitalWrite(LEDV2, LOW);
  digitalWrite(LEDA, LOW);
  digitalWrite(LEDR1, LOW);
  digitalWrite(LEDR2, LOW);
  digitalWrite(BUZ, LOW);
  ObjServo.attach(servo);
 
  //Conexión a la red
  Serial.print("Conectando a ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  //Mientras el ESP32 se conecta a la red
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //Cuando se logre la conexión
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}



//Loop del programa
void loop() {
  WiFiClient client = server.available(); //Listen for incoming clients
  
  if (client) {
    Serial.println("Nuevo cliente");
    String currentLine = "";
    String currentline = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            if(header.indexOf("GET /S/on") >= 0) {
              
              //Sensor de humedad
              Humedad = dht.readHumidity();
              
              //Sensor de temperatura
              Temperatura = dht.readTemperature();
              
              //Nivel de luz
              LDR_val = analogRead(LDR);
              nivel = (LDR_val * 100)/4095;
              
              //Normalización de la temperatura
              normTemp = (Temperatura - 0)/(50 - 0);
              
              //Normalización de la humedad
              normHum = (Humedad - 0)/(100 - 0);
              
              //Normalización del nivel de luz
              normniv = (nivel - 0)/(100 - 0);
              
              float X[4] = {normTemp, normHum, normniv, 1};
              float N[5];

              //Red neuronal
              for(int n = 0; n < 5; n++) {
                float s = 0.0;
                for(int i = 0; i < 4; i++) {
                  s = s + (X[i] * m[n][i]);
                  }
                N[n] = (1 / (1 + exp(-s)));
                }
                float s = 0.0;
                for(int n = 0; n < 5; n++){
                  s = s + (N[n] * o[n]);
                }
                float y = (1/(1 + exp(-s)));
                yy = (y * 4) + 1;

                if(yy == 5) {
                  digitalWrite(LEDV1, HIGH);
                  digitalWrite(LEDV2, HIGH);
                  digitalWrite(LEDA, HIGH);
                  digitalWrite(LEDR1, HIGH);
                  digitalWrite(LEDR2, HIGH);
                  digitalWrite(BUZ, LOW);
                  ObjServo.write(180);
                }
                if(yy >= 4 && yy < 5) {
                  digitalWrite(LEDV1, LOW);
                  digitalWrite(LEDV2, HIGH);
                  digitalWrite(LEDA, HIGH);
                  digitalWrite(LEDR1, HIGH);
                  digitalWrite(LEDR2, HIGH);
                  digitalWrite(BUZ, LOW);
                  ObjServo.write(144);
                }
                if(yy>=3 && yy<4){
                  digitalWrite(LEDV1, LOW);
                  digitalWrite(LEDV2, LOW);
                  digitalWrite(LEDA, HIGH);
                  digitalWrite(LEDR1, HIGH);
                  digitalWrite(LEDR2, HIGH);
                  digitalWrite(BUZ, LOW);
                  ObjServo.write(108);
                }
                if(yy>=2 && yy<3){
                  digitalWrite(LEDV1, LOW);
                  digitalWrite(LEDV2, LOW);
                  digitalWrite(LEDA, LOW);
                  digitalWrite(LEDR1, HIGH);
                  digitalWrite(LEDR2, HIGH);
                  digitalWrite(BUZ, HIGH);
                  ObjServo.write(70);
                }
                if(yy>=1 && yy<2){
                  digitalWrite(LEDV1, LOW);
                  digitalWrite(LEDV2, LOW);
                  digitalWrite(LEDA, LOW);
                  digitalWrite(LEDR1, LOW);
                  digitalWrite(LEDR2, HIGH);
                  digitalWrite(BUZ, HIGH);
                  ObjServo.write(30);
                }
                Serial.println(yy);
                Serial.println(normTemp);
                Serial.println(normHum);
                Serial.println(normniv);
                SensoresEstado = "on";
                }else if (header.indexOf("GET /S/off") >= 0) {
                  //Sensor de humedad
                  Humedad = dht.readHumidity();
                  //Sensor de temperatura
                  Temperatura = dht.readTemperature();
                  //Sensor de nivel de luz
                  LDR_val = analogRead(LDR);
                  nivel = (LDR_val * 100)/4095;
                  //Normalización de la temperatura
                  normTemp = (Temperatura - 0)/(50 - 0);
                  //Normalización de la humedad
                  normHum = (Humedad - 0)/(100 - 0);

                  float X[4] = {normTemp, normHum, normniv, 1};
                  float N[5];

                  //Red neuronal
                  for(int n = 0; n < 5; n++) {
                    float s = 0.0;
                    for(int i = 0; i < 4; i++) {
                      s = s + (X[i] * m[n][i]);
                    }
                    N[n] = (1 / (1 + exp(-s)));
                    }
                    float s = 0.0;
                    for(int n = 0; n < 5; n++) {
                      s = s + (N[n] * o[n]);
                    }
                    float y = (1/(1 + exp(-s)));
                    yy = (y * 4) + 1;
                    if (yy == 5) {
                      digitalWrite(LEDV1, HIGH);
                      digitalWrite(LEDV2, HIGH);
                      digitalWrite(LEDA, HIGH);
                      digitalWrite(LEDR1, HIGH);
                      digitalWrite(LEDR2, HIGH);
                      digitalWrite(BUZ, LOW);
                      ObjServo.write(180);
                    }
                    if(yy >= 4 && yy < 5) {
                      digitalWrite(LEDV1, LOW);
                      digitalWrite(LEDV2, HIGH);
                      digitalWrite(LEDA, HIGH);
                      digitalWrite(LEDR1, HIGH);
                      digitalWrite(LEDR2, HIGH);
                      digitalWrite(BUZ, LOW);
                      ObjServo.write(144);
                    }
                    if(yy >= 3 && yy < 4) {
                      digitalWrite(LEDV1, LOW);
                      digitalWrite(LEDV2, LOW);
                      digitalWrite(LEDA, HIGH);
                      digitalWrite(LEDR1, HIGH);
                      digitalWrite(LEDR2, HIGH);
                      digitalWrite(BUZ, LOW);
                      ObjServo.write(108);
                    }
                    if(yy>=2 && yy<3){
                      digitalWrite(LEDV1, LOW);
                      digitalWrite(LEDV2, LOW);
                      digitalWrite(LEDA, LOW);
                      digitalWrite(LEDR1, HIGH);
                      digitalWrite(LEDR2, HIGH);
                      digitalWrite(BUZ, HIGH);
                      ObjServo.write(70);
                    }
                    if(yy>=1 && yy<2){
                      digitalWrite(LEDV1, LOW);
                      digitalWrite(LEDV2, LOW);
                      digitalWrite(LEDA, LOW);
                      digitalWrite(LEDR1, LOW);
                      digitalWrite(LEDR2, HIGH);
                      digitalWrite(BUZ, HIGH);
                      ObjServo.write(30);
                    }
                    Serial.println(yy);
                    Serial.println(normTemp);
                    Serial.println(normHum);
                    Serial.println(normniv);
                    SensoresEstado = "off";
                  }
                  //Página HTML
                  client.println("<!DOCTYPE html><html>");
                  client.println("<head><meta name=\"viewport\"content=\"width=device-width, initial-scale=1\">");
                  client.println("<link rel=\"icon\" href=\"data:,\">");
                  //CSS to style the on/off buttons
                  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                  client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial; margin-left:auto; margin-right:auto;}");
                  //Color para los sensores
                  client.println(".button1 { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                  client.println(".button2 {background-color: #555555;}</style></head>");
                  //Web page heading
                  client.println("<body><h1>Practica #2.</h1>");
                  client.println("<p>Nivel de Humedad: " + String (Humedad) +"%</p>" "<p>Temperatura: " + String (Temperatura) +" Grados C</p>" 
                  "<p>Nivel de Luz: " + String (nivel) +"%</p>");
                  if (SensoresEstado=="off") {
                    client.println("<p><a href=\"/S/on\"><button class=\"button1\">Leer</button></a></p>");
                  } else {
                    client.println("<p><a href=\"/S/off\"><button class=\"button1 button2\">Leer</button></a></p>");
                    }
                    client.println("</body></html>");
                    client.println();
                    break;
            } else {
              currentLine = "";
              }
            } else if(c != '\r') {
                currentLine += c;
        }
      }
    }
    header = ""; 
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
