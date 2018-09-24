#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define PROCESY_MAX 64

//Liczba samolotów obsługiwanych
int n;

//Liczba samolotów aktualnie na lotniskowcu
int samoloty_na_lotniskowcu = 0;

int samoloty_ladujace = 0;
int samoloty_startujace = 0;

//Semafor
sem_t semafor;

void* startowanie(void* arg)
{
	//Oczekiwanie na pozwolenie do startu
	sem_wait(&semafor);
	while(samoloty_ladujace > 0 && samoloty_na_lotniskowcu <= n/2) {
		sem_post(&semafor);
		sem_wait(&semafor);
	}
	
	//Sprawdzenie możliwości wykonania rozkazu
	if(samoloty_na_lotniskowcu > 0) {
		//Animacja losowej długości
		printf("Samolot startuje");
		fflush(stdout);
		for(int i=0; i<3; i++) {
			printf(".");
			sleep((rand()%2)+1);
			fflush(stdout);
		}
		printf("samolot wystartował\n");
		fflush(stdout);
		
		//Aktualizacja danych
		samoloty_na_lotniskowcu--;
		samoloty_startujace--;
	}
	else
		printf("Brak samolotów do startu\n");
	
	//Komunikat o udanym starcie
	sem_post(&semafor);
}

void* ladowanie(void* arg)
{
	//Oczekiwanie na pozwolenie na lądowanie
	sem_wait(&semafor);
	while(samoloty_startujace > 0 && samoloty_na_lotniskowcu > n/2) {
		sem_post(&semafor);
		sem_wait(&semafor);
	}
	
	//Sprawdzenie możliwości wykonania rozkazu
	if(samoloty_na_lotniskowcu < n) {
		//Animacja losowej długości
		printf("Samolot laduje");
		fflush(stdout);
		for(int i=0; i<3; i++) {
			printf(".");
			sleep((rand()%2)+1);
			fflush(stdout);
		}
		printf("samolot wyladowal\n");
		fflush(stdout);
		
		//Aktualizacja danych
		samoloty_na_lotniskowcu++;
		samoloty_ladujace--;
	}
	else
		printf("Brak miejsc na lotniskowcu, odmowa pozwolenia na lądowanie\n");
	
	//Komunikat o udanym lądowaniu
	sem_post(&semafor);
}
 
 
int main() {
	srand(time(NULL));
	
	//Informacje o działaniu programu
	printf(
		"=========================================================\n"
		"Liczba samolotów obsługiwana przez lotnisko: "
	);
	
	//Pobranie maksymalnej ładowności od użytkownika
	scanf("%d", &n);
	
	//Pominięcie znaku nowej linii w buforze
	getchar();
	
	printf(
		"=========================================================\n"
		"Podaj sekwencję startów (s) oraz lądowań (l). Uruchom naciskając Enter\n"
		"Dla przykładu: slls oznacza\n"
		"startuj / ląduj / ląduj / startuj\n"
		"=========================================================\n"
		"Sekwencja: "
	);
	
	//Inicjacja semaforu
	sem_init(&semafor, 0, 1);
	
	//Inicjowanie procesów
	pthread_t procesy[PROCESY_MAX];
	int liczba_procesow = 0;
	char znak;
	while((znak = getchar()) != '\n') {
		switch(znak) {
			case 's':
				samoloty_startujace++;
				pthread_create(&procesy[liczba_procesow], NULL, startowanie, NULL);
				break;
			case 'l':
				samoloty_ladujace++;
				pthread_create(&procesy[liczba_procesow], NULL, ladowanie, NULL);
				break;
		}
		
		//Przepełnienie tablicy procesów, przerwanie działania programu
		if((liczba_procesow+1) == PROCESY_MAX) {
			sem_destroy(&semafor);
			printf("Ilość procesów musi być mniejsza od 100");
			return 1;
		}
		liczba_procesow++;
	}
	printf("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv\n\n");
	
	//Oczekiwanie na ich zakończenie
	for(int i=0; i<liczba_procesow; i++)
		pthread_join(procesy[i], NULL);
	
	//Wyjście z programu
	printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	sem_destroy(&semafor);
    return 0;
}
