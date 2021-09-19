#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;

	if (argc!=3) {
		fprintf(stderr,"Usage: %s <file_name> <num_bytes>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);


	/****************************ORIGINAL******************************
	int c,ret;

	// Read file byte by byte
	while ((c = getc(file)) != EOF) {
		// Print byte to stdout
		ret=putc((unsigned char) c, stdout);
	
		if (ret==EOF){
			fclose(file);
			err(3,"putc() failed!!");
		}
	}
	******************************************************************/
	
	//**************************EJERCICIO******************************
	//Necesitamos saber el tamaño del archivo a leer para poder reservar memoria
	//Para ello primero situaremos el indicador de posición del fichero al final
	//con fseek, luego la llamada a ftell nos devuelve los bytes desde la posición
	//0 a la posición del indicador de posición, que en este caso será el final del
	//fichero (SEEK_END)
	
	if (fseek(file, 0, SEEK_END)!=0) {
		fclose(file);
		err(4, "Error in fseek");
	}
	int bytes = ftell(file);

	//El argumento argv[2] viene como un string así que es necesario hacer atoi,
	//que lo pasa a entero
	int num_bytes = atoi(argv[2]);

	//Necesitamos saber si hay menos bytes en el fichero que los que pide el usuario
	if (bytes > num_bytes)
		bytes = num_bytes;

	//Reservamos espacio de memoria
	char* buff = malloc(bytes);

	//Antes de hacer fread hay que volver a posicionar el indicador de posición
	//al inicio del fichero, es lo mismo que fseek(file, 0, SEEK_SET)
	rewind(file);

	//Read file byte by byte
	fread(buff, sizeof(char), bytes, file);
	
	//fread y fwrite mueve el indicador de posición hasta donde lee, asi que lo
	//posicionamos de nuevo al inicio, aunque no es necesario porque no afecta más
	rewind(file);

	//Print byte to stdout
	fwrite(buff, sizeof(char), bytes, stdout);	//stdout es la propia consola
	rewind(file);

	free(buff);
	//*****************************************************************
	
	fclose(file);
	return 0;
}
