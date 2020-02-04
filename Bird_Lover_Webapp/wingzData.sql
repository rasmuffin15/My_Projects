INSERT INTO users(username, name, email_address, university, favorite_bird) VALUES('JTheIsenhart', 'JD Isenhart', 'james.isenhart@colorado.edu', 'University of Colorado Boulder', 'EAGLE');
INSERT INTO userlogin VALUES('JtheIsenhart', '72f75591aee8c47a0f95177b9e13e3ff');

INSERT INTO userlogin VALUES('wingzTest', 'a7d046ad3a55a9467bb7dc4dfe6eae16');
INSERT INTO users VALUES ('wingzTest', 'test@cu.edu', 'University of Colorado Boulder', 'Wingz Testing', 'Emu');

INSERT INTO birds VALUES (DEFAULT, 'Canada Goose', 'Branta canadensis', 'A large wild goose species with a black head and neck, white cheeks, and a brown body. Natice to arctic and temperate regions of North America.', 'https://upload.wikimedia.org/wikipedia/commons/4/40/Canada_goose_on_Seedskadee_NWR_%2827826185489%29.jpg');
INSERT INTO birds VALUES (DEFAULT, 'Bald Eagle', 'Haliaeetus leucocephalus', 'The national bird of the US, this bird of prey can be found near large bodies of open water with an abundant food supply and old-growth trees for nesting.', 'https://upload.wikimedia.org/wikipedia/commons/1/1a/About_to_Launch_%2826075320352%29.jpg');
INSERT INTO birds VALUES (DEFAULT, 'Finch');
INSERT INTO birds VALUES (DEFAULT, 'Hummingbird');
INSERT INTO birds VALUES (DEFAULT, 'Crane');
INSERT INTO birds VALUES (DEFAULT, 'Black-Billed Magpie');
INSERT INTO birds VALUES (DEFAULT, 'Falcon');
INSERT INTO birds VALUES (DEFAULT, 'Owl');


INSERT INTO birdsightings VALUES (DEFAULT, 'wingzTest', 1, 'Ya mom', 'The UMC', 1);
