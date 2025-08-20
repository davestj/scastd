# Database Backend Migration

The original code base only supported MariaDB. The database layer has been refactored to a pluggable
interface and now also supports PostgreSQL.

## Selecting a Backend

Configure the database engine using the `DatabaseType` field in `scastd.conf`. Valid values:

- `mariadb`
- `postgres`

Schema setup scripts are loaded from `/etc/scastd/<dbtype>.sql` if present. Bundled copies live
in the source tree under `src/`:

- `src/mariadb.sql`
- `src/postgres.sql`
- `src/sqlite.sql`

Example:

```
username root
password secret
DatabaseType postgres
```

## SQL Dialect Notes

* **MariaDB** uses `?` placeholders in prepared statements.
* **PostgreSQL** uses numbered placeholders such as `$1`, `$2`.

Ensure that migration scripts and client code reflect the correct placeholder syntax.

## Schema Differences

PostgreSQL requires explicit `SERIAL`/`BIGSERIAL` types for auto-incrementing keys and does not
support `ENGINE=MyISAM` directives. MariaDB may need `ROW_FORMAT` adjustments for older dumps.

Refer to your database documentation when porting existing schemas.

### MariaDB

The provided dump at `src/mariadb.sql` has been modernized for MariaDB 10.11 and later:

* Deprecated `timestamp(14)` and `int(11)` types were replaced with `TIMESTAMP` and `INT`.
* All tables declare `ENGINE=InnoDB` with `CHARSET=utf8mb4` and `COLLATE=utf8mb4_unicode_ci`.
* Every column specifies an explicit `DEFAULT` clause, and auto-incrementing keys use
  `AUTO_INCREMENT`.

### PostgreSQL

An equivalent schema and seed data are available in `src/postgres.sql`. Key differences from the
MariaDB dialect include:

* `SERIAL` is used for auto-incrementing identifiers.
* Character fields rely on PostgreSQL's `TEXT` type and inherit database encoding (typically UTF‑8).
* Default values use `CURRENT_TIMESTAMP`, `CURRENT_DATE` and `CURRENT_TIME` expressions.
