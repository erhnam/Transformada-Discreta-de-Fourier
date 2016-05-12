#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

//#define Pi 3.14159265358979323846

//Estructura de un número complejo
struct complejo {

	double r;		//Parte real
	double i;		//parte imaginaria

};

//Comienzo del programa
int main (int argc, char **argv){

	clock_t start;								//Variable para el tiempo
	unsigned int i=0;							//Variable para bucle for
	int N;									//Tamaño Transformada de Fourier y de lineas del fichero
	int frec;									//Frecuencia de la señal en Hz
	srand((unsigned)time(NULL));

	printf("\n**************************************************************\n");
	printf("*            Crear una onda sinosuidal y aplicar             *\n");
	printf("*              Transformada Discreta de Fourier              *\n");
	printf("*                    Ejecución secuencial                    *\n");
	printf("**************************************************************\n\n");

	//Comprobacion de la sintaxis introducida por teclado
	if(argc != 3) {
		printf("Error de sintaxis: %s < Tamaño de señal en potencia de 2 >  < Frecuencia de Señal en Hz >\n", argv[0]);
		return 0;
	}

	N=atoi(argv[1]);
	frec=atoi(argv[2]);

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

	//Se crea las variables que se van a usar en la DFT
	struct complejo espectro[N];						//Espectro de x[N]
	int k;									//Indice frecuencial
	int n;									//Indice temporal
	double modulo[N];							//Vector de modulos
	double fase[N];							//Vector de fases
    	
    	start = clock();								//Comienza a contar

	//Calculo del espectro
	for(k=0; k<N; k++){
		espectro[k].r=espectro[k].i=0.0;				//Inicialización a 0 de las variables
		for(n=0; n<N; n++){
			//Cálculo de parte real de Espectro[k].r
			espectro[k].r += x[n] * cos(-2*M_PI*k*n/N);

			//Cálculo de parte imagiinaria de Espectro[k].r
			espectro[k].i += x[n] * sin(-2*M_PI*k*n/N);
		}
	}

/*
	//Imprime parte real y parte imaginaria
	for (k=0; k<N; k++){
		printf("Re: %f    Im: %f\n", espectro[k].r ,espectro[k].i);
	}
*/
	
	//Calculo del Módulo y fase del espectro
	for (k=0; k<N; k++){
		modulo[k] = sqrt(pow(espectro[k].r,2) + pow(espectro[k].i,2));
		fase[k] = atan2(espectro[k].i,espectro[k].r);
	}

	printf("Tiempo de ejecución secuencial: %11.9f segundos\n\n", (double)(clock() - start) / CLOCKS_PER_SEC);

/*	//Imprime el módulo y la fase de cada iteración en el espectro
	for (k=0; k<N; k++){
		printf("k = %d\nModulo = %12f\nFase = %12f\n\n", k,modulo[k], fase[k]);
	}
*/

	// Pasar los resutados a MATLAB para usar plot
	FILE *f = fopen("dftplots.m", "w");
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