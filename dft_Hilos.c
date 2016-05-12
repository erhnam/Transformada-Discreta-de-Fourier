#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#include <pthread.h>
#include <errno.h>

//Estructura de un número complejo
struct complejo {
	double r;		//Parte real
	double i;		//parte imaginaria
};


//Creamos las variables globales de la Funcion de Furier
double * modulo;		//Vector de modulos
double * fase;			//Vector de fases
struct complejo * espectro;	//Espectro de x[N]
double * x;			//Señal temporal		
int N;				//Tamaño Transformada de Fourier y de lineas del fichero

int numHilos;

void * calcula_Espectro_Mod_y_Fase (void * arg);

//Comienzo del programa
int main (int argc, char **argv){
						
	int i=0;							//Variable para bucle for
	int frec;							//Frecuencia de la señal en Hz
	double inicio,fin;					//variable para contar tiempoi ejecucion	
	srand((unsigned)time(NULL));

	printf("\n**************************************************************\n");
	printf("*            Crear una onda sinosuidal y aplicar             *\n");
	printf("*              Transformada Discreta de Fourier              *\n");
	printf("*               Ejecución Paralelo con Pthread               *\n");
	printf("**************************************************************\n\n");

	//Comprobacion de la sintaxis introducida por teclado
	if(argc != 4) {
		printf("Error de sintaxis: %s < Indique número de Hilos > < Tamaño de señal en potencia de 2 >  < Frecuencia de Señal en Hz >\n", argv[0]);
		return 0;
	}

	numHilos=atoi(argv[1]);
	N=atoi(argv[2]);
	frec=atoi(argv[3]);

	pthread_t thread[numHilos];						//Crea tantos hilos como indique la variable
	int hilo[numHilos];	
	double t[N];    	

    	//Asignamos nuevo tamaño a los vectores
    	espectro=malloc(N*sizeof(struct complejo));
    	x=malloc(N*sizeof(double));               								
    	modulo=malloc(N*sizeof(double));
    	fase=malloc(N*sizeof(double));

	//Se genera una señal sinosuidal con los datos pedidos anterior mente
	for(i = 0; i < N;  i++){
		t[i] = (double)(i) / ((double)(N) - 1.0);
		x[i] = sin(t[i]*frec);		
	}

 /*
 	//Imprime onda por pantalla
	for (i=0; i<N; i++){
		printf("%11.9f\n", x[i]);
	}
*/
	//Comienza el programa
	inicio=omp_get_wtime();

	//Calculo del espectro
	for(i=0 ; i < numHilos; i++){
		hilo[i]=i;
		if( pthread_create(&thread[i], NULL, calcula_Espectro_Mod_y_Fase, (void *) &hilo[i]) != 0){
			perror("Error en el create.\n");
			printf("Errno value = %d\n", errno);
			exit(EXIT_FAILURE);
		}
	}

	//Bucle for para recibir los hilos.
	for(i=0 ; i<numHilos; i++){
		if(pthread_join (thread[i], NULL) != 0){
			perror("Error en el join.\n");
			printf("Errno value = %d\n", errno);
			exit(EXIT_FAILURE);			
		}
	}

	fin=omp_get_wtime();			//Termina la cuenta
	                    								
	printf("Tiempo de ejecución con %d hilos usando Pthread: %11.9f segundos\n\n", numHilos, fin-inicio);

/*	//Imprime el módulo y la fase de cada iteración en el espectro	
	for (i=0; i<N; i++){
		printf("i = %d\nModulo = %12f\nFase = %12f\n\n", i,modulo[i], fase[i]);
	}
*/

	
	// Pasar los resutados a MATLAB para usar plot
	int n=0;
	FILE *f = fopen("dftplots_Pthread.m", "w");
	fprintf(f, "n = [0:%d];\n", N-1);
	fprintf(f, "x = [ ");
	for (n=0 ; n<N ; ++n){
	fprintf(f, "%f ", x[n]);
	}
	fprintf(f, "];\n");

	//fase
	fprintf(f, "fase = [ ");
	for (n=0 ; n<N ; ++n){
	fprintf(f, "%f ", fase[n] );
	}
	fprintf(f, "];\n");

	//modulo
	fprintf(f, "modulo = [ ");
	for (n=0 ; n<N ; ++n){
	fprintf(f, "%f ", modulo[n] );
	}
	fprintf(f, "];\n");


	//Código para Matlab
	fprintf(f, "Xk=fft(x);\n");
	fprintf(f, "k=x;\n");
	fprintf(f, "subplot(231)\n");
	fprintf(f, "stem(k,real(Xk));\n");
	fprintf(f, "title('Parte real del Espectro X(k)')\n");
	fprintf(f, "subplot(232)\n");
	fprintf(f, "stem(k,imag(Xk));\n");
	fprintf(f, "title('Parte imaginaria del Espectro X(k)')\n");
	fprintf(f, "xn=ifft(Xk);\n");
	fprintf(f, "subplot(234)\n");
	fprintf(f, "plot(n,real(xn));\n");
	fprintf(f, "title('Senal temporal x(n)')\n");


	//Amplitud y fase
	fprintf(f, "f=0:0.1:102.3;\n");
	fprintf(f, "subplot(233)\n");
	fprintf(f, "plot(f,modulo)\n");
	fprintf(f, "title('Amplitud del Espectro X(k)')\n");
	fprintf(f, "set(gca,'Xtick',[15 40 60 85])\n");
	fprintf(f, "subplot(236)\n");
	fprintf(f, "plot(f,fase*180/pi)\n");
	fprintf(f, "title('Fase del Espectro X(k)')\n");
	fprintf(f, "set(gca,'Xtick',[15 40 60 85])\n");


	fclose(f);


	return EXIT_SUCCESS;
}


//funcion de calcular el espectro
void * calcula_Espectro_Mod_y_Fase (void * arg){

int inicio, fin, intervalo;
int k;										//Indice frecuencial
int n;										//Indice temporal
int * numhilo=(int*)arg;

	//Con esto obtenemos el numero de hilo	
	intervalo=N/numHilos;
	inicio=(*numhilo)*intervalo;
	fin=(inicio+intervalo);

	for(k=inicio; k<fin; k++){
		espectro[k].r=espectro[k].i=0.0;				//Inicialización a 0 de las variables		
		for(n=0; n<N; n++){
			//Cálculo de parte real de Espectro[k].r
			espectro[k].r += x[n] * cos(-2*M_PI*k*n/N);

			//Cálculo de parte imagiinaria de Espectro[k].r
			espectro[k].i += x[n] * sin(-2*M_PI*k*n/N);
		}
	}

	for (k=inicio; k<fin; k++){
		modulo[k] = sqrt(pow(espectro[k].r,2) + pow(espectro[k].i,2));
		fase[k] = atan2(espectro[k].i, espectro[k].r);
	}

	pthread_exit(NULL);

}
