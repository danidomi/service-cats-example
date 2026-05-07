CREATE DATABASE IF NOT EXISTS cats;
USE cats;

CREATE TABLE IF NOT EXISTS cats (
    id   INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(128) NOT NULL,
    age  INT NOT NULL
);

INSERT INTO cats (name, age) VALUES
    ('Whiskers', 3),
    ('Mittens',  5),
    ('Felix',    1);
