#include "kernel.h"
#include <stdlib.h>

struct TCB *head=NULL;
struct TCB *su_an_calisan=NULL;
//Sistem ilk açıldığında RAM'de kayıtı ve o an çalışan hiçbir görev yoktur. 
/*ilk olarak bu iki değişkeni NULL tanımlamamızın nedeni: ilk önce değişkenler için RAM de yer açıyoruz (değişkenlerin adreslerini tutmak için bir boş kağıt almak gibi)
ve daha sonrasında bu deeğişkenlere atama yapacağız. */

void nav_core_init (void){
    head=NULL;
    su_an_calisan=NULL;
}
/*Bu fonksiyon ilk kurulumda ve yazılımsal reset gerektiğinde devreye girer. Çalışan bir sistemi kapatıp yeniden başlatmak 
istediğimizde eski görevlerin adreslerinin kalmaması içi değişkenleri yeniden NULL yptık.*/


//listeye yeni eklenen bir görevi önceliğine göre yerleştirdik.
void gorev_ekle (struct TCB *eklenecek_gorev_adresi){
    if (head==NULL || eklenecek_gorev_adresi->priority < head->priority){
        eklenecek_gorev_adresi->siradaki_gorev=head;
        head=eklenecek_gorev_adresi;
 }

 
    else {
    struct TCB *temporary_pointer=head;
    while (temporary_pointer->siradaki_gorev!=NULL && temporary_pointer->siradaki_gorev->priority<=eklenecek_gorev_adresi->priority)
    {temporary_pointer=temporary_pointer->siradaki_gorev;}
    eklenecek_gorev_adresi->siradaki_gorev=temporary_pointer->siradaki_gorev;
    temporary_pointer->siradaki_gorev=eklenecek_gorev_adresi;
    }

}

//görev silme algoritması

void gorev_sil(uint8_t silinecek_id){
     if (head==NULL) return;


     //head silinecekse 
   
    if (head->id == silinecek_id){
      struct TCB *silinecek_gorev=head;
      head=silinecek_gorev->siradaki_gorev;
      free(silinecek_gorev);
    return;}
   
    //aradaki bir görev silinecekse.
    struct TCB *temporary_pointer=head;
    while (temporary_pointer->siradaki_gorev != NULL && temporary_pointer->siradaki_gorev->id != silinecek_id){
        temporary_pointer=temporary_pointer->siradaki_gorev;
    }
    if (temporary_pointer->siradaki_gorev != NULL) {
        struct TCB *silinecek_gorev = temporary_pointer->siradaki_gorev;
        temporary_pointer->siradaki_gorev = silinecek_gorev->siradaki_gorev;
        free(silinecek_gorev);
    }

    }

//sistemde o an çalışacak hiçbir görev yoksa yoksa bu görevi çalışacak böylece sistemin çökmesini (hard fault) engelledik. (veri bekleme ve veri gönderme anlarında bu görev devreye girebilir.)
void idle_task (void){
    while (1){

    }
}
// sistemin zamanını tuttuk. listenin başından sonuna tüm görevleri gezer ve bekleme sürelerini günceller. 
//fake_head ile listenin başından başlayarak tüm görevleri tek tek kontrol ediyoruz. Eğer bir görevin bekleme süresi 0'dan büyükse, o görevin bekleme süresini 1 azaltıyoruz. 
//Bu işlem her ms de bir kez yapılır, böylece görevlerin zamanlaması doğru şekilde yönetilir.
void nav_core_tick (void){
    struct TCB *fake_head=head;
    while (fake_head!= NULL) {
        if (fake_head->delay_ms > 0 ){
            fake_head->delay_ms--;
        }
     }
}

/* listeyi en acilden başlayıp tarayarak, çalışmaya hazır (beklemede olmayan) görevi bulup onu aktif eder */
void schedular (void) {
    struct TCB *fake_head=head;
    while (fake_head != NULL) {
        if (fake_head->delay_ms == 0) {
            su_an_calisan = fake_head ;
            su_an_calisan->gorev_fonksiyonu();
            return;
        } 
        fake_head=fake_head->siradaki_gorev;
    }

}
/* Arama işlemine head ile başlar. Oan bakılan görevde delay==0 mı diye kontrol eder. Eğer delay=0 ise görevi su_an_calisan olarak ayarlar ve 
daha düşük öncelikli görevlere bakmaya gerek kalmadan programdan çıkar. Eğer görev hala bekliyorsa o görevi atlar ve bir sonraki göreve geçip yine delay 
kontrolü yapar. Eğer tüm liste tarandıysa ve çalışacak hiçbir görevi bulamadıysa idle_task çalışır. */