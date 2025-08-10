#!/usr/bin/env python3
"""Fetch JSON data from an HTTP API and insert it into a SQLite database."""

import json
import sqlite3
import urllib.request

API_URL = "https://jsonplaceholder.typicode.com/posts/1"
DB_PATH = "example.db"


def main() -> None:
    # Retrieve JSON payload from the HTTP API
    with urllib.request.urlopen(API_URL) as response:
        payload = json.load(response)

    # Create or open the database and ensure the table exists
    conn = sqlite3.connect(DB_PATH)
    cur = conn.cursor()
    cur.execute(
        "CREATE TABLE IF NOT EXISTS posts (id INTEGER PRIMARY KEY, title TEXT, body TEXT)"
    )

    # Insert the retrieved payload into the database
    cur.execute(
        "INSERT OR REPLACE INTO posts (id, title, body) VALUES (:id, :title, :body)",
        payload,
    )
    conn.commit()

    # Verify insertion
    row = cur.execute("SELECT id, title FROM posts WHERE id = ?", (payload["id"],)).fetchone()
    print(f"Inserted row: {row}")
    conn.close()


if __name__ == "__main__":
    main()
