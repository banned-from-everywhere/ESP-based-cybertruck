#include <WiFi.h>
#include <WebServer.h>

// Access Point credentials
const char* ssid = "ESP32_Car";
const char* password = "12345678";

// Motor pins
const int rightMotorForward = 27;
const int rightMotorBackward = 26;
const int leftMotorForward = 12;
const int leftMotorBackward = 14;

WebServer server(80);

void setup() {
  // Initialize motor pins as output
  pinMode(rightMotorForward, OUTPUT);
  pinMode(rightMotorBackward, OUTPUT);
  pinMode(leftMotorForward, OUTPUT);
  pinMode(leftMotorBackward, OUTPUT);

  // Stop all motors at the start
  stopMotors();

  // Start Wi-Fi in Access Point mode
  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  Serial.println("Access Point started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Define web server routes
  server.on("/", handleRoot);
  server.on("/forward", []() { handleMotor(true, false, true, false); });
  server.on("/backward", []() { handleMotor(false, true, false, true); });
  server.on("/left", []() { handleMotor(false, true, true, false); });
  server.on("/right", []() { handleMotor(true, false, false, true); });
  server.on("/stop", []() { stopMotors(); });

  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>Car Control</title>
      <style>
        body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
        button { padding: 20px; margin: 10px; font-size: 20px; cursor: pointer; }
      </style>
      <script>
        let activeKey = null;

        document.addEventListener("keydown", function(event) {
          if (event.key !== activeKey) {
            activeKey = event.key;
            sendCommand(event.key);
          }
        });

        document.addEventListener("keyup", function(event) {
          activeKey = null;
          sendCommand("stop");
        });

        function sendCommand(key) {
          let url = "";
          switch (key.toLowerCase()) {
            case "w": url = "/forward"; break;
            case "a": url = "/left"; break;
            case "s": url = "/backward"; break;
            case "d": url = "/right"; break;
            case " ": url = "/stop"; break;
            default: return;
          }
          fetch(url).catch(err => console.log(err));
        }
      </script>
    </head>
    <body>
      <h1>WiFi Car Control</h1>
      <p>Use the buttons below or the keyboard (W/A/S/D/Space) to control the car.</p>
      <button onmousedown="sendCommand('w')" onmouseup="sendCommand('stop')">W</button><br>
      <button onmousedown="sendCommand('a')" onmouseup="sendCommand('stop')">A</button>
      <button onmousedown="sendCommand(' ')" onmouseup="sendCommand('stop')">Stop</button>
      <button onmousedown="sendCommand('d')" onmouseup="sendCommand('stop')">D</button><br>
      <button onmousedown="sendCommand('s')" onmouseup="sendCommand('stop')">S</button>
    </body>
    </html>
  )rawliteral");
}

void handleMotor(bool rf, bool rb, bool lf, bool lb) {
  digitalWrite(rightMotorForward, rf ? HIGH : LOW);
  digitalWrite(rightMotorBackward, rb ? HIGH : LOW);
  digitalWrite(leftMotorForward, lf ? HIGH : LOW);
  digitalWrite(leftMotorBackward, lb ? HIGH : LOW);
  server.send(200, "text/plain", "Motor command executed");
}

void stopMotors() {
  digitalWrite(rightMotorForward, LOW);
  digitalWrite(rightMotorBackward, LOW);
  digitalWrite(leftMotorForward, LOW);
  digitalWrite(leftMotorBackward, LOW);
  server.send(200, "text/plain", "Motors stopped");
}
