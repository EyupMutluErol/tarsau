# test.sh - tarsau projesi için kapsamlı test betiği

# Renkler (görselleştirme için)
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # Renksiz

PASS=0
FAIL=0

# Yardımcı: test sonucunu yazdırır
check() {
    local desc="$1"
    local expected="$2"
    local actual="$3"
    if [[ "$actual" == *"$expected"* ]]; then
        echo -e "  ${GREEN}✓ BAŞARILI${NC}: $desc"
        PASS=$((PASS+1))
    else
        echo -e "  ${RED}✗ BAŞARISIZ${NC}: $desc"
        echo "    Beklenen: $expected"
        echo "    Gerçek  : $actual"
        FAIL=$((FAIL+1))
    fi
}

# Derlenmiş mi kontrol ediyoruz
if [[ ! -x ./tarsau ]]; then
    echo -e "${RED}Hata: ./tarsau bulunamadı. Önce 'make' çalıştırın.${NC}"
    exit 1
fi

# Temiz çalışma alanı
rm -rf test_workspace
mkdir test_workspace
cd test_workspace
cp ../tarsau .

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  tarsau Test Süiti${NC}"
echo -e "${BLUE}========================================${NC}"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 1] Şartnamedeki örnek: 5 dosya, farklı izinler${NC}"
echo "ilk dosya icerigi"     > t1
echo "ikinci dosya icerigi"  > t2
echo "ucuncu dosya icerigi"  > t3
echo "dorduncu dosya"        > t4.txt
echo "besinci dosya"         > t5.dat
chmod 755 t4.txt
chmod 600 t5.dat

out=$(./tarsau -b t1 t2 t3 t4.txt t5.dat -o s1.sau 2>&1)
check "Arşivleme" "Dosyalar birleştirildi." "$out"

out=$(./tarsau -a s1.sau d1 2>&1)
check "Açma" "d1 dizininde t1, t2, t3, t4.txt ve t5.dat dosyaları açıldı." "$out"

# İzin korunması kontrolü
perm_t4=$(stat -c %a d1/t4.txt)
perm_t5=$(stat -c %a d1/t5.dat)
check "t4.txt izin korunması (755)" "755" "$perm_t4"
check "t5.dat izin korunması (600)" "600" "$perm_t5"

# İçerik aynı mı?
diff t1 d1/t1 > /dev/null && check "t1 içeriği aynı" "OK" "OK" || check "t1 içeriği aynı" "OK" "FARK"
diff t5.dat d1/t5.dat > /dev/null && check "t5.dat içeriği aynı" "OK" "OK" || check "t5.dat içeriği aynı" "OK" "FARK"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 2] Varsayılan çıktı adı (-o verilmezse a.sau)${NC}"
rm -f a.sau
out=$(./tarsau -b t1 t2 2>&1)
check "Arşivleme başarılı" "Dosyalar birleştirildi." "$out"
[[ -f a.sau ]] && check "a.sau oluşturuldu" "VAR" "VAR" || check "a.sau oluşturuldu" "VAR" "YOK"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 3] ASCII olmayan dosya reddedilmeli${NC}"
printf 'merhaba\200\220dunya' > binary.bin
out=$(./tarsau -b t1 binary.bin -o err.sau 2>&1)
check "ASCII hatası mesajı" "binary.bin giriş dosyasının formatı uyumsuzdur!" "$out"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 4] Yanlış uzantı (.sau değil)${NC}"
out=$(./tarsau -a t1 d2 2>&1)
check "Uzantı hatası mesajı" "Arşiv dosyası uygunsuz veya bozuk!" "$out"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 5] Bozuk .sau dosyası${NC}"
echo "bu bir sau dosyasi degil" > sahte.sau
out=$(./tarsau -a sahte.sau d3 2>&1)
check "Bozuk dosya mesajı" "Arşiv dosyası uygunsuz veya bozuk!" "$out"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 6] Dizinsiz açma (mevcut dizine)${NC}"
mkdir -p test6 && cd test6
cp ../tarsau . && cp ../s1.sau .
out=$(./tarsau -a s1.sau 2>&1)
check "Dizinsiz açma çalıştı" "dosyaları açıldı" "$out"
[[ -f t1 && -f t5.dat ]] && check "Dosyalar mevcut dizine açıldı" "VAR" "VAR" || check "Dosyalar mevcut dizine açıldı" "VAR" "YOK"
cd ..

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 7] Mutlak yol ile açma${NC}"
rm -rf /tmp/tarsau_test_abs
out=$(./tarsau -a s1.sau /tmp/tarsau_test_abs 2>&1)
check "Mutlak yola açma" "/tmp/tarsau_test_abs dizininde" "$out"
[[ -f /tmp/tarsau_test_abs/t1 ]] && check "/tmp altında dosya var" "VAR" "VAR" || check "/tmp altında dosya var" "VAR" "YOK"
rm -rf /tmp/tarsau_test_abs

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 8] Argümansız çağırma${NC}"
./tarsau > /dev/null 2>&1
exit_code=$?
[[ "$exit_code" == "1" ]] && check "Çıkış kodu 1 (hata)" "1" "1" || check "Çıkış kodu 1 (hata)" "1" "$exit_code"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 9] -a sonrası fazla parametre${NC}"
./tarsau -a s1.sau d4 fazlalik > /dev/null 2>&1
exit_code=$?
[[ "$exit_code" == "1" ]] && check "Çıkış kodu 1 (hata)" "1" "1" || check "Çıkış kodu 1 (hata)" "1" "$exit_code"

# ------------------------------------------------------------
echo -e "\n${YELLOW}[TEST 10] .sau dosya formatı kontrolü${NC}"
first_10=$(head -c 10 s1.sau)
size_value=$(echo "$first_10" | tr -d ' ')
[[ "$size_value" =~ ^[0-9]+$ ]] && check "İlk 10 bayt sayısal" "OK" "OK" || check "İlk 10 bayt sayısal" "OK" "GEÇERSİZ"
echo -e "  ${BLUE}İlk 10 bayt:${NC} '$first_10'"
echo -e "  ${BLUE}Org. bölüm boyutu:${NC} $size_value"

# ------------------------------------------------------------
cd ..

echo -e "\n${BLUE}========================================${NC}"
echo -e "${BLUE}  SONUÇ${NC}"
echo -e "${BLUE}========================================${NC}"
echo -e "Başarılı: ${GREEN}$PASS${NC}"
echo -e "Başarısız: ${RED}$FAIL${NC}"

if [[ $FAIL -eq 0 ]]; then
    echo -e "${GREEN}✓ TÜM TESTLER BAŞARILI${NC}"
    exit 0
else
    echo -e "${RED}✗ BAZI TESTLER BAŞARISIZ${NC}"
    exit 1
fi
