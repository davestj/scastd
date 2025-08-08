# Database Backend Migration

The original code base only supported MySQL. The database layer has been refactored to a pluggable
interface and now supports MySQL, MariaDB and PostgreSQL.

## Selecting a Backend

Configure the database engine using the `DatabaseType` field in `scastd.conf`. Valid values:

- `mysql`
- `mariadb`
- `postgres`

Example:

```
username root
password secret
DatabaseType postgres
```

## SQL Dialect Notes

* **MySQL / MariaDB** use `?` placeholders in prepared statements.
* **PostgreSQL** uses numbered placeholders such as `$1`, `$2`.

Ensure that migration scripts and client code reflect the correct placeholder syntax.

## Schema Differences

PostgreSQL requires explicit `SERIAL`/`BIGSERIAL` types for auto-incrementing keys and does not
support `ENGINE=MyISAM` directives present in some MySQL schemas. MariaDB follows MySQL semantics
but may need `ROW_FORMAT` adjustments for older MySQL dumps.

Refer to your database documentation when porting existing schemas.
