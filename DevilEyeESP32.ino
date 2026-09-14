#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>
#include <ESPmDNS.h>

const char* WIFI_SSID = "RTkwifi";
const char* WIFI_PASSWORD = "00990099";

const int RELAY_PINS[4] = {
  5,
  17,
  16,
  4
};

const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

WebServer server(80);
Preferences prefs;

String relayNames[4] = {
  "Relay 1",
  "Relay 2",
  "Relay 3",
  "Relay 4"
};

bool relayStates[4] = {
  false,
  false,
  false,
  false
};

struct Task {
  bool active;
  int relay;
  bool value;
  unsigned long executeAt;
};

Task tasks[8];

void applyRelay(int index, bool value) {

  if (index < 0 || index > 3)
    return;

  relayStates[index] = value;

  digitalWrite(
    RELAY_PINS[index],
    value ? RELAY_ON : RELAY_OFF
  );

  prefs.putBool(
    ("r" + String(index)).c_str(),
    value
  );
}

void loadConfig() {

  for (int i = 0; i < 4; i++) {

    String key =
      "name" + String(i);

    relayNames[i] =
      prefs.getString(
        key.c_str(),
        "Relay " + String(i + 1)
      );

    relayStates[i] =
      prefs.getBool(
        ("r" + String(i)).c_str(),
        false
      );
  }
}

String jsonStatus() {

  String json = "{";

  json += "\"ok\":true,";
  json += "\"online\":true,";
  json += "\"ip\":\"";
  json += WiFi.localIP().toString();
  json += "\",";
  json += "\"host\":\"devil-eye.local\",";

  json += "\"states\":[";
  for (int i = 0; i < 4; i++) {

    json += relayStates[i] ? "true" : "false";

    if (i < 3)
      json += ",";
  }

  json += "],";

  json += "\"names\":[";
  for (int i = 0; i < 4; i++) {

    json += "\"";
    json += relayNames[i];
    json += "\"";

    if (i < 3)
      json += ",";
  }

  json += "],";

  json += "\"message\":\"RTK Devil Eye ESP32 online\"";

  json += "}";

  return json;
}

bool tokenOK() {

  /*
    Optional pairing token.

    To enable hardware authentication later:
    uncomment the comparison below and set the same token
    in the APK.

    For first prototype the token is optional.
  */

  return true;
}

void handleStatus() {

  if (!tokenOK()) {
    server.send(
      401,
      "application/json",
      "{\"ok\":false,\"error\":\"unauthorized\"}"
    );
    return;
  }

  server.send(
    200,
    "application/json",
    jsonStatus()
  );
}

void handleSet() {

  if (!tokenOK()) {
    server.send(
      401,
      "application/json",
      "{\"ok\":false}"
    );
    return;
  }

  if (!server.hasArg("r") ||
      !server.hasArg("v")) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false,\"error\":\"missing parameters\"}"
    );

    return;
  }

  int r =
    server.arg("r").toInt();

  int v =
    server.arg("v").toInt();

  if (r < 0 || r > 3) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false,\"error\":\"invalid relay\"}"
    );

    return;
  }

  applyRelay(
    r,
    v != 0
  );

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleAll() {

  if (!server.hasArg("v")) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false}"
    );

    return;
  }

  bool value =
    server.arg("v").toInt() != 0;

  for (int i = 0; i < 4; i++)
    applyRelay(i, value);

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleNames() {

  for (int i = 0; i < 4; i++) {

    String key =
      "r" + String(i);

    if (server.hasArg(key)) {

      String value =
        server.arg(key);

      if (value.length() > 0 &&
          value.length() <= 32) {

        relayNames[i] = value;

        prefs.putString(
          ("name" + String(i)).c_str(),
          value
        );
      }
    }
  }

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleSchedule() {

  if (!server.hasArg("r") ||
      !server.hasArg("v") ||
      !server.hasArg("d")) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false}"
    );

    return;
  }

  int relay =
    server.arg("r").toInt();

  bool value =
    server.arg("v").toInt() != 0;

  unsigned long delaySeconds =
    server.arg("d").toInt();

  if (
    relay < 0 ||
    relay > 3 ||
    delaySeconds > 86400
  ) {

    server.send(
      400,
      "application/json",
      "{\"ok\":false}"
    );

    return;
  }

  for (int i = 0; i < 8; i++) {

    if (!tasks[i].active) {

      tasks[i].active = true;
      tasks[i].relay = relay;
      tasks[i].value = value;
      tasks[i].executeAt =
        millis() +
        delaySeconds * 1000UL;

      server.send(
        200,
        "application/json",
        "{\"ok\":true}"
      );

      return;
    }
  }

  server.send(
    503,
    "application/json",
    "{\"ok\":false,\"error\":\"schedule full\"}"
  );
}

void handleTasks() {

  String json = "{\"ok\":true,\"tasks\":[";

  bool first = true;

  for (int i = 0; i < 8; i++) {

    if (tasks[i].active) {

      if (!first)
        json += ",";

      first = false;

      json += "{";
      json += "\"relay\":";
      json += tasks[i].relay;
      json += ",";
      json += "\"value\":";
      json += tasks[i].value ? "true" : "false";
      json += "}";
    }
  }

  json += "]}";

  server.send(
    200,
    "application/json",
    json
  );
}

void handleRoot() {

  String html =
    "<!doctype html>"
    "<html><head>"
    "<meta name='viewport' content='width=device-width,initial-scale=1'>"
    "<title>RTK Devil Eye</title>"
    "<style>"
    "body{font-family:Arial;background:#090712;color:white;padding:20px}"
    ".card{background:#171126;padding:18px;border-radius:18px;margin:10px 0}"
    "button{padding:14px 18px;border:0;border-radius:12px;margin:5px}"
    ".on{background:#7c4dff;color:white}"
    ".off{background:#333;color:white}"
    "</style></head><body>"
    "<h1>RTK Devil Eye</h1>"
    "<p>ESP32 Smart Controller</p>";

  for (int i = 0; i < 4; i++) {

    html +=
      "<div class='card'><h3>" +
      relayNames[i] +
      "</h3>";

    html +=
      "<a href='/set?r=" +
      String(i) +
      "&v=1'><button class='on'>ON</button></a>";

    html +=
      "<a href='/set?r=" +
      String(i) +
      "&v=0'><button class='off'>OFF</button></a>";

    html += "</div>";
  }

  html +=
    "<p>IP: " +
    WiFi.localIP().toString() +
    "</p>"
    "<p>Host: devil-eye.local</p>"
    "</body></html>";

  server.send(
    200,
    "text/html",
    html
  );
}

void connectWiFi() {

  WiFi.mode(WIFI_STA);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  unsigned long start =
    millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - start < 15000
  ) {

    delay(300);
  }
}

void setup() {

  Serial.begin(115200);

  /*
    SAFE BOOT:
    all relays OFF before Wi-Fi/config.
  */

  for (int i = 0; i < 4; i++) {

    pinMode(
      RELAY_PINS[i],
      OUTPUT
    );

    digitalWrite(
      RELAY_PINS[i],
      RELAY_OFF
    );

    relayStates[i] = false;

    tasks[i].active = false;
  }

  /*
    Initialize all remaining task slots.
  */

  for (int i = 4; i < 8; i++) {
    tasks[i].active = false;
  }

  prefs.begin(
    "devil-eye",
    false
  );

  loadConfig();

  /*
    Force safe state on boot.
  */

  for (int i = 0; i < 4; i++)
    applyRelay(i, false);

  connectWiFi();

  /*
    Start mDNS.

    ESP32 will be reachable as:
    http://devil-eye.local
  */

  if (WiFi.status() == WL_CONNECTED) {

    if (MDNS.begin("devil-eye")) {

      MDNS.addService(
        "http",
        "tcp",
        80
      );

      Serial.println(
        "mDNS: http://devil-eye.local"
      );

    } else {

      Serial.println(
        "mDNS start failed"
      );
    }
  }

  server.on(
    "/",
    handleRoot
  );

  server.on(
    "/status",
    handleStatus
  );

  server.on(
    "/set",
    handleSet
  );

  server.on(
    "/all",
    handleAll
  );

  server.on(
    "/names",
    handleNames
  );

  server.on(
    "/schedule",
    handleSchedule
  );

  server.on(
    "/tasks",
    handleTasks
  );

  server.begin();

  Serial.println();
  Serial.println(
    "RTK Devil Eye ESP32 ready"
  );

  Serial.print(
    "IP: "
  );

  Serial.println(
    WiFi.localIP()
  );

  Serial.println(
    "Host: http://devil-eye.local"
  );
}

void loop() {

  server.handleClient();

  /*
    Wi-Fi reconnect.
  */

  static unsigned long lastWifiCheck = 0;

  if (
    millis() - lastWifiCheck >
    10000
  ) {

    lastWifiCheck =
      millis();

    if (
      WiFi.status() !=
      WL_CONNECTED
    ) {

      WiFi.disconnect();
      WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
      );
    }
  }

  /*
    Execute scheduled tasks.
  */

  for (int i = 0; i < 8; i++) {

    if (
      tasks[i].active &&
      (long)(
        millis() -
        tasks[i].executeAt
      ) >= 0
    ) {

      applyRelay(
        tasks[i].relay,
        tasks[i].value
      );

      tasks[i].active = false;
    }
  }
}
