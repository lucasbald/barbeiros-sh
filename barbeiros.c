#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* Argumentos passados */
int qtdBarbeiros;
int qtdCadeiras;
int tempoTrabalho;
int tempoEntreChegadas;

/*  */
int thread_flag;
int cliente;
pthread_cond_t thread_flag_cv;
pthread_mutex_t thread_flag_mutex;

struct clientes
{
	int numero;
	struct clientes* next;
};

struct clientes* lista_cliente = NULL;
struct clientes* ultimo_cliente = NULL;

/* Estrutura de um Barbeiro (thread) */
struct char_print_parms
{
	int numeroDoBarbeiro;
};

struct char_print_parms thread_args;

void initialize_flag()
{
	pthread_mutex_init (&thread_flag_mutex, NULL);
	pthread_cond_init (&thread_flag_cv, NULL);
	thread_flag = 0;
}

void cortar_cabelo() {
	usleep(tempoTrabalho * 1000000);
}

void set_thread_flag (int flag_value)
{
	pthread_mutex_lock(&thread_flag_mutex);
	thread_flag = flag_value;
	pthread_cond_signal(&thread_flag_cv);
	pthread_mutex_unlock(&thread_flag_mutex);
}

void* thread_function(void* thread_arg)
{
	struct char_print_parms* arg = (struct char_print_parms *)thread_arg;
	int numeroDoBarbeiro = arg->numeroDoBarbeiro;
	struct clientes* cli = NULL;
	
	while (1) {
		pthread_mutex_lock(&thread_flag_mutex);	
		if(lista_cliente == NULL)
		{
			while(thread_flag == 0)
			{
				printf("O barbeiro %i esta dormindo.\n",numeroDoBarbeiro);
				pthread_cond_wait(&thread_flag_cv,&thread_flag_mutex);
				printf("O barbeiro %i acordou.\n",numeroDoBarbeiro);
			}
		}
		thread_flag--;
		cli = lista_cliente;
		lista_cliente = cli->next;
		pthread_mutex_unlock(&thread_flag_mutex);
		printf("O Barbeiro %i esta cortanto o cabelo do Cliente %i.\n", numeroDoBarbeiro, cli->numero);
		cortar_cabelo();
		printf("O Barbeiro %i acabou de cortar o cabelo do Cliente %i.\n", numeroDoBarbeiro, cli->numero);
	}
	
	return NULL;
}

int main(int argc, char* argv[])
{
	int i;
	pthread_t idBarbeiro[qtdBarbeiros];
	
	/* Setando os parametros */
	qtdBarbeiros = atoi(argv[1]);
	qtdCadeiras = atoi(argv[2]);
	tempoTrabalho = atoi(argv[3]);
	tempoEntreChegadas = atoi(argv[4]);
	
	initialize_flag();
	
	/* Barbeiros trabalhando simultaneamente */
	for(i = 1; i <= qtdBarbeiros; i++)
	{
		thread_args.numeroDoBarbeiro = i;
		pthread_create(&idBarbeiro[i], NULL, &thread_function, &thread_args);
		usleep(100000);
	}
	
	/* Simulando as chegadas dos clientes */
	for(cliente = 1; cliente <= 20; cliente++)
	{
		printf("\n");
		if(thread_flag >= qtdCadeiras)
		{
			printf("Cliente %i chegou, e foi embora sem cortar o cabelo. Sala de espera cheia.\n", cliente);
		}
		else
		{
			if(lista_cliente == NULL)
			{
				struct clientes* cli = malloc(sizeof(struct clientes));
				cli->numero = cliente;
				cli->next = NULL;
				lista_cliente = cli;
				ultimo_cliente = cli;
			}else
			{
				struct clientes* cli = malloc(sizeof(struct clientes));
				cli->numero = cliente;
				cli->next = NULL;
				ultimo_cliente->next = cli;
				ultimo_cliente = cli;
			}
			set_thread_flag(++thread_flag);
			printf("Cliente %i chegou.\n", cliente);
		}
		usleep(tempoEntreChegadas * 1000000);
	}
	usleep(1000000);
	return 0;
}
