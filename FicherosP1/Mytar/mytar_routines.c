#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	//Posicionamos el indicador de posición al final del fichero origin
	if (fseek(origin, 0, SEEK_END) != 0) {
		printf("\nError en copynFile al hacer fseek del fichero origen\n");
		fclose(origin);
		return -1;
	}
	//Guardamos los bytes totales de origin
	int bytes = ftell(origin);

	//Calculamos cual es el mínimo: nBytes o los bytes del fichero origin
	if (nBytes < bytes)
		bytes = nBytes;

	printf("\nBytes copiados: %d\n", bytes);

	//Reservamos espacio para los bytes del fichero a copiar
	char* buff = malloc(bytes);

	//Volvemos a colocar el marcador de posición al inicio de origin
	rewind(origin);

	fread(buff, sizeof(char), bytes, origin);
	fwrite(buff, sizeof(char), bytes, destination);

	printf("\nLectura y escritura de fichero origen a fichero destino con éxito\n");

	return bytes;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	// Complete the function
	return NULL;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	// Complete the function
	//Llama a loadstr

	return NULL;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE* mtar = NULL;

	//Abrimos el fichero
	if ((mtar = fopen(tarName, "w")) == NULL) {
		printf("\nError en createTar al hacer fopen de mytar\n");
		return EXIT_FAILURE;
	}

	stHeaderEntry* header = malloc(sizeof(stHeaderEntry) * nFiles);

	//Guardamos los bytes que hay que saltarse en el mtar porque son para la cabecera
	int hBytes = sizeof(int) + (sizeof(unsigned int) * nFiles);
	for (int i = 0; i < nFiles; ++i) {
		hBytes += strlen(fileNames[i]) + 1;	//+1 por el '\0'
	}

	//Nos saltamos los bytes de la cabecera para copiar los archivos
	if (fseek(mtar, hBytes + 1, SEEK_SET) != 0) {
		printf("\nError en createTar al hacer fseek en mytar\n");
		return EXIT_FAILURE;
	}
	
	//Por cada archivo que haya que copiar en mtar hay que abrir el archivo, llamar a
	//copynFile y cerrar el archivo
	for (int i = 0; i < nFiles; ++i) {
		FILE* file = NULL;
		if ((file = fopen(fileNames[i], "r")) == NULL) {
			printf("\nError en createTar al abrir el fichero %s\n", fileNames[i]);
			return EXIT_FAILURE;
		}
		
		int ret = copynFile(file, mtar, INT_MAX);
		if (ret == -1)	{
			printf("\nError en createTar al copiar el fichero %s\n", fileNames[i]);		//No cierro el fichero porque lo cierro
			return EXIT_FAILURE;	//en copynFile
		}

		//Cerramos el fichero
		fclose(file);
		
		//Como name es un char* necesita que resevemos memoria
		header[i].name = malloc(strlen(fileNames[i]) + 1);
		strcpy(header[i].name, fileNames[i]);
		strcat(header[i].name, "\0");	//Sirve para contatenar cadenas

		header[i].size = ret;
	}

	//Volvemos al comienzo del fichero mtar para escribir la cabecera
	rewind(mtar);
	fwrite(&nFiles, sizeof(int), 1, mtar);
	
	//Copiamos el array header en la cabecera
	for (int i = 0; i < nFiles; ++i) {
		fwrite(header[i].name, strlen(header[i].name) + 1, 1, mtar);
		fwrite(&header[i].size, sizeof(unsigned int), 1, mtar);
	}

	//Liberamos espacio
	//Primero liberamos los header[].name
	for (int i = 0; i < nFiles; ++i) {
		free(header[i].name);
		header[i].name = NULL;
	}

	//Liberamos el header en sí
	free(header);

	//Cerramos el fichero mtar
	fclose(mtar);

	printf("\nCopia de ficheros a mytar con éxito\n");

	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	// Complete the function
	return EXIT_FAILURE;
}
