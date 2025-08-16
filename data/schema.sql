-- QuickQash SQLite schema (minimal)

PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS products (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    barcode TEXT UNIQUE,
    name TEXT NOT NULL,
    price_cents INTEGER NOT NULL DEFAULT 0,
    stock INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE IF NOT EXISTS sales (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    created_at TEXT NOT NULL DEFAULT (datetime('now')),
    total_cents INTEGER NOT NULL DEFAULT 0,
    paid_cents INTEGER NOT NULL DEFAULT 0,
    cashier TEXT
);

CREATE TABLE IF NOT EXISTS sale_items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    sale_id INTEGER NOT NULL,
    product_id INTEGER NOT NULL,
    qty INTEGER NOT NULL DEFAULT 1,
    price_cents INTEGER NOT NULL DEFAULT 0,
    FOREIGN KEY (sale_id) REFERENCES sales(id) ON DELETE CASCADE,
    FOREIGN KEY (product_id) REFERENCES products(id)
);
