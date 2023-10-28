CC := gcc

all: customer.out restaurant.out

restaurant.out: udp.o 
	$(CC) udp.o -o restaurant.out

restaurant.out: udp.o 
	$(CC) udp.o -o restaurant.out

starwars.o: starwars.cpp GameController.cpp constants.hpp GameController.hpp 
	$(CC) -c starwars.cpp -o starwars.o

GameController.o: GameController.cpp Rocket.cpp GameObject.cpp Shooter.cpp Bullet.cpp Enemy.cpp MovingEnemy.cpp Hostage.cpp MapReader.cpp PowerUp.cpp src/rsdl.cpp constants.hpp GameController.hpp Rocket.hpp GameObject.hpp Shooter.hpp Bullet.hpp MovingEnemy.hpp Hostage.hpp MapReader.hpp PowerUp.hpp src/rsdl.hpp
	$(CC) -c GameController.cpp -o GameController.o

MapReader.o: MapReader.cpp src/rsdl.cpp MapReader.hpp constants.hpp ObjectOnMapData.hpp src/rsdl.hpp
	$(CC) -c MapReader.cpp -o MapReader.o
	
Rocket.o: Rocket.cpp Shooter.cpp Bullet.cpp PowerUp.cpp src/rsdl.cpp constants.hpp Rocket.hpp Shooter.hpp Bullet.hpp PowerUp.hpp src/rsdl.hpp
	$(CC) -c Rocket.cpp -o Rocket.o

Enemy.o: Enemy.cpp  Shooter.cpp Bullet.cpp src/rsdl.cpp constants.hpp Enemy.hpp Shooter.hpp Bullet.hpp src/rsdl.hpp
	$(CC) -c Enemy.cpp -o Enemy.o

MovingEnemy.o: MovingEnemy.cpp Enemy.cpp Bullet.cpp src/rsdl.cpp constants.hpp Enemy.hpp Bullet.hpp src/rsdl.hpp
	$(CC) -c MovingEnemy.cpp -o MovingEnemy.o

Bullet.o: Bullet.cpp  GameObject.cpp src/rsdl.cpp Bullet.hpp GameObject.hpp constants.hpp src/rsdl.hpp
		$(CC) -c Bullet.cpp -o Bullet.o

PowerUp.o: PowerUp.cpp  GameObject.cpp src/rsdl.cpp PowerUp.hpp GameObject.hpp constants.hpp src/rsdl.hpp
		$(CC) -c PowerUp.cpp -o PowerUp.o

Shooter.o: Shooter.cpp GameObject.cpp src/rsdl.cpp Shooter.hpp GameObject.hpp constants.hpp src/rsdl.hpp
	$(CC) -c Shooter.cpp -o Shooter.o

Hostage.o: Hostage.cpp GameObject.cpp src/rsdl.cpp Hostage.hpp GameObject.hpp constants.hpp src/rsdl.hpp
	$(CC) -c Hostage.cpp -o Hostage.o

GameObject.o: GameObject.cpp src/rsdl.cpp GameObject.hpp constants.hpp src/rsdl.hpp
	$(CC) -c GameObject.cpp -o GameObject.o

rsdl.o: src/rsdl.hpp src/rsdl.cpp
	$(CC) -c src/rsdl.cpp -o rsdl.o

.PHONY: clean

clean:
	rm -r *.o