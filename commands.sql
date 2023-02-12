CREATE DATABASE sql_test;
CREATE TABLE IF NOT EXISTS `Author` (
  `id` INT NOT NULL AUTO_INCREMENT,
  `first_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `last_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  `email` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
  `title` VARCHAR(1024) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,
  PRIMARY KEY (`id`),
  KEY `fn` (`first_name`),
  KEY `ln` (`last_name`)
);
CREATE TABLE IF NOT EXISTS Message (
  id INT NOT NULL AUTO_INCREMENT,
  id_from INT NOT NULL,
  id_to INT NOT NULL,
  message VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (id),
  KEY ft (id_from, id_to)
);
insert into Author (first_name,last_name,email,title) values ('Иван','Иванов','ivanov@yandex.ru','господин');
insert into Author (first_name,last_name,email,title) values ('Петр','Петров','petrov@yandex.ru','господин');