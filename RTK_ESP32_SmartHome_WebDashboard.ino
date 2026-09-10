#include <WiFi.h>
#include <WebServer.h>

const char* SSID = "RTkwifi";
const char* PASS = "00990099";

#define ON  LOW
#define OFF HIGH

WebServer server(80);

const byte pin[4] = {5,17,16,4};
bool state[4] = {0,0,0,0};

unsigned long lastReconnect = 0;

void allOff(){
  for(byte i=0;i<4;i++){
    state[i]=0;
    digitalWrite(pin[i],OFF);
  }
}

String json(){
  String j="[";
  for(byte i=0;i<4;i++){
    j+=state[i];
    if(i<3)j+=",";
  }
  return j+"]";
}

void api(){
  int r=server.arg("r").toInt();
  int v=server.arg("v").toInt();

  if(r>=0 && r<4){
    state[r]=v;
    digitalWrite(pin[r],v?ON:OFF);
  }

  server.send(200,"application/json",json());
}

void all(){
  bool v=server.arg("v").toInt();

  for(byte i=0;i<4;i++){
    state[i]=v;
    digitalWrite(pin[i],v?ON:OFF);
  }

  server.send(200,"application/json",json());
}

void home(){
  server.send(200,"text/html",R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>RTK DEVIL</title>
<style>
*{box-sizing:border-box}
body{
margin:0;background:#090d12;color:#fff;
font-family:Arial;padding:18px
}
main{max-width:430px;margin:auto}
header{text-align:center;padding:8px 0 18px}
h1{margin:0;color:#58a6ff;font-size:25px}
p{color:#8993a0;font-size:12px}
.card{
background:#151b23;border:1px solid #29313d;
border-radius:16px;padding:17px;margin:10px 0;
display:flex;justify-content:space-between;align-items:center
}
.name{font-weight:bold;font-size:16px}
.gpio{font-size:11px;color:#7d8793;margin-top:4px}
.switch{width:55px;height:30px;position:relative}
.switch input{display:none}
.slider{
position:absolute;inset:0;background:#303844;
border-radius:30px;transition:.2s
}
.slider:before{
content:"";position:absolute;width:22px;height:22px;
left:4px;top:4px;background:#fff;
border-radius:50%;transition:.2s
}
input:checked+.slider{background:#18a558}
input:checked+.slider:before{transform:translateX(25px)}
.actions{display:flex;gap:10px;margin-top:14px}
button{
flex:1;border:0;border-radius:13px;padding:14px;
font-weight:bold;color:#fff
}
.on{background:#168443}
.off{background:#d33b3b}
footer{text-align:center;color:#59636f;font-size:10px;margin-top:18px}
</style>
</head>

<body>
<main>

<header>
<h1>RTK DEVIL</h1>
<p>IoT • Smart Control Department</p>
</header>

<div class="card">
<div><div class="name">Fan</div><div class="gpio">GPIO 5 • Relay 1</div></div>
<label class="switch">
<input id="r0" type="checkbox" onchange="setR(0)">
<span class="slider"></span>
</label>
</div>

<div class="card">
<div><div class="name">Relay 2</div><div class="gpio">GPIO 17</div></div>
<label class="switch">
<input id="r1" type="checkbox" onchange="setR(1)">
<span class="slider"></span>
</label>
</div>

<div class="card">
<div><div class="name">Relay 3</div><div class="gpio">GPIO 16</div></div>
<label class="switch">
<input id="r2" type="checkbox" onchange="setR(2)">
<span class="slider"></span>
</label>
</div>

<div class="card">
<div><div class="name">Relay 4</div><div class="gpio">GPIO 4</div></div>
<label class="switch">
<input id="r3" type="checkbox" onchange="setR(3)">
<span class="slider"></span>
</label>
</div>

<div class="actions">
<button class="on" onclick="all(1)">ALL ON</button>
<button class="off" onclick="all(0)">ALL OFF</button>
</div>

<footer>RTK DEVIL DEPARTMENT • ESP32 CONTROL</footer>

</main>

<script>
function ui(a){
for(let i=0;i<4;i++)
document.getElementById("r"+i).checked=a[i];
}

function setR(i){
let v=document.getElementById("r"+i).checked?1:0;
fetch("/set?r="+i+"&v="+v)
.then(x=>x.json()).then(ui).catch(()=>{});
}

function all(v){
fetch("/all?v="+v)
.then(x=>x.json()).then(ui).catch(()=>{});
}

function sync(){
fetch("/state")
.then(x=>x.json()).then(ui).catch(()=>{});
}

sync();
setInterval(sync,1500);
</script>

</body>
</html>
)rawliteral");
}

void setup(){

  for(byte i=0;i<4;i++)
    pinMode(pin[i],OUTPUT);

  allOff();

  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  WiFi.begin(SSID,PASS);

  server.on("/",home);
  server.on("/set",api);
  server.on("/all",all);

  server.on("/state",[](){
    server.send(200,"application/json",json());
  });

  server.begin();
}

void loop(){

  server.handleClient();

  if(WiFi.status()!=WL_CONNECTED){

    // Safety: Wi-Fi gaya = sab relay OFF
    allOff();

    // Har 5 second mein fresh reconnect attempt
    if(millis()-lastReconnect > 5000){
      lastReconnect=millis();

      WiFi.disconnect();
      delay(100);
      WiFi.begin(SSID,PASS);
    }
  }
}
