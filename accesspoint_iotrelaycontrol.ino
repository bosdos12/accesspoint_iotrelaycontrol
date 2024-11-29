#include <WiFi.h>
#include <WebServer.h> // Include the WebServer library

// Network credentials for Access Point (AP)
const char* ssid = "adak_esp32"; // SSID of the AP
const char* password = "12345678";        // Password of the AP

WebServer server(80); // Create a web server on port 80

// GPIO pins for actuators
int actuator1 = 5;
int actuator2 = 18;
int actuator3 = 19;
int actuator4 = 21;

// HTML content embedded directly in the code
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Light Control</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f4f4f9;
            color: #333;
            display: flex;
            flex-direction: column;
            align-items: center;
        }
        h1 {
            margin: 20px;
        }
        .container {
            display: flex;
            flex-direction: column;
            gap: 15px;
        }
        .box {
            width: 300px;
            padding: 20px;
            border: 2px solid #ccc;
            border-radius: 10px;
            background: #fff;
            text-align: center;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }
        .toggle {
            margin-top: 10px;
        }
    </style>
</head>
<body>
    <h1>Light Control</h1>
    <div class="container">
        <div class="box">
            <p>Light 1</p>
            <input type="checkbox" id="light1" class="toggle">
        </div>
        <div class="box">
            <p>Light 2</p>
            <input type="checkbox" id="light2" class="toggle">
        </div>
        <div class="box">
            <p>Light 3</p>
            <input type="checkbox" id="light3" class="toggle">
        </div>
        <div class="box">
            <p>Light 4</p>
            <input type="checkbox" id="light4" class="toggle">
        </div>
    </div>
    <script>
        // Add event listeners to all toggles
        document.querySelectorAll('.toggle').forEach(toggle => {
            toggle.addEventListener('change', (event) => {
                const lightId = event.target.id;
                const state = event.target.checked ? "L" : "H";
                fetch(`/${lightId}/${state}`)
                    .then(response => {
                        if (response.ok) console.log(`Toggled ${lightId} to ${state}`);
                        else console.error(`Error toggling ${lightId}`);
                    })
                    .catch(error => console.error(`Network error: ${error}`));
            });
        });
    </script>
</body>
</html>
)rawliteral";

// Handle requests for the main page
void handleRoot() {
  server.send(200, "text/html", index_html);
}

// Handle device toggling requests
void handleToggle() {
  String path = server.uri(); // Get the URI path
  String device, state;

  // Parse the path to extract the device and state
  if (path.startsWith("/light1/")) {
    state = path.substring(8); // Extract "L" or "H" after /light1/
    digitalWrite(actuator1, state == "L" ? LOW : HIGH);
  } else if (path.startsWith("/light2/")) {
    state = path.substring(8);
    digitalWrite(actuator2, state == "L" ? LOW : HIGH);
  } else if (path.startsWith("/light3/")) {
    state = path.substring(8);
    digitalWrite(actuator3, state == "L" ? LOW : HIGH);
  } else if (path.startsWith("/light4/")) {
    state = path.substring(8);
    digitalWrite(actuator4, state == "L" ? LOW : HIGH);
  }

  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // Set GPIO pins as output
  pinMode(actuator1, OUTPUT);
  pinMode(actuator2, OUTPUT);
  pinMode(actuator3, OUTPUT);
  pinMode(actuator4, OUTPUT);

  // Start Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point started.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Define routes
  server.on("/", handleRoot); // Serve the main page
  server.onNotFound(handleToggle); // Handle toggle requests

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient(); // Handle incoming client requests
}
