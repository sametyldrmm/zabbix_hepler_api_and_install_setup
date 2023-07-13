#!/bin/bash
DB_PATH="db_config"
DB_NAME=$(grep -i "db_name" $DB_PATH | cut -d'=' -f2 | tr -d ' ')

# PostgreSQL'e yönetici olarak giriş yap
sudo -u postgres psql << EOF

\c $DB_NAME;

DO \$\$
DECLARE
  tbl_name text;
BEGIN
  FOR tbl_name IN (SELECT table_name FROM information_schema.tables WHERE table_schema='public' AND table_type='BASE TABLE') LOOP
    EXECUTE 'DROP TABLE IF EXISTS ' || tbl_name || ' CASCADE';
  END LOOP;
END
\$\$;

EOF
