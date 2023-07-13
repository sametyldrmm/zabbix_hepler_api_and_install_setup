#!/bin/bash
DB_PATH="./models/db_config"
# PostgreSQL veritabanı kullanıcı bilgileri
DB_USER=$(grep -i "user" $DB_PATH | cut -d'=' -f2 | tr -d ' ')
DB_PASSWORD=$(grep -i "password" $DB_PATH | cut -d'=' -f2 | tr -d ' ')

# Kullanıcıyı oluşturma sorgusu
SQL_QUERY="CREATE USER $DB_USER WITH PASSWORD '$DB_PASSWORD';"
# PostgreSQL komutu ile kullanıcıyı oluşturma
sudo -u postgres psql -c "$SQL_QUERY"

# Kullanıcıyı süper kullanıcı olarak yetkilendirme sorgusu
SQL_QUERY="ALTER USER $DB_USER WITH SUPERUSER ;"
# PostgreSQL komutu ile kullanıcıyı süper kullanıcı olarak yetkilendirme
sudo -u postgres psql -c "$SQL_QUERY"

DB_NAME=$(grep -i "dbname" $DB_PATH | cut -d'=' -f2 | tr -d ' ')
# Kullanıcıya ait veritabanını oluşturma sorgusu
SQL_QUERY="CREATE DATABASE $DB_NAME OWNER $DB_USER;"
# PostgreSQL komutu ile veritabanını oluşturma
sudo -u postgres psql -c "$SQL_QUERY"
