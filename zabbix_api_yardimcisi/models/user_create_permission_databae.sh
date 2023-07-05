#!/bin/bash

# PostgreSQL veritabanı kullanıcı bilgileri
DB_USER="test_user"
DB_PASSWORD="1"


# Kullanıcıyı oluşturma sorgusu
SQL_QUERY="CREATE USER $DB_USER WITH PASSWORD '$DB_PASSWORD';"
# PostgreSQL komutu ile kullanıcıyı oluşturma
sudo -u postgres psql -c "$SQL_QUERY"


# Kullanıcıyı süper kullanıcı olarak yetkilendirme sorgusu
SQL_QUERY="ALTER USER $DB_USER WITH SUPERUSER ;"
# PostgreSQL komutu ile kullanıcıyı süper kullanıcı olarak yetkilendirme
sudo -u postgres psql -c "$SQL_QUERY"


DB_NAME="test_veritabani"
# Kullanıcıya ait veritabanını oluşturma sorgusu
SQL_QUERY="CREATE DATABASE $DB_NAME OWNER $DB_USER;"
# PostgreSQL komutu ile veritabanını oluşturma
sudo -u postgres psql -c "$SQL_QUERY"
