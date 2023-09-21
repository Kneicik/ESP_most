#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid     = "esp";
const char* password = "Karolina2137";

const int ENDSTOP_L_PIN = D5;  // GPIO14
const int ENDSTOP_R_PIN = D6;  // GPIO12
const int EMERGENCY_PIN = D7;  // GPIO13
const int MOTOR_L_PIN = D1;    // GPIO5
const int MOTOR_R_PIN = D2;    // GPIO4

ESP8266WebServer server(80);

void setup() {
  pinMode(ENDSTOP_L_PIN, INPUT);
  pinMode(ENDSTOP_R_PIN, INPUT);
  pinMode(EMERGENCY_PIN, INPUT);
  pinMode(MOTOR_L_PIN, OUTPUT);
  pinMode(MOTOR_R_PIN, OUTPUT);
  digitalWrite(MOTOR_L_PIN, LOW);
  digitalWrite(MOTOR_R_PIN, LOW);

  analogWriteRange(1023);  // Default PWM range for ESP8266
  
  Serial.begin(115200);
  delay(10);

  IPAddress staticIP(192, 168, 2, 10);
  IPAddress gateway(192, 168, 2, 1);   
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(staticIP, gateway, subnet);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/mostdown", handleMostDown);
  server.on("/mostup", handleMostUp);
  server.on("/motoroff", handleMotorOff);
  server.onNotFound(handleRoot);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  if(digitalRead(EMERGENCY_PIN) == HIGH) {
    ESP.restart();
  }
  if(digitalRead(ENDSTOP_L_PIN) == HIGH) {
    analogWrite(MOTOR_L_PIN, 0);
  }
  if(digitalRead(ENDSTOP_R_PIN) == HIGH) {
    analogWrite(MOTOR_R_PIN, 0);
  }
}

void handleRoot() {
  String html = R"=====(
    <!DOCTYPE html>
    <html>
    <form action="/mostdown">
    <input type="submit" value="Most down" />
    </form>
    <form action="/mostup">
    <input type="submit" value="Most up" />
    </form>
    <form action="/motoroff">
    <input type="submit" value="Motor off" />
    </form>
    </body>
    </html>
  )=====";
  
  server.send(200, "text/html", html);
}

void handleMostDown() {
    analogWrite(MOTOR_L_PIN, 1000);
    analogWrite(MOTOR_R_PIN, 0);
    // This will be non-blocking in the real world. Need additional checks for motoroff request.
  // analogWrite(MOTOR_L_PIN, 0);
  handleRoot();  // Redirect back to the main page
}

void handleMostUp() {
    analogWrite(MOTOR_R_PIN, 1000);
    analogWrite(MOTOR_L_PIN, 0);
    // This will be non-blocking in the real world. Need additional checks for motoroff request.
  // analogWrite(MOTOR_R_PIN, 0);
  handleRoot();  // Redirect back to the main page
}

void handleMotorOff() {
  analogWrite(MOTOR_L_PIN, 0);
  analogWrite(MOTOR_R_PIN, 0);
  handleRoot();  // Redirect back to the main page
}
