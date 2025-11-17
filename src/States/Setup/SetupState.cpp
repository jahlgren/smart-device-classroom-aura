#include "SetupState.h"
#include "../../Hardware/Display.h"
#include "../../Utils.h"

SetupState::SetupState() : State() {}

void SetupState::enter() {
  Serial.println("SetupState: Enter");
  subState = SubState::Step1;
  finished = false;
  display.clear();

  config.load();
}

void SetupState::update() {
  if(prevSubState != subState) {
    prevSubState = subState;
    onSubStateChanged();
  }

  switch(subState) {
    case SubState::Step1: return onStep1Update();
    case SubState::Step2: return onStep2Update();
  }
}

void SetupState::exit() {
  Serial.println("SetupState: Exit");
  display.clear();
  stopServer();
}

State* SetupState::nextState() {
  return finished ? nullptr : nullptr;
}

void SetupState::onSubStateChanged() {
  switch(subState) {
    case SubState::Failed: return onFailedEnter();
    case SubState::Step1: return onStep1Enter();
    case SubState::Step2: return onStep2Enter();

    default:
      Serial.println("SubState not recognized");
      return;
  }
}

void SetupState::onFailedEnter() {
  Serial.println("Enter Failed!");
  display.clear();
  display.setBg(Display::Color::Error);
  display.setText(0, "Setup (Step 1)");
  display.setText(1, "Something went wrong, please try again.");
}

void concat(char* buffer, const char* text) {
  int i = 0;
  char c;
  while((c = text[i]) != '\0') {
    buffer[i] = c;
    i++;
  }
  buffer[i] = '\0';
}

void SetupState::onStep1Enter() {
  display.clear();
  display.setBg(Display::Color::Info);
  display.setText(0, "Setup (Step 1)");

  const char ssid[] = "CAConfig";
  char pass[9];
  generateRandomString(pass, 8);

  char buffer[64];
  snprintf(buffer, sizeof(buffer), "Connect to AP    >>    SSID: %s    PASS: %s", ssid, pass);
  display.setText(1, buffer);

  if(!startServer(ssid, pass)) {
    subState = SubState::Failed;
  }
}

void SetupState::onStep1Update() {
  if (apStatus != WiFi.status()) {
    apStatus = WiFi.status();
    if (apStatus == WL_AP_CONNECTED) {
      Serial.println("Setup Mode: Device connected to AP");
      subState = SubState::Step2;
    } else {
      Serial.println("Setup Mode: Device disconnected from AP");
    }
  }
}

void SetupState::onStep2Enter() {
  display.clear();
  display.setBg(Display::Color::Info);
  display.setText(0, "Setup (Step 2)");

  IPAddress ip = WiFi.localIP();
  
  char buffer[64];
  snprintf(buffer, sizeof(buffer), 
         "Go to: http://%u.%u.%u.%u",
         ip[0], ip[1], ip[2], ip[3]);

  display.setText(1, buffer);
}

void SetupState::onStep2Update() {
  WiFiClient client = server->available();
  if (client) { 
    Serial.println("Setup Mode: Client connected");
    String currentLine = "";
    int contentLength = 0;
    bool isPost = false;
    bool saveSuccess = false;
    String body = "";
    
    // Read headers
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // End of headers
            break;
          }
          // Check if this is POST
          if(currentLine.startsWith("POST")) {
            isPost = true;
          }
          // Check for Content-Length header
          if (currentLine.startsWith("Content-Length:")) {
            contentLength = currentLine.substring(15).toInt();
          }
          currentLine = "";
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    
    // handle POST
    if(isPost) {
      // Read POST body
      while (client.available() && body.length() < contentLength) {
        char c = client.read();
        body += c;
      }

      // --- Parse SSID, password, and device id ---
      String ssid, password, deviceId;

      int ssidStart     = body.indexOf("ssid=");
      int passStart     = body.indexOf("password=");
      int deviceIdStart = body.indexOf("did=");

      // --- SSID ---
      // if (ssidStart >= 0) {
      //   int amp = body.indexOf('&', ssidStart);
      //   if (amp > ssidStart) {
      //     ssid = body.substring(ssidStart + 5, amp);
      //   } else {
      //     ssid = body.substring(ssidStart + 5);
      //   }
      // }
      ssid = parseValue(body, "ssid");

      // --- Password ---
      // if (passStart >= 0) {
      //   int amp = body.indexOf('&', passStart);
      //   if (amp > passStart) {
      //     password = body.substring(passStart + 9, amp);
      //   } else {
      //     password = body.substring(passStart + 9);
      //   }
      // }
      password = parseValue(body, "password");

      // --- Device ID ---
      // if (deviceIdStart >= 0) {
      //   int amp = body.indexOf('&', deviceIdStart);
      //   if (amp > deviceIdStart) {
      //     deviceId = body.substring(deviceIdStart + 4, amp);
      //   } else {
      //     deviceId = body.substring(deviceIdStart + 4);
      //   }
      // }
      deviceId = parseValue(body, "did");

      strncpy(config.data.ssid, ssid.c_str(), sizeof(config.data.ssid) - 1);
      config.data.ssid[sizeof(config.data.ssid)-1] = '\0';
      
      strncpy(config.data.password, password.c_str(), sizeof(config.data.password) - 1);
      config.data.password[sizeof(config.data.password)-1] = '\0';
      
      strncpy(config.data.deviceId, deviceId.c_str(), sizeof(config.data.deviceId) - 1);
      config.data.deviceId[sizeof(config.data.deviceId)-1] = '\0';

      saveSuccess = config.save();

      // Debug print
      Serial.print("SSID: ");     Serial.println(config.data.ssid);
      Serial.print("Password: "); Serial.println(config.data.password);
      Serial.print("Device ID: "); Serial.println(config.data.deviceId);
    }

    // Send HTML Response

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html>");
    client.println("<html lang=\"en\"><head>");
    client.println("<meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    client.println("<title>Classroom Aura | Config</title>");
    client.println("<style>");
    client.println("* { margin: 0; padding: 0; box-sizing: border-box; }");
    client.println("body { background: white; color: rgba(0, 0, 0, 0.97); margin: 1rem; font-family: sans-serif; line-height: 1.5; }");
    client.println("h1, h2 { line-height: 1; }");
    client.println("h1 { font-size: 1.5rem; }");
    client.println("h2 { font-size: 0.75rem;}");
    client.println("form { margin-top: 1rem; display: flex; flex-direction: column; gap: 1rem; }");
    client.println("fieldset { padding: 0 1rem 1rem 1rem; border-radius: 4px; }");
    client.println("fieldset > div { display: flex; flex-direction: column; margin-top: 0.5rem; }");
    client.println("label { font-size: 0.75rem; font-weight: 500; }");
    client.println("input { padding: 0.5rem 1rem; border-radius: 4px; border: 1px solid #999999; font: inherit; }");
    client.println("button { padding: 0.5rem 1rem; border-radius: 4px; border: 1px solid #0066ff; background: #0066ff; color: white; cursor: pointer; }");
    client.println(".success { margin-bottom: 1rem; padding: 1rem; border-radius: 4px; background: #bfe2ba; }");
    client.println(".failed { margin-bottom: 1rem; padding: 1rem; border-radius: 4px; background: #e2baba; }");
    client.println("</style></head><body>");
    client.println("<h1>Classroom Aura</h1><h2>Configuration</h2>");
    client.println("<form method=\"POST\" action=\"\/\">");
    client.println("<fieldset>");
    client.println("<legend>WIFI Credentials</legend>");
    client.println("<div>");
    client.println("<label for=\"ssid\">SSID</label>");

    // Input for SSID
    client.println("<input type=\"text\" id=\"ssid\" name=\"ssid\" value=\"");
    client.println(config.data.ssid);
    client.println("\">");
    
    client.println("</div>");
    client.println("<div>");
    client.println("<label for=\"password\">Password</label>");

    // Input for Password
    client.println("<input type=\"text\" id=\"password\" name=\"password\" value=\"");
    client.println(config.data.password);
    client.println("\">");
    
    client.println("</div>");
    client.println("</fieldset>");
    client.println("<fieldset>");
    client.println("<legend>API</legend>");
    client.println("<div>");
    client.println("<label for=\"did\">Device ID</label>");

    // Input for DeviceID
    client.println("<input type=\"text\" id=\"did\" name=\"did\" value=\"");
    client.println(config.data.deviceId);
    client.println("\">");
    
    client.println("</div>");
    client.println("<div><strong>THIS IS A PROTOTYPE</strong><p>For a real device we also need an API_SECRET to be passed along with each HTTP request to know if this device is authorized..</p></div>");
    client.println("</fieldset>");
    client.println("<div>");

    if(isPost) {
      if(saveSuccess) {
        client.println("<p class=\"success\">Changes were successfully saved. A reboot is required for the changes to take effect.</p>");
      }
      else {
        client.println("<p class=\"failed\">Changes were not saved, something went wrong. Please try again.</p>");
      }
    }
    
    client.println("<button type=\"submit\">Save</button>");
    client.println("</div>");
    client.println("</form></body></html>");
    client.println();

    // Give browser time to receive data
    delay(1);

    client.stop();
    Serial.println("Client disconnected");
  }
}

bool SetupState::startServer(const char* ssid, const char* password) {
  apStatus = WiFi.beginAP(ssid, password);

  if (apStatus != WL_AP_LISTENING) {
    Serial.println("SetupState: Failed to start AP");
    return false;
  }

  Serial.println("SetupState: AP Started");

  server = new WiFiServer(80);
  server->begin();

  Serial.println("SetupState: HTTP Server Started");

  return true;
}

void SetupState::stopServer() {
  if (server) {
    delete server;
    server = nullptr;
  }

  WiFi.end();
  
  Serial.println("SetupState: AP And HTTP Server Stopped");
}
