## Пример по курсу Архитектура Программных Систем

Использование proxy sql для шардинга данных в несколько инстансов MySQL https://proxysql.com/



### Подготовка базы данных

CREATE TABLE IF NOT EXISTS Message (id INT NOT NULL AUTO_INCREMENT, id_from INT NOT NULL,id_to INT NOT NULL, message VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,PRIMARY KEY (id),KEY ft (id_from,id_to));

CREATE TABLE IF NOT EXISTS `Author` (`id` INT NOT NULL AUTO_INCREMENT,`first_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,`last_name` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,`email` VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,`title` VARCHAR(1024) CHARACTER SET utf8 COLLATE utf8_unicode_ci NULL,PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));


### Добавление авторов

http://localhost/author?add&first_name=*Vasia*&last_name=*Pupkin*&title=*Mr*&email=*vasia@pupkin.com*

### Поиск авторов

http://localhost/author?id=*some_id*

## Добавляем сообщение

http://localhost/message?add&id_from=*some_id*&id_to=another_id&message=*Wow*

### Получить сообщения от одного атвора другому

http://localhost/message?id_from=*some_id*&id_to=*another_id*

### Получить список контактов

http://localhost/message?id=*some_id*
