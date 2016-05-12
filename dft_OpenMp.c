#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

//Estructura de un número complejo
struct complejo {

	double r;		//Parte real
	double i;		//parte imaginaria

};

//Comienzo del programa
int main (int argc, char **argv){

	unsigned int i=0;							//Variable para bucle for
	int N;									//Tamaño Transformada de Fourier y de lineas del fichero
	int frec;									//Frecuencia de la señal en Hz
	int numHilos;
	srand((unsigned)time(NULL));

	printf("\n**************************************************************\n");
	printf("*            Crear una onda sinosuidal y aplicar             *\n");
	printf("*              Transformada Discreta de Fourier              *\n");
	printf("*                Ejecución Paralelo con OpenMp               *\n");
	printf("**************************************************************\n\n");

	//Comprobacion de la sintaxis introducida por teclado
	if(argc != 4) {
		printf("Error de sintaxis: %s < Indique número de Hilos >  < Tamaño de señal en potencia de 2 >  < Frecuencia de Señal en Hz >\n", argv[0]);
		return 0;
	}

	numHilos=atoi(argv[1]);
	N=atoi(argv[2]);
	frec=atoi(argv[3]);

	double t[N];								//Buffer para crear señal temporal
	double x[N];								//Señal temporal

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

	//Creamos las variables de la Funcion de Furier
	struct complejo espectro[N];				//Espectro de x[N]
	int k;										//Indice frecuencial
	int n;										//Indice temporal
	double modulo[N];								//Vector de modulos
	double fase[N];								//Vector de fases
	double inicio,fin;								//variable para contar tiempoi ejecucion
	omp_set_num_threads(numHilos);						//Variable Openmp para trabajar con x hilos

	inicio=omp_get_wtime();							//Comienza a contar

	//Calculo del espectro

	#pragma omp parallel shared(espectro, x, N, modulo, fase) private(k, n) 
    	{
		#pragma omp for schedule(static) 
		for(k=0; k<N; k++){
			espectro[k].r=espectro[k].i=0.0;				//Inicialización a 0 de las variables
			for(n=0; n<N; n++){
				//Cálculo de parte real de Espectro[k].r
				espectro[k].r += x[n] * cos(-2*M_PI*k*n/N);

				//Cálculo de parte imagiinaria de Espectro[k].r
				espectro[k].i += x[n] * sin(-2*M_PI*k*n/N);
			}
		}

		//Calculo del Módulo y fase del espectro
		#pragma omp for schedule(static)
		for (k=0; k<N; k++){
			modulo[k] = sqrt(pow(espectro[k].r,2) + pow(espectro[k].i,2));
			fase[k] = atan2(espectro[k].i,espectro[k].r);
		}
	}
	
	//Termina la cuenta
	fin=omp_get_wtime();			
	                    								
	printf("Tiempo de ejecución con %d hilos usando OpenMp: %11.9f segundos\n\n", numHilos, fin-inicio);

/*	//Imprime el módulo y la fase de cada iteración en el espectro
	for (k=0; k<N; k++){
		printf("k = %d\nModulo = %12f\nFase = %12f\n\n", k,modulo[k], fase[k]);
	}
*/

 // Pasar los resutados a MATLAB para usar plot
	FILE *f = fopen("dftplots_OMP.m", "w");
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