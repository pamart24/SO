#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define N_PARADAS 5 // número de paradas de la ruta
#define EN_RUTA 0 // autobús en ruta
#define EN_PARADA 1 // autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4 // numero de usuarios

// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobús

// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};

// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};

// Otras definiciones globales (comunicación y sincronización)
pthread_t autobus;
pthread_t usuarios[USUARIOS];
pthread_cond_t busEnParada;
pthread_cond_t moverBus;
pthread_mutex_t mAutobus;
pthread_mutex_t mSubidas;
pthread_mutex_t mBajadas;

void Autobus_En_Parada() {
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */
	estado = EN_PARADA;
	printf("%d personas quieren subir\n", esperando_parada[parada_actual]);
	printf("%d personas quieren bajar\n", esperando_bajar[parada_actual]);

	//Avisamos a todos de que el bus está en parada
	pthread_cond_broadcast(&busEnParada);

	pthread_mutex_lock(&mAutobus);

	//Esperamos a que podamos mover el autobus
	while(esperando_parada[parada_actual] != 0 || esperando_bajar[parada_actual] != 0) {
		printf("Autobús esperando para moverse\n");
		pthread_cond_wait(&moverBus, &mAutobus);
	}
	pthread_mutex_unlock(&mAutobus);
}

void Conducir_Hasta_Siguiente_Parada() {
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	estado = EN_RUTA;
	printf("Autobús comienza a moverse...\n");

	sleep(random() % 5);

	//Actualizamos la parada
	parada_actual = (parada_actual + 1) % N_PARADAS;	//% N_PARADAS para que de la vuelta
	printf("Autobús se para en la parada %d...\n", parada_actual);
}

void Subir_Autobus(int id_usuario, int origen) {
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	//Bloqueamos el mutex de subidas
	pthread_mutex_lock(&mSubidas);
	printf("El usuario %d quiere subir en la parada %d\n", id_usuario, origen);

	//Hay un nuevo usuario que espera en esta parada
	esperando_parada[origen] = esperando_parada[origen] + 1;

	while(parada_actual != origen) {
		pthread_cond_wait(&busEnParada, &mSubidas);
	}

	//Cuando salgamos del bucle quiere decir que podemos subir al autobus
	esperando_parada[origen] = esperando_parada[origen] - 1;
	if(esperando_parada[origen] == 0) {
		//El autobus tiene que volver a ponerse en ruta
		pthread_cond_broadcast(&moverBus);
	}
	printf("El usuario %d se ha subido al autobús\n", id_usuario);

	pthread_mutex_unlock(&mSubidas);
}

void Bajar_Autobus(int id_usuario, int destino) {
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */

	//Bloqueamos el mutex de bajadas
	pthread_mutex_lock(&mBajadas);
	printf("El usuario %d quiere bajar en la parada %d\n", id_usuario, destino);

	//Hay un nuevo usuario que quiere bajar en la parada destino
	esperando_bajar[destino] = esperando_bajar[destino] + 1;

	while(parada_actual != destino) {
		pthread_cond_wait(&busEnParada, &mBajadas);
	}

	//Cuando salgamos del bucle quiere decir que ya hemos bajado del autobus
	esperando_bajar[destino] = esperando_bajar[destino] - 1;
	if(esperando_bajar[destino] == 0) {
		//El autobus tiene que volver a ponerse en ruta
		pthread_cond_broadcast(&moverBus);
	}
	printf("El usuario %d se ha bajado del autobús\n", id_usuario);

	pthread_mutex_unlock(&mBajadas);
}

void* thread_autobus(void * args) {
	while (1) {
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}

//Establece en que parada se sube un usuario y en cual se baja
void* thread_usuario(void * arg) {
	int id_usuario = (int)arg, a = 0, b = 0;
	// obtener el id del usario
	while (1) {
		a=rand() % N_PARADAS;
		do {
			b=rand() % N_PARADAS;
		} while(a==b);
			Usuario(id_usuario,a,b);
	}
}

void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus esté en parada origen para subir
	Subir_Autobus(id_usuario, origen);

	// Bajarme en estación destino
	Bajar_Autobus(id_usuario, destino);
}

int main(int argc, char* argv[])
{
	int i;

	// Definicion de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	// Crear el thread Autobus
	pthread_create(&autobus, NULL, thread_autobus, NULL);

	for (i = 0; i < USUARIOS; ++i) {
		// Crear thread para el usuario i
		pthread_create(&usuarios[i], NULL, thread_usuario, (void *)i);
	}
	for (i = 0; i < USUARIOS; ++i) {
		// Esperar terminacion de los hilos
		pthread_join(usuarios[i], NULL);
	}
	//Esperar terminacion del autobus
	pthread_join(autobus, NULL);

	return 0;
}