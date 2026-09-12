#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

const char* SSID = "RTkwifi";
const char* PASSWORD = "00990099";

#define R1 19
#define R2 18
#define R3 5
#define R4 17

#define ON LOW
#define OFF HIGH

WebServer server(80);
Preferences pref;

const byte pins[] = {R1, R2, R3, R4};

bool st[4] = {0, 0, 0, 0};
String nm[4] = {"Fan", "Light", "TV", "Cooler"};

unsigned long lastWifiCheck = 0;
const unsigned long WIFI_RETRY_INTERVAL = 5000;


// =====================================================
// SAVE RELAY STATES
// =====================================================

void saveStates() {
  for (byte i = 0; i < 4; i++) {
    String k = "s" + String(i);
    pref.putBool(k.c_str(), st[i]);
  }
}


// =====================================================
// LOAD RELAY STATES
// =====================================================

void loadStates() {
  for (byte i = 0; i < 4; i++) {
    String k = "s" + String(i);

    // false is default if nothing has been saved yet
    st[i] = pref.getBool(k.c_str(), false);
  }
}


// =====================================================
// APPLY RELAY STATES
// =====================================================

void relays() {
  for (byte i = 0; i < 4; i++) {
    digitalWrite(pins[i], st[i] ? ON : OFF);
  }
}


// =====================================================
// SAVE + APPLY
// =====================================================

void applyAndSave() {
  relays();
  saveStates();
}


// =====================================================
// JSON STATUS
// =====================================================

String json() {

  String j = "{\"s\":[";

  for (byte i = 0; i < 4; i++) {
    j += st[i] ? "1" : "0";

    if (i < 3) {
      j += ",";
    }
  }

  j += "],\"n\":[";

  for (byte i = 0; i < 4; i++) {

    j += "\"";

    // Basic JSON-safe name
    String safe = nm[i];
    safe.replace("\\", "\\\\");
    safe.replace("\"", "\\\"");

    j += safe;
    j += "\"";

    if (i < 3) {
      j += ",";
    }
  }

  j += "]}";

  return j;
}


// =====================================================
// WEB PAGE
// =====================================================

void page() {

String h = R"rawliteral(
<!DOCTYPE html>
<html>

<meta name=viewport content="width=device-width,initial-scale=1,user-scalable=no">

<title>RTK Devil Eye</title>

<style>

*{
box-sizing:border-box;
margin:0
}

html,body{
height:100%;
overflow:hidden
}

body{
font:14px Arial;
background:#05070b;
color:#eee;
padding:10px;
display:flex;
flex-direction:column
}

header{
height:50px;
display:flex;
align-items:center;
justify-content:space-between;
max-width:460px;
width:100%;
margin:auto
}

.logo{
font-weight:800;
font-size:16px;
color:#ff3333
}

.sub{
font-size:8px;
color:#777;
letter-spacing:1px
}

.onl{
font-size:9px;
color:#55e888;
background:#0b1811;
padding:6px 9px;
border-radius:15px
}

main{
width:100%;
max-width:460px;
margin:auto
}

.info{
display:flex;
justify-content:space-between;
background:#10151d;
border:1px solid #252d38;
border-radius:10px;
padding:8px;
margin-bottom:8px;
font-size:10px
}

.grid{
display:grid;
grid-template-columns:1fr 1fr;
gap:8px
}

.card{
height:115px;
background:linear-gradient(145deg,#181e27,#0b0e13);
border:1px solid #29313c;
border-radius:14px;
padding:11px;
display:flex;
flex-direction:column;
justify-content:space-between
}

.card.a{
border-color:#a52525;
box-shadow:0 0 14px #ff00001c
}

.no{
font-size:8px;
color:#687384
}

.name{
font-size:14px;
font-weight:bold;
margin-top:5px;
white-space:nowrap;
overflow:hidden;
text-overflow:ellipsis
}

.badge{
font-size:8px;
color:#ff6666;
background:#271214;
padding:4px 6px;
border-radius:8px;
float:right
}

.badge.a{
color:#65ed91;
background:#102518
}

.sw{
width:48px;
height:25px;
position:relative
}

.sw input{
display:none
}

.sl{
position:absolute;
inset:0;
background:#29313b;
border-radius:20px
}

.sl:before{
content:"";
position:absolute;
width:19px;
height:19px;
left:3px;
top:3px;
background:#999;
border-radius:50%;
transition:.2s
}

input:checked+.sl{
background:#771818
}

.sw input:checked+.sl:before{
transform:translateX(23px);
background:#ff3333;
box-shadow:0 0 8px red
}

.buttons{
display:grid;
grid-template-columns:1fr 1fr;
gap:8px;
margin-top:8px
}

button{
height:40px;
border:1px solid #303846;
border-radius:10px;
background:#121720;
color:white;
font-weight:bold
}

.red{
background:#701515;
border-color:#a52b2b
}

.rename{
margin-top:8px;
width:100%;
height:38px;
background:#0e131a;
border:1px solid #29313c;
border-radius:10px;
color:#aaa;
padding:0 10px
}

.foot{
max-width:460px;
width:100%;
margin:auto;
text-align:center;
font-size:8px;
color:#4e5866
}

#toast{
position:fixed;
bottom:18px;
left:50%;
transform:translate(-50%,20px);
opacity:0;
background:#151a22;
border:1px solid #394352;
border-radius:9px;
padding:9px 15px;
font-size:10px;
font-weight:bold;
transition:.2s
}

#toast.show{
opacity:1;
transform:translate(-50%,0)
}

@media(max-height:620px){

.card{
height:95px
}

header{
height:42px
}

.info{
padding:6px
}

.buttons button{
height:35px
}

}

</style>

<body>

<header>

<div>

<div class=logo>◉ RTK DEVIL EYE</div>

<div class=sub>
DEPARTMENT • SMART CONTROL
</div>

</div>

<div class=onl>
● ONLINE
</div>

</header>


<main>

<div class=info>

<b>DEVICE CONTROL</b>

<span>
ACTIVE:
<b id=count>0</b>/4
</span>

</div>


<div class=grid id=grid></div>


<div class=buttons>

<button class=red onclick=all(1)>
⚡ ALL ON
</button>

<button onclick=all(0)>
ALL OFF
</button>

</div>


<input
class=rename
id=rn
maxlength=18
placeholder="Device name change karne ke liye tap karein..."
onclick=rename()
>


</main>


<div class=foot>
RTK SYSTEM • ESP32 LOCAL NETWORK
</div>


<div id=toast></div>


<script>

let s=[0,0,0,0],
n=["Fan","Light","TV","Cooler"],
sel=0;


function ui(){

let g="",
a=0;

for(let i=0;i<4;i++){

if(s[i])a++;

g+=`

<div class="card ${s[i]?'a':''}">

<div>

<span class=no>
RELAY 0${i+1}
</span>

<span class="badge ${s[i]?'a':''}">
${s[i]?'ON':'OFF'}
</span>

<div class=name>
${n[i]}
</div>

</div>

<label class=sw>

<input
type=checkbox
${s[i]?'checked':''}
onchange="set(${i},this.checked?1:0)"
>

<span class=sl></span>

</label>

</div>

`;

}

grid.innerHTML=g;
count.innerText=a;

}


function get(){

fetch('/status')

.then(x=>x.json())

.then(x=>{

s=x.s;
n=x.n;

ui();

})

.catch(()=>msg('ESP32 OFFLINE'));

}


function set(i,v){

fetch(`/set?r=${i}&v=${v}`)

.then(x=>x.json())

.then(x=>{

s=x.s;
n=x.n;

ui();

msg(n[i]+(v?' ON':' OFF'));

})

.catch(()=>msg('COMMAND FAILED'));

}


function all(v){

fetch('/all?v='+v)

.then(x=>x.json())

.then(x=>{

s=x.s;
n=x.n;

ui();

msg(v?'ALL ON':'ALL OFF');

})

.catch(()=>msg('COMMAND FAILED'));

}


function rename(){

let x=[];

for(let i=0;i<4;i++){

let z=prompt(
'Relay '+(i+1)+' name:',
n[i]
);

x.push(
z&&z.trim()
?z.trim()
:n[i]
);

}

fetch(
'/names?'+
x.map(
(v,i)=>
'r'+i+'='+encodeURIComponent(v)
).join('&')
)

.then(x=>x.json())

.then(x=>{

s=x.s;
n=x.n;

ui();

msg('NAMES SAVED');

})

.catch(()=>msg('NAME SAVE FAILED'));

}


function msg(t){

toast.innerText=t;

toast.className='show';

clearTimeout(window.tt);

window.tt=setTimeout(
()=>toast.className='',
1400
);

}


get();

</script>

</body>

</html>
)rawliteral";

server.send(200, "text/html", h);
}


// =====================================================
// WIFI CONNECT / RECONNECT
// =====================================================

void connectWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.println("WiFi connecting...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  unsigned long start = millis();

  // Only wait a limited time.
  // ESP32 will NOT get stuck here forever.
  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - start < 10000
  ) {

    delay(300);

    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("WiFi connected!");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

  } else {

    Serial.println("WiFi not available.");
    Serial.println("Will retry automatically.");

  }
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(300);


  // ---------------------------------------------------
  // RELAY GPIO
  // ---------------------------------------------------

  for (byte i = 0; i < 4; i++) {

    pinMode(pins[i], OUTPUT);

    digitalWrite(pins[i], OFF);
  }


  // ---------------------------------------------------
  // PREFERENCES
  // ---------------------------------------------------

  pref.begin("rtk", false);


  // Load saved relay states
  loadStates();


  // Load saved names
  for (byte i = 0; i < 4; i++) {

    String k = "r" + String(i);

    nm[i] = pref.getString(
      k.c_str(),
      nm[i]
    );
  }


  // IMPORTANT:
  // Restore previous relay states
  // before WiFi connection.
  relays();


  Serial.println();
  Serial.println("==============================");
  Serial.println(" RTK DEVIL EYE ESP32");
  Serial.println("==============================");

  Serial.println("Saved relay states restored.");


  // ---------------------------------------------------
  // WIFI
  // ---------------------------------------------------

  WiFi.mode(WIFI_STA);

  WiFi.setAutoReconnect(true);

  WiFi.persistent(false);

  connectWiFi();


  // ---------------------------------------------------
  // WEB SERVER
  // ---------------------------------------------------

  server.on("/", page);


  // ---------------------------------------------------
  // STATUS
  // ---------------------------------------------------

  server.on("/status", []() {

    server.send(
      200,
      "application/json",
      json()
    );

  });


  // ---------------------------------------------------
  // SINGLE RELAY
  // ---------------------------------------------------

  server.on("/set", []() {

    int r = server.arg("r").toInt();

    int v = server.arg("v").toInt();


    if (r >= 0 && r < 4) {

      st[r] = v ? true : false;

      applyAndSave();

      server.send(
        200,
        "application/json",
        json()
      );

      return;
    }


    server.send(
      400,
      "application/json",
      "{\"error\":\"invalid relay\"}"
    );

  });


  // ---------------------------------------------------
  // ALL RELAYS
  // ---------------------------------------------------

  server.on("/all", []() {

    bool v = server.arg("v").toInt();


    for (byte i = 0; i < 4; i++) {

      st[i] = v;
    }


    applyAndSave();


    server.send(
      200,
      "application/json",
      json()
    );

  });


  // ---------------------------------------------------
  // RENAME
  // ---------------------------------------------------

  server.on("/names", []() {

    for (byte i = 0; i < 4; i++) {

      String k = "r" + String(i);


      if (server.hasArg(k)) {

        String value = server.arg(k);

        value.trim();


        if (value.length() > 0) {

          nm[i] = value;

          pref.putString(
            k.c_str(),
            nm[i]
          );
        }
      }
    }


    server.send(
      200,
      "application/json",
      json()
    );

  });


  // ---------------------------------------------------
  // START SERVER
  // ---------------------------------------------------

  server.begin();

  Serial.println("Web server started.");

  if (WiFi.status() == WL_CONNECTED) {

    Serial.print("Open: http://");
    Serial.println(WiFi.localIP());

  } else {

    Serial.println(
      "WiFi disconnected. Auto reconnect active."
    );
  }
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // Web server must continue working
  server.handleClient();


  // ---------------------------------------------------
  // AUTO WIFI RECONNECT
  // ---------------------------------------------------

  if (
    WiFi.status() != WL_CONNECTED &&
    millis() - lastWifiCheck >= WIFI_RETRY_INTERVAL
  ) {

    lastWifiCheck = millis();

    Serial.println();
    Serial.println("WiFi disconnected.");
    Serial.println("Trying to reconnect...");

    WiFi.disconnect();

    delay(100);

    WiFi.begin(SSID, PASSWORD);
  }


  // ---------------------------------------------------
  // CONNECTION RESTORED
  // ---------------------------------------------------

  if (WiFi.status() == WL_CONNECTED) {

    static bool wasConnected = false;

    if (!wasConnected) {

      wasConnected = true;

      Serial.println();
      Serial.println("WiFi connection restored.");

      Serial.print("IP: ");
      Serial.println(WiFi.localIP());

      // Make absolutely sure relay states
      // remain the saved states.
      relays();
    }

  } else {

    static bool wasConnected = false;

    wasConnected = false;
  }
}
