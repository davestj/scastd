# PostgreSQL Setup

This guide demonstrates how to apply the provided PostgreSQL schema and outlines key differences from the MySQL dialect.

## Applying the Schema

1. Ensure PostgreSQL and the `psql` client are installed and the server is running.
2. Create a dedicated database user and database:
   ```sh
   createuser -P scastd_user
   createdb -O scastd_user scastd
   ```
3. From the repository root, load the schema:
   ```sh
   psql -U scastd_user -d scastd -f src/postgres.sql
   ```
4. Verify the tables were created:
   ```sh
   psql -U scastd_user -d scastd -c '\\dt'
   ```

## SQL Dialect Differences

* **Auto-increment** – PostgreSQL uses `SERIAL` or `BIGSERIAL` instead of MySQL's `AUTO_INCREMENT`.
* **Timestamp defaults** – `CURRENT_TIMESTAMP` is used for default values and PostgreSQL does not support MySQL's `ON UPDATE` clauses.
* **Identifier quoting** – Double quotes (`"`) quote identifiers and preserve case; MySQL uses backticks and is case-insensitive.

## Troubleshooting

* `psql: could not connect to server`: ensure the PostgreSQL service is running and that host/port settings are correct.
* `psql: FATAL: role "scastd_user" does not exist`: create the user with `createuser` and rerun the command.
* `ERROR: permission denied for database scastd`: confirm the database owner and granted privileges.
* `ERROR: relation "<table>" does not exist`: verify the schema loaded successfully and that the `search_path` is set correctly.
