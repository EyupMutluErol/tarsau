// tarsau.h - Ortak sabitler, veri yapıları ve fonksiyon bildirimleri.
// Tüm .c dosyaları bu başlığı include eder.

#ifndef TARSAU_H
#define TARSAU_H

// Proje sabitleri 
#define MAX_FILES          32                   // En fazla 32 giriş dosyası      
#define MAX_SIZE           (200L * 1024 * 1024) // En fazla 200 MB toplam boyut   
#define MAX_FILENAME       256                  // Maksimum dosya adı uzunluğu    
#define HEADER_PREFIX_SIZE 10                   // Org. bölümü boyut başlığı (10B)
#define BUFFER_SIZE        8192                 // Okuma/yazma tampon boyutu      
#define MAX_ORG_SECTION    (MAX_FILES * 512)    // Organizasyon bölümü buffer'ı   

// Veri yapıları
typedef struct {
    char         name[MAX_FILENAME];  // Dosya adı                 
    unsigned int permissions;         // Dosya izinleri (oktal)    
    long         size;                // Dosya boyutu (byte)       
} FileInfo;

// utils.c'de tanımlı yardımcı fonksiyonlar 
int          is_ascii_text(const char *filename);
long         get_file_size(const char *filename);
unsigned int get_file_permissions(const char *filename);

// build.c'de tanımlı arşivleme fonksiyonu 
int build_archive(char *files[], int file_count, const char *output_name);

// extract.c'de tanımlı açma fonksiyonu 
int extract_archive(const char *archive_name, const char *directory);

#endif 
