-- SQLITE :memory:
-- Column-Width: 80

CREATE TABLE contacts (
	contact_id INTEGER PRIMARY KEY,
	first_name TEXT NOT NULL,
	last_name TEXT NOT NULL
);

INSERT INTO contacts (first_name, last_name) VALUES('Test', 'User');
INSERT INTO contacts (first_name, last_name) VALUES('Example', 'User');
INSERT INTO contacts (first_name, last_name) VALUES('User', 'van Test');

SELECT * FROM contacts;

SELECT	1 + 1;

SELECT 
   10 / 5, 
   2 * 4 ;