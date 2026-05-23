// extract.c - Bir .sau arşivini açar.

#define _POSIX_C_SOURCE 200809L   

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "tarsau.h"

int extract_archive(const char *archive_name, const char *directory)
{
    //.sau uzantısı zorunlu 
    const char *ext = strrchr(archive_name, '.');
    if (!ext || strcmp(ext, ".sau") != 0) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        return 1;
    }

    FILE *in = fopen(archive_name, "rb");
    if (!in) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        return 1;
    }

    // İlk 10 baytı okuyoruz . organizasyon bölümünün boyutu 
    char header_prefix[HEADER_PREFIX_SIZE + 1];
    if (fread(header_prefix, 1, HEADER_PREFIX_SIZE, in) != HEADER_PREFIX_SIZE) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        fclose(in);
        return 1;
    }
    header_prefix[HEADER_PREFIX_SIZE] = '\0';

    long org_size = atol(header_prefix);
    if (org_size <= 0 || org_size > MAX_ORG_SECTION) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        fclose(in);
        return 1;
    }

    // Organizasyon bölümünü okuyoruz 
    char *org_section = (char*)malloc((size_t)org_size + 1);
    if (!org_section) {
        fclose(in);
        return 1;
    }
    if (fread(org_section, 1, (size_t)org_size, in) != (size_t)org_size) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        free(org_section);
        fclose(in);
        return 1;
    }
    org_section[org_size] = '\0';

    if (directory != NULL && strlen(directory) > 0) {
        struct stat st;
        if (stat(directory, &st) != 0) {
            if (mkdir(directory, 0755) != 0) {
                fprintf(stderr, "Hata: %s dizini oluşturulamadı: %s\n",
                        directory, strerror(errno));
                free(org_section);
                fclose(in);
                return 1;
            }
        }
    }

    FileInfo files[MAX_FILES];
    int      file_count = 0;

    char *saveptr1;
    char *token = strtok_r(org_section, "|", &saveptr1);
    while (token != NULL && file_count < MAX_FILES) {
        char *saveptr2;
        char *name     = strtok_r(token, ",", &saveptr2);
        char *perm_str = strtok_r(NULL,  ",", &saveptr2);
        char *size_str = strtok_r(NULL,  ",", &saveptr2);

        if (!name || !perm_str || !size_str) {
            printf("Arşiv dosyası uygunsuz veya bozuk!\n");
            free(org_section);
            fclose(in);
            return 1;
        }

        strncpy(files[file_count].name, name, MAX_FILENAME - 1);
        files[file_count].name[MAX_FILENAME - 1] = '\0';
        sscanf(perm_str, "%o", &files[file_count].permissions);
        files[file_count].size = atol(size_str);
        file_count++;

        token = strtok_r(NULL, "|", &saveptr1);
    }

    char filepath[MAX_FILENAME * 2 + 16];
    char buffer[BUFFER_SIZE];

    for (int i = 0; i < file_count; i++) {
        if (directory != NULL && strlen(directory) > 0)
            snprintf(filepath, sizeof(filepath), "%s/%s", directory, files[i].name);
        else
            snprintf(filepath, sizeof(filepath), "%s", files[i].name);

        FILE *out = fopen(filepath, "wb");
        if (!out) {
            fprintf(stderr, "Hata: %s dosyası oluşturulamadı.\n", filepath);
            long remaining = files[i].size;
            while (remaining > 0) {
                size_t to_read = (remaining > BUFFER_SIZE) ? BUFFER_SIZE
                                                           : (size_t)remaining;
                size_t n = fread(buffer, 1, to_read, in);
                if (n == 0) break;
                remaining -= (long)n;
            }
            continue;
        }

        long remaining = files[i].size;
        while (remaining > 0) {
            size_t to_read = (remaining > BUFFER_SIZE) ? BUFFER_SIZE
                                                       : (size_t)remaining;
            size_t n = fread(buffer, 1, to_read, in);
            if (n == 0) break;
            fwrite(buffer, 1, n, out);
            remaining -= (long)n;
        }
        fclose(out);

        chmod(filepath, files[i].permissions);
    }

    if (directory != NULL && strlen(directory) > 0)
        printf("%s dizininde ", directory);

    for (int i = 0; i < file_count; i++) {
        printf("%s", files[i].name);
        if (file_count >= 2 && i == file_count - 2)
            printf(" ve ");
        else if (i < file_count - 2)
            printf(", ");
    }
    printf(" dosyaları açıldı.\n");

    free(org_section);
    fclose(in);
    return 0;
}
