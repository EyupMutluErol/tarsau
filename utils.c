// utils.c - Dosya kontrolü ve meta bilgi yardımcı fonksiyonları.

#include <stdio.h>
#include <sys/stat.h>

#include "tarsau.h"

// Dosyanın tüm baytlarını okur. Herhangi bir bayt 127'den büyükse ASCII değildir 0 döner. Aksi halde 1
int is_ascii_text(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f) return 0;

    int c;
    while ((c = fgetc(f)) != EOF) {
        if ((unsigned char)c > 127) {
            fclose(f);
            return 0;
        }
    }
    fclose(f);
    return 1;
}

// Başarılıysa byte boyutu, hata varsa -1 döner. 
long get_file_size(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) != 0) return -1;
    return (long)st.st_size;
}

// st_mode'un alt 9 biti (rwx rwx rwx) çıkarılıp döndürülür. 
unsigned int get_file_permissions(const char *filename)
{
    struct stat st;
    if (stat(filename, &st) != 0) return 0;
    return st.st_mode & 0777;
}
