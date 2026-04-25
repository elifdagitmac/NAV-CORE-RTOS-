#include "kernel.h"
#include "tasks.h"
#include <Arduino.h>

/* 1. blok
 * Görev TCB (Kimlik Kartı) Tanımlamaları 
 * Bu yapılar main.c tarafında kernel listesine eklenecek.
 */
struct TCB radar_gorevi;
struct TCB manevra_gorevi;
struct TCB monitor_gorevi;

//2. blok
// --- DONANIM PİNLERİ ---
#define TRIG_PIN 12
#define ECHO_PIN 11
#define PERVANE_PWM 6   // Pervane hızı
#define MANEVRA_YON_1 5  // Motor yön pini 1
#define MANEVRA_YON_2 4  // Motor yön pini 2
#define RED_LED 7        // Tehlike göstergesi
#define BLUE_LED 13       // Sistem "Heartbeat" LED'i

//3. blok
// --- GÖREVLER ARASI HABERLEŞME (Global Bayrak) ---
volatile bool engel_durumu = false; 

/** 4. blok
 * GÖREV 1: RADAR TARAMA
 * Radar sensörü ile mesafe ölçer ve engel varsa bayrağı kaldırır.
 */
void Task_RadarScan(void *pvParameters) {
    while(1) {
        // Sensör tetikleme
        digitalWrite(TRIG_PIN, LOW);
        delayMicroseconds(2); //sensörü bir sonraki gelecek olan komuta hazırlar, temizlik için kısa bir LOW sinyali
        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);//ses dalgasının gönderilmesi için 10 mikro saniye HIGH sinyali
        digitalWrite(TRIG_PIN, LOW);
        
        // Yankı süresini oku ve mesafeye çevir
        long sure = pulseIn(ECHO_PIN, HIGH);
        float mesafe = sure * 0.034 / 2;

        // Engel kontrolü (15 cm kritik sınır)
        if(mesafe > 0 && mesafe < 15) { 
            engel_durumu = true;
            digitalWrite(RED_LED, HIGH);
        } else {
            engel_durumu = false;
            digitalWrite(RED_LED, LOW);
        }
        
        // Kernel'a 50ms dinlenme süresi ver
        radar_gorevi.delay_ms = 50; 
    }
}

/** 5. blok
 * GÖREV 2: MANEVRA VE PERVANE
 * "engel_durumu" bilgisine göre motorları sürer.
 */
void Task_ManeuverControl(void *pvParameters) {
    while(1) {
        if(engel_durumu) {
            // ENGEL VAR: Pervaneyi yavaşlat ve ters yöne manevra yap
            digitalWrite(MANEVRA_YON_1, LOW);
            digitalWrite(MANEVRA_YON_2, HIGH); 
            analogWrite(PERVANE_PWM, 100); // %40 güçle manevra
        } else {
            // YOL AÇIK: Pervane tam güç, ileri seyir
            digitalWrite(MANEVRA_YON_1, HIGH);
            digitalWrite(MANEVRA_YON_2, LOW);
            analogWrite(PERVANE_PWM, 200); // %80 güçle seyir
        }
        
        // Motor tepki süresi için 100ms periyot
        manevra_gorevi.delay_ms = 100;
    }
}

/** 6. blok
 * GÖREV 3: SİSTEM MONİTÖRÜ
 * Sistemin genel durumunu ve zaman bilgisini seri porttan basar.
 */
void Task_SystemMonitor(void *pvParameters) {
    while(1) {
        // Heartbeat LED (Sistemin donmadığını gösterir)
        digitalWrite(BLUE_LED, !digitalRead(BLUE_LED));

        Serial.println("---------- NAV-CORE STATUS ----------");
        Serial.print("Sistem Zamani: "); Serial.print(millis()); Serial.println(" ms");
        Serial.print("Engel Durumu: "); Serial.println(engel_durumu ? "TEHLIKE!" : "GÜVENLI");
        Serial.println("-------------------------------------");

        // Çok sık basıp seri portu yormayalım (1 saniye)
        monitor_gorevi.delay_ms = 1000;
    }
}