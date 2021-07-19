#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#define FIREBASE_HOST " "  //wifi ve firebase sunucu ayarları
#define FIREBASE_AUTH " "
#define WIFI_SSID " "
#define WIFI_PASSWORD " "
#include "DHTesp.h"
DHTesp dht;
unsigned long int gecen_zaman;
unsigned long int ayarlanan_zaman;
unsigned long int yeni_milis;

struct yeniMillis{ //millis(mcu nun çalışma süresi) fonksiyonun reset yapabilmesi için oluşturulmuş bir struct
  unsigned long offset =0;
  unsigned long milis(){ //millis ayarla
    return yeni_milis=millis()-offset; 
    }
    void sifirla (){ //millis i sıfırla
      offset = millis();
    }
}
yeni;

void setup() {
//  Serial.begin(9600); // serial printden değişkenleri görmek için (isteğe bağlı)
  
  dht.setup(D4, DHTesp:: DHT11);
  pinMode(D8,OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
/*  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }*/
/*   Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());*/
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

int n = 0;

void loop() {
  
  gecen_zaman=yeni.milis();
  
  float hum= dht.getHumidity();
  float temp= dht.getTemperature();
  int gaz= analogRead(A0);
  bool lamp=digitalRead(D8);
/*   Serial.print("hum: ");Serial.println(hum);
  Serial.print("temp: ");Serial.println(temp);
  Serial.print("gaz: ");Serial.println(gaz); */

  Firebase.setFloat("hum", hum); //float tipi değişken gönderme
  /*   if (Firebase.failed()) {
      Serial.print("setting /hum failed:");
      Serial.println(Firebase.error()); 
  }*/
  
  Firebase.setFloat("temp", temp);
   /*  if (Firebase.failed()) {
      Serial.print("setting /temp failed:");
      Serial.println(Firebase.error()); 
  }*/

  Firebase.setInt("gaz", gaz);
 /*   if (Firebase.failed()) {
      Serial.print("setting /gas failed:");
      Serial.println(Firebase.error()); 
  } */

   Firebase.setBool("lamba", lamp);
 /*     if (Firebase.failed()) {
      Serial.print("setting /lamp failed:");
      Serial.println(Firebase.error()); 
  }*/
 String lambadurum =Firebase.getString("lamp"); //string tipi değişken alma
 String gaz_alarm =Firebase.getString("gaz_alarm");
 String sicaklik_alarmi =Firebase.getString("sicaklik_alarmi");
 String zaman =Firebase.getString("zaman");
 String zamanlayici =Firebase.getString("zamanlayici");
 

if(zamanlayici=="0"){ //zamalayıcı kapalıysa milis'i  sıfırla
 yeni.sifirla();
}
else{
 ayarlanan_zaman=zaman.toInt(); //ayarlanan zaman string tipinde karşılaştırılma yapılması için integer olmalı
 ayarlanan_zaman=ayarlanan_zaman*1000;  
}

 
/* Serial.print("Gaz alarm=");Serial.println(gaz_alarm);
 Serial.print("lambadurum=");Serial.println(lambadurum);
 Serial.print("Sıcaklık alarmı=");Serial.println(sicaklik_alarmi);

 Serial.print("zamanlayici=");Serial.println(zamanlayici);
 Serial.print("millis degeri=");Serial.println(gecen_zaman);
 Serial.print("gonderilen lamba verisi=");Serial.println(lamp);
 Serial.print("Ayarlanan zaman=");Serial.println(ayarlanan_zaman);*/

//gaz alarmı 200 e ayarlı, sıcaklık 32 dereceye ayarlı, zamanlayıcı millis değerinden az olduğuna ayarlı
if((lambadurum=="1") | (gaz>200 & gaz_alarm=="1" ) | (temp>=32 & sicaklik_alarmi=="1" ) | (zamanlayici=="1" & (gecen_zaman>ayarlanan_zaman)))
{
 digitalWrite(D8,HIGH);
 }
 else {
 digitalWrite(D8,LOW);
 }
}
