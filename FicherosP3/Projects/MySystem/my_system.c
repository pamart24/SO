#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	return my_system(argv[1]);
}

int my_system(const char *command) {
	int status;
	pid_t pid = fork();

	int ret = 0;

	//Proceso hijo
	if (pid == 0) {
		if (execlp("/bin/bash", "-c", command, (char *) NULL) == -1) {
			printf("Error: fallo al hacer execlp\n");
			ret = -1;		
		}
	}
	//Proceso padre -- espera a que acabe el hijo
	else {
		//wait espera a que terminen todos los procesos hijos
		//status es una var de salida
		while(wait(&status) != pid);
	}
	return ret;
}
