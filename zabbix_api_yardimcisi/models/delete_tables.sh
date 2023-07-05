#!/bin/bash

# PostgreSQL'e yönetici olarak giriş yap
sudo -u postgres psql << EOF

\c test_veritabani;

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
