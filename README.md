# tarsau — Sıkıştırmasız Arşivleme Programı

Bilgisayar Mühendisliği — Sistem Programlama Dersi 2025-2026 Bahar Dönemi Projesi

## Derleme

```bash
make
```

`make clean` ile üretilen dosyaları temizleyebilir, `make test` ile hızlı bir kendi-kendine test çalıştırabilirsiniz. Daha kapsamlı senaryolar için `./test.sh` betiğini de kullanabilirsiniz.

## Kullanım

### 1) Arşivleme (`-b`)

```bash
./tarsau -b dosya1 dosya2 dosya3 ... [-o cikti.sau]
```

- Giriş dosyalarının tamamı **ASCII metin** olmalıdır.
- En fazla **32 dosya**, toplamda en fazla **200 MB** kabul edilir.
- `-o` belirtilmezse çıktı adı varsayılan olarak **`a.sau`** olur.
- Bir dosya ASCII değilse program `"<dosya> giriş dosyasının formatı uyumsuzdur!"` yazıp sorunsuz çıkar.

Örnek:
```bash
./tarsau -b t1 t2 t3 t4.txt t5.dat -o s1.sau
```

### 2) Açma (`-a`)

```bash
./tarsau -a arsiv.sau [dizin]
```

- Birinci parametre `*.sau` arşiv dosyasıdır. Geçersiz/bozuk olursa `"Arşiv dosyası uygunsuz veya bozuk!"` mesajı verilir.
- İkinci parametre **opsiyoneldir**. Verilmezse dosyalar mevcut dizine açılır.
- Dizin yoksa otomatik oluşturulur. Göreceli veya mutlak yol kabul edilir.
- Açılan dosyalar **orijinal izinlerini korur**.

Örnek:
```bash
./tarsau -a s1.sau d1
```

## .sau Dosya Formatı

```
+---------------------+--------------------------------------+----------------------+
| 10 bayt: org boyutu | Organizasyon kayıtları (org_boyutu)  | Dosya içerikleri     |
| (ASCII, boşluk pad) | "ad1,izin,boyut|ad2,izin,boyut|..."  | (arka arkaya, ASCII) |
+---------------------+--------------------------------------+----------------------+
```

- İlk 10 bayt: organizasyon bölümünün boyutunu ASCII rakamlarla içerir (sağa boşluk dolgulu).
- Organizasyon bölümünde her kayıt `|` ile, kayıt içindeki alanlar `,` ile ayrılır.
- Dosya içerikleri kayıtların hemen ardından, **ayraçsız** olarak peş peşe yazılır.

## Dosya Yapısı

```
.
├── Makefile      # Derleme kuralları
├── README.md     # Bu dosya
├── tarsau.h      # Ortak sabitler, FileInfo struct ve fonksiyon bildirimleri
├── tarsau.c      # main() ve argüman ayrıştırma
├── utils.c       # Yardımcı fonksiyonlar (is_ascii_text, get_file_size, get_file_permissions)
├── build.c       # Arşivleme fonksiyonu (build_archive)
├── extract.c     # Açma fonksiyonu (extract_archive)
└── test.sh       # Kapsamlı test betiği
```

Proje, sorumlulukların net ayrımı için modüler yapıda tasarlanmıştır:

- **`tarsau.h`** — tüm modüller arasında paylaşılan sabitler, veri yapıları ve fonksiyon bildirimleri.
- **`utils.c`** — dosya bilgisi okuma ve ASCII format kontrolü.
- **`build.c`** — `-b` modu, dosyaları `.sau` formatında birleştirir.
- **`extract.c`** — `-a` modu, `.sau` arşivini açar ve izinleri geri yükler.
- **`tarsau.c`** — komut satırı argümanlarını ayrıştırır, ilgili modüle yönlendirir.
