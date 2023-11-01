FROM mariadb:10.10.4
# FROM mysql:8
# FROM bitnami/mysql:5.7
# Update and clean apt cache
USER root
RUN apt-get clean && apt-get update --fix-missing && \
    apt-get install -y nano ssh inetutils-ping net-tools htop telnet curl ncdu rsync whois dnsutils git zip gosu wget;

# MySQL Custom Configs
COPY my.cnf /etc/mysql/
COPY mysql.conf.d/ /etc/mysql/mysql.conf.d/

# Touching log files
RUN touch -a /var/log/mysql/error.log && chown mysql:adm /var/log/mysql/error.log
RUN touch -a /var/log/mysql/general.log && chown mysql:adm /var/log/mysql/general.log
RUN touch -a /var/log/mysql/slow.log && chown mysql:adm /var/log/mysql/slow.log
