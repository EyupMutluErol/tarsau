CC      = gcc
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 -O2 -Wno-format-truncation
TARGET  = tarsau
OBJS    = tarsau.o utils.o build.o extract.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

tarsau.o: tarsau.c tarsau.h
	$(CC) $(CFLAGS) -c tarsau.c

utils.o: utils.c tarsau.h
	$(CC) $(CFLAGS) -c utils.c

build.o: build.c tarsau.h
	$(CC) $(CFLAGS) -c build.c

extract.o: extract.c tarsau.h
	$(CC) $(CFLAGS) -c extract.c

clean:
	rm -f $(TARGET) *.o *.sau
	rm -rf test_out

test: $(TARGET)
	@echo "=== Test dosyaları oluşturuluyor ==="
	@echo "Bu birinci dosyadir." > t1
	@echo "Bu ikinci dosyadir."  > t2
	@echo "Bu ucuncu dosyadir."  > t3.txt
	@echo "=== Arşivleme ==="
	./$(TARGET) -b t1 t2 t3.txt -o test.sau
	@echo "=== Açma ==="
	./$(TARGET) -a test.sau test_out
	@echo "=== Açılan dosyalar ==="
	ls -l test_out/
	@echo "=== İçerikler ==="
	cat test_out/t1 test_out/t2 test_out/t3.txt

.PHONY: all clean test
