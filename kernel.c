#include "kernel.h"
#include <stddef.h> 

/* --- Küresel Değişkenlerin Fiziksel Tanımı (Bellekte Yer Açma) --- */
struct TCB *head = NULL; 
struct TCB *su_an_calisan = NULL;

/* --- Fonksiyon İçerikleri --- */

void nav_core_init() {
    head = NULL;
    su_an_calisan = NULL;
}

void gorev_ekle(struct TCB *eklenecek_gorev_adresi) {
    if (head == NULL || eklenecek_gorev_adresi->priority < head->priority) {
        eklenecek_gorev_adresi->siradaki_gorev = head;
        head = eklenecek_gorev_adresi;
    } else {
        struct TCB *gecici = head;
        while (gecici->siradaki_gorev != NULL && gecici->siradaki_gorev->priority <= eklenecek_gorev_adresi->priority) {
            gecici = gecici->siradaki_gorev;
        }
        eklenecek_gorev_adresi->siradaki_gorev = gecici->siradaki_gorev;
        gecici->siradaki_gorev = eklenecek_gorev_adresi;
    }
}

void scheduler() {
    if (head != NULL) {
        su_an_calisan = head;
        su_an_calisan->gorev_adres();
    }
}

