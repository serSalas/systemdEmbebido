FLAGS = -std=gnu11 -Wall -Werror -pedantic -Wextra -Wconversion
ULFIUS = -lulfius -lorcania -ljansson -lyder
BIN_1 = users
BIN_2 = stats
SRC_FOLDER = src
BIN_FOLDER = bin

all: users stats

users: $(SRC_FOLDER)/usuarios.c
	mkdir -p $(BIN_FOLDER)
	gcc $(FLAGS) $(SRC_FOLDER)/usuarios.c -o $(BIN_FOLDER)/$(BIN_1) $(ULFIUS)
stats: $(SRC_FOLDER)/status.c
	gcc $(FLAGS) $(SRC_FOLDER)/status.c -o $(BIN_FOLDER)/$(BIN_2) $(ULFIUS)

.PHONY: clean check install docs

clean :
	rm -Rf $(BIN_FOLDER)
check :
	cppcheck --enable=all ./
install :
	sudo useradd -p 1234 skaliver
	echo 'skaliver ALL=(ALL:ALL) NOPASSWD:ALL' | sudo EDITOR='tee -a' visudo
	sudo mkdir -p /var/log/tp3
	sudo chown skaliver /var/log/tp3
	sudo touch /var/log/tp3/hw_info.log
	sudo touch /var/log/tp3/users.log
	sudo chown skaliver /var/log/tp3/hw_info.log
	sudo chown skaliver /var/log/tp3/users.log
	sudo cp -f bin/users /usr/bin/users_tp3
	sudo cp -f bin/stats /usr/bin/hwinfo_tp3
	sudo cp -f build/usuariostp3.service /lib/systemd/system/usuariostp3.service
	sudo cp -f build/hwinfotp3.service /lib/systemd/system/hwinfotp3.service
	sudo cp -f build/tp3.conf /etc/nginx/sites-available/tp3.conf
	sudo ln -s /etc/nginx/sites-available/tp3.conf /etc/nginx/sites-enabled/
	sudo htpasswd -b -c /etc/apache2/.htpasswd so2 123456
	sudo systemctl daemon-reload
	sudo systemctl start usuariostp3.service
	sudo systemctl enable usuariostp3.service
	sudo systemctl start hwinfotp3.service
	sudo systemctl enable hwinfotp3.service
	sudo nginx -s reload
docs :
	doxygen Doxyfile