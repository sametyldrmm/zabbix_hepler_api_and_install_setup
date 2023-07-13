# Derleyici
CC := g++

# Derleme seçenekleri
CFLAGS := -lncurses -lpqxx -lpq -Wall -Wextra -Werror 

# Derleme hedefi
TARGET := run_program/panel

# Kaynak dosyaları
SOURCES := views/panel.cpp views/utils.cpp models/databaseManager.cpp  

# Nesne dosyaları
OBJECTS := $(SOURCES:.cpp=.o)

# Kural: Tüm hedef
all: get_items $(TARGET)

# Kural: ncurses kütüphanesini kur
ncurses:
	sudo apt-get install libncurses5-dev

# Kural: Nesne dosyalarını oluştur
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Kural: Programı derle ve oluşturulan dosyayı taşı
$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@

get_items: controllers/get_items.go
	go build -o run_program/get_items $<

# Temizleme işlemi
clean:
	rm -f $(OBJECTS) $(OBJECTS_DB) $(TARGET) run_program/hosts_listele run_program/get_items

firstMake: ncurses
	# sudo apt install libpqxx-dev
	# sudo bash models/user_create_permission_databae.sh
	sudo mkdir /var/log/zabix_api_yardimcisi
	echo "12"
	sudo touch /var/log/zabix_api_yardimcisi/all.log
	go build ./models/veri_tabani_create.go 
	sudo ./veri_tabani_create
	rm -rf ./veri_tabani_create

re : clean all
# PHONY hedefleri
.PHONY: all ncurses clean firstMake re
