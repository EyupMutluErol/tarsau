// build.c - Birden çok metin dosyasını tek bir .sau arşivinde birleştirir.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tarsau.h"

int build_archive(char *files[], int file_count, const char *output_name)
{
    if (file_count == 0) {
        fprintf(stderr, "Hata: En az bir giriş dosyası belirtmelisiniz.\n");
        return 1;
    }
    if (file_count > MAX_FILES) {
        fprintf(stderr, "Hata: En fazla %d dosya arşivlenebilir.\n", MAX_FILES);
        return 1;
    }

    FileInfo file_info[MAX_FILES];
    long     total_size = 0;

    for (int i = 0; i < file_count; i++) {
        if (!is_ascii_text(files[i])) {
            printf("%s giriş dosyasının formatı uyumsuzdur!\n", files[i]);
            return 1;
        }
        long sz = get_file_size(files[i]);
        if (sz < 0) {
            printf("%s giriş dosyasının formatı uyumsuzdur!\n", files[i]);
            return 1;
        }

        strncpy(file_info[i].name, files[i], MAX_FILENAME - 1);
        file_info[i].name[MAX_FILENAME - 1] = '\0';
        file_info[i].permissions = get_file_permissions(files[i]);
        file_info[i].size        = sz;
        total_size              += sz;
    }

    if (total_size > MAX_SIZE) {
        fprintf(stderr, "Hata: Toplam dosya boyutu 200 MB'ı aşamaz.\n");
        return 1;
    }

    char org_section[MAX_ORG_SECTION];
    org_section[0] = '\0';

    char record[MAX_FILENAME + 64];
    for (int i = 0; i < file_count; i++) {
        if (i > 0) strcat(org_section, "|");
        snprintf(record, sizeof(record), "%s,%o,%ld",
                 file_info[i].name,
                 file_info[i].permissions,
                 file_info[i].size);
        strcat(org_section, record);
    }
    long org_size = (long)strlen(org_section);

    FILE *out = fopen(output_name, "wb");
    if (!out) {
        fprintf(stderr, "Hata: %s dosyası oluşturulamadı.\n", output_name);
        return 1;
    }

    fprintf(out, "%-10ld", org_size);
    fwrite(org_section, 1, (size_t)org_size, out);

    char buffer[BUFFER_SIZE];
    for (int i = 0; i < file_count; i++) {
        FILE *in = fopen(files[i], "rb");
        if (!in) {
            fprintf(stderr, "Hata: %s dosyası açılamadı.\n", files[i]);
            fclose(out);
            return 1;
        }
        size_t n;
        while ((n = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
            fwrite(buffer, 1, n, out);
        }
        fclose(in);
    }
    fclose(out);

    printf("Dosyalar birleştirildi.\n");
    return 0;
}
