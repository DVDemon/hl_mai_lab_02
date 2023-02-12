# DB

## ProxySQL
## admin mysql -uradmin -pradmin -h 127.0.0.1 -P6032 --prompt='Admin> '

use main;
show tables;

Schema

```sql
create table if not exists customers
(
	id int auto_increment primary key,
	country varchar(255) not null,
	name varchar(255) null,
	city varchar(255) null
);

```

Users

```sql
GRANT ALL PRIVILEGES ON test.* TO 'test'@'%';
FLUSH PRIVILEGES;

SELECT * FROM mysql.user;
```

Config

```sql
UPDATE global_variables SET variable_value = "admin:admin;radmin:radmin" WHERE variable_name = 'admin-admin_credentials';
UPDATE global_variables SET variable_value = "0.0.0.0:6032" WHERE variable_name = 'admin-mysql_ifaces';
UPDATE global_variables SET variable_value = "true" WHERE variable_name = 'admin-web_enabled';
UPDATE global_variables SET variable_value = "true" WHERE variable_name = 'admin-restapi_enabled';
UPDATE main.global_variables SET variable_value = "events.log" WHERE variable_name = 'mysql-eventslog_filename';
UPDATE main.global_variables SET variable_value = "audit.log" WHERE variable_name = 'mysql-auditlog_filename';
```

Saving Global Variables

```
LOAD MYSQL VARIABLES TO RUNTIME;
SAVE MYSQL VARIABLES TO DISK;
```

Saving SERVERS

```
LOAD MYSQL SERVERS TO RUNTIME;
SAVE MYSQL VARIABLES TO DISK;
```

Saving USERS

```
LOAD MYSQL USERS TO RUNTIME;
SAVE MYSQL USERS TO DISK;
```

Stats from connection pool

```
SELECT * FROM stats.stats_mysql_connection_pool;
```

Rules Stats

```sql
CREATE TABLE stats_mysql_query_rules (
    rule_id INTEGER PRIMARY KEY,
    hits INT NOT NULL
);

SELECT * FROM stats.stats_mysql_query_rules \G;

```

Users stats

```sql
SELECT username, frontend_connections conns, frontend_max_connections max_conns  FROM stats_mysql_users WHERE frontend_connections > 0;
```