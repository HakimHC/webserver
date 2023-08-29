all:
	cp -r www nginx/www
	cp -r www webserv/www
	cp -r ../src webserv
	cp -r ../include webserv
	cp ../Makefile webserv
	docker compose up --build
dt:
	cp -r www nginx/www
	cp -r www webserv/www
	cp -r ../src webserv
	cp -r ../include webserv
	cp ../Makefile webserv
	docker compose up -d --build
down:
	docker compose down
	rm -rf webserv/src
	rm -rf webserv/include
	rm -rf webserv/Makefile

re:: down
re:: all
