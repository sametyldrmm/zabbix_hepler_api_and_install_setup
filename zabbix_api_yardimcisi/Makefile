# Makefile

# Derleyici
CC := g++

# Derleme seçenekleri
CFLAGS := -lncurses

# Derleme hedefi
TARGET := run_program/panel

# Kaynak dosyaları
SOURCES := views/panel.cpp views/utils.cpp

# Nesne dosyaları
OBJECTS := $(SOURCES:.cpp=.o)

# Kural: Tüm hedef
all: ncurses hosts_listele get_items $(TARGET)

# Kural: ncurses kütüphanesini kur
ncurses:
	sudo apt-get install libncurses5-dev

# Kural: Nesne dosyalarını oluştur
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Kural: Programı derle ve oluşturulan dosyayı taşı
$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@

# Hedef: hosts_listele ve get_items
hosts_listele: controllers/hosts_listele.go
	go build -o run_program/hosts_listele $<

get_items: controllers/get_items2.go
	go build -o run_program/get_items $<

# Temizleme işlemi
clean:
	rm -f $(OBJECTS) $(TARGET) run_program/hosts_listele run_program/get_items

# PHONY hedefleri
.PHONY: all ncurses clean
