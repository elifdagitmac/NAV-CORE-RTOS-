#ifndef KERNEL_H
#define KERNEL_H
// Bu tanımlar dosyanın birden fazla kez include edilmesini engeller. 

#include <stdint.h> 
// Compilerin uint8_t tipini tanıması için kullanmamız gerekli. (Bellek tasarrufu için bu tipi kullandık.)

typedef void (*GorevFonksiyonu) ();
// Bu satır sayesinde sisteme eklenecek her görevin aynı tipte olmasını sağladık. (Standartlaştırma yaptık.)
// Bu parametre almayan, değer döndürmeyen bir fonksiyon olarak tanımlanmıştır. 

struct TCB {
    uint8_t id; // Görevin kimlik numarası. main.c dosyasında manuel olarak belirleyeceğiz.
    uint8_t priority; // Görevin öncelik sırası. (0 en yüksek)
    GorevFonksiyonu gorev_adres; // Fonksiyonun adresi.
    struct TCB *siradaki_gorev; // Linked List yapısında sonraki halka. Öncelik sırasına göre devam eder.


};

void nav_core_init(); //Programın başında 1 kez çalışır sadece, sistemin ihtiyaç duyduğu hazırlıkları yapar.
void gorev_ekle (struct TCB *eklenecek_gorev_adresi); /*Muhtemelen diğer dosyaların içerisinde var olan bir görevin adresine alıp 
Lİnked List yapısında uygun bir yere ekler.*/
void scheduler(); //Görevler arasındaki geçiş yönetimini yapan fonksiyon.

//extern dosyaları hafızada yer kaplamaz sadece dosyalara değişkenleri ilan eder.
extern struct TCB *head;/* Linked List yapısındaki başlangıç fonksiyonunun adres etiketi. 
Asıl görev başka dosyada burada sadece başlangıç adresini tüm dosyalarla paylaşıyoruz.*/
extern struct TCB *su_an_calisan; /*Sistemin o anda çalıştırdığı fonksiyonun adresini tüm dosyalarla paylaşan fonksiyon.*/

#endif






