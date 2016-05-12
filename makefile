.PHONY :  clean

CC = gcc-5	#se usa gcc-5 para mac si usas linux sustituir por gcc
FLAGS =  -Wall -o 
CFLAGS =  -Wall -fopenmp -o 

main: $(OBJETOS_SECUENCIAL OBJETOS_PTHREAD OBJETOS_OPENMP)

	@echo Generando objetos...
	$(CC) -c dft_secuencial.c
	$(CC) -c dft_Hilos.c 
	$(CC) -c dft_OpenMp.c
	@echo
	@echo Compilando...
	$(CC) $(FLAGS) dft_secuencial.out dft_secuencial.o
	$(CC) $(CFLAGS) dft_Hilos.out dft_Hilos.o
	$(CC) $(CFLAGS) dft_OpenMp.out dft_OpenMp.o
	@echo
	@echo Borrando ficheros .o ...
	rm -rf *.o
	@echo
	@echo  Terminado...
	@echo
	@echo  "./dft_secuencial.out <número de muestras> <frecuencia de la señal> para iniciar el programa secuencial"
	@echo  "./dft_Hilos.out <número de muestras> <frecuencia de la señal> para iniciar el programa secuencial"
	@echo  "./dft_OpenMp.out <número de muestras> <frecuencia de la señal> para iniciar el programa secuencial" 
	@echo se generará un fichero .m para mostrar los resultados en Matlab.
	@echo

clean:
	@echo Borrando ficheros .o ...
	rm -rf *.o
	@echo Borrando ficheros .m ...
	rm -rf *.m
	@echo Borrando ficheros .out ...
	rm -rf *.out
