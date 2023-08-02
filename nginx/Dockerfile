FROM nginx:latest

ARG CONFIG_FILE

COPY ${CONFIG_FILE} /etc/nginx/nginx.conf
COPY index.html /usr/share/nginx/html

RUN mkdir /www

EXPOSE 80
