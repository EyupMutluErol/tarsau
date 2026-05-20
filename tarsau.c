/* tarsau.c - Ana giriş noktası. Komut satırı argümanlarını ayrıştırıp
build.c'deki build_archive veya extract.c'deki extract_archive
fonksiyonlarını çağırır.*/

#include <stdio.h>
#include <string.h>

#include "tarsau.h"

static void print_usage(const char *prog)
{
    fprintf(stderr, "Kullanım:\n");
    fprintf(stderr, "  %s -b dosya1 dosya2 ... [-o cikti.sau]\n", prog);
    fprintf(stderr, "  %s -a arsiv.sau [dizin]\n", prog);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // -b : Arşivleme modu 
    if (strcmp(argv[1], "-b") == 0) {
        char *files[MAX_FILES];
        int   file_count = 0;
        char *output     = "a.sau";   // Varsayılan çıktı adı 

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-o") == 0) {
                if (i + 1 < argc) {
                    output = argv[++i];
                } else {
                    fprintf(stderr, "Hata: -o parametresinden sonra "
                                    "dosya adı belirtilmelidir.\n");
                    return 1;
                }
            } else {
                if (file_count >= MAX_FILES) {
                    fprintf(stderr, "Hata: En fazla %d dosya işlenebilir.\n",
                            MAX_FILES);
                    return 1;
                }
                files[file_count++] = argv[i];
            }
        }
        return build_archive(files, file_count, output);
    }

    // -a : Açma modu 
    if (strcmp(argv[1], "-a") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Hata: -a parametresinden sonra arşiv dosyası "
                            "belirtilmelidir.\n");
            return 1;
        }
        if (argc > 4) {
            fprintf(stderr, "Hata: -a parametresinden sonra en fazla 2 "
                            "parametre verilebilir.\n");
            return 1;
        }
        const char *archive   = argv[2];
        const char *directory = (argc >= 4) ? argv[3] : NULL;
        return extract_archive(archive, directory);
    }

    fprintf(stderr, "Hata: Geçersiz parametre. -b veya -a kullanın.\n");
    print_usage(argv[0]);
    return 1;
}
