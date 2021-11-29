#include <ArduinoJson.h>
#include <WiFi.h>

const char* ssid = "tadel_5G";
const char* password = "happy108";
const char* host = "api.openweathermap.org";
const char* resource = "/data/2.5/weather?id=1668341&appid=a01bda985f7e8fd44912f588ed54ea6a";
char jsonRead[600]; //讀取response後儲存JSON資料的變數，必須是全域變數

const char* linehost = "maker.ifttt.com"; //IFTTT server網址
const char* event = "line_notify";  //IFTTT事件名稱
const char* apiKey = "qDM71o3mQ5s-uqDP5lOsR";  //IFTTT Applet key 

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
}

void loop(){

    //以char*格式儲存回傳的json
    char* json = GetWeatherData();
    
    Serial.println(json);   
    StaticJsonDocument<600> doc;
    deserializeJson(doc, json);
    const char* location = doc["name"]; 
    String weather = doc["weather"][0]["description"];
    String weatherfix="";
    double temp = doc["main"]["temp"];
    double humidity = doc["main"]["humidity"];
    for(int i=0;i<weather.length();i++){
     if(weather[i]!=' ') weatherfix+=weather[i];
     else if(weather[i]==' ')weatherfix+="%20";
      }
     Serial.print("*** ");
    Serial.print(location);
    Serial.println(" ***");
    Serial.print("Type: ");
    Serial.println(weatherfix);
    Serial.print("Temp: ");
    Serial.print(temp - 273);
    Serial.println("C");
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
    Serial.println("--------------------"); 
   
  Lineweather(String(weatherfix), String(temp - 273), String(humidity));  //Client傳送資料
  
  delay(3000);
}

char* GetWeatherData(){
  
  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
  String JsonString = "";  //此範例不會用到

  //Client連結Server
  if (client.connect(host, httpPort)) {

    //Client傳送
    client.println(String("POST ") + resource + " HTTP/1.1");
    client.println(String("Host: ") + host); 
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料  
    int timeout = 0;
    while(!client.available() && (timeout++ <= 50)){
      delay(100);
    }
  
    //如果無回應
    if(!client.available()) {
      Serial.println("No response...");
    }

    //Checking for the end of HTTP stream
    while(!client.find("\r\n\r\n")){
      // wait for finishing header stream reading ...
    }

    //讀取資料並儲存在jsonRead中
    client.readBytes(jsonRead, 600);

    //停止Client
    client.stop(); 

    //回傳
    return jsonRead;
  } 
}

void Lineweather(String value1, String value2, String value3){
  
  WiFiClient client;  //建立Client物件
  const int httpPort = 80;  //預設通訊阜80
   String url = "";
  String JsonString = "";  //此範例不會用到

  //Client連結Server
  if (client.connect(linehost,httpPort)) {
    
    //Webhook API
     url += "/trigger/" + String(event) + "/with/key/" + String(apiKey);
    //Query String
    url += "?value1=" + value1 + "&value2=" + value2+"&value3=" + value3;

    //Client傳送
    client.println(String("POST ") + url + " HTTP/1.1");
    client.println(String("Host: ") + linehost); 
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(JsonString.length());
    client.println();
    client.println(JsonString);

    //等待5秒，每0.1秒偵測一次是否有接收到response資料  
    int timeout = 0;
    while(!client.available() && (timeout++ <= 50)){
      delay(100);
    }
  
    //如果無回應
    if(!client.available()) {
      Serial.println("No response...");
    }
    //用while迴圈一字一字讀取Response
    while(client.available()){
      Serial.write(client.read());
    }

    //停止Client
    client.stop(); 
  } 
  }
