#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

        //VERSÃO COM CONTROLE E VARIOS PRODUTORES E VARIOS CONSUMIDORES


#define TAM_BUFFER 20 // Tamanho do buffer compartilhado 
#define NUM_USUARIOS 4 // Número de threads produtoras (usuários)
#define NUM_PROCESSADORES 4 // Número de threads consumidoras (processadores)

// Buffer compartilhado
char buffer[TAM_BUFFER][100];


int inicio = 0; 
int fim = 0;
int contador = 0;

int max_atraso_produtor;
int max_atraso_consumidor;
int num_usuarios;
int num_processadores;
int msgs_por_produtor = 20;

// Controle global de IDs
// int proxIdMensagem = 1;

// Mutex
pthread_mutex_t mutex; // Garante exclusão mútua na hora de mexer no buffer

// Semáforos
sem_t vazio; // Conta os espaços vazios (bloqueia produtor se 0)
sem_t cheio; // Conta os espaços preenchidos (bloqueia consumidor se 0)

// Verifica se buffer está vazio
// int BufferVazio() {

//     if(contador == 0)
//         return 1;

//     return 0;
// }

// // Verifica se buffer está cheio
// int BufferCheio() {

//     if(contador == TAM_BUFFER)
//         return 1;

//     return 0;
// }

// Exibe estado atual do buffer
void mostrarBuffer() {

    printf("\n======= SERVIDOR =======\n");

    printf("Inicio: %d\n", inicio);
    printf("Fim: %d\n", fim);
    printf("Contador: %d\n\n", contador);

    for(int i = 0; i < contador; i++) {

        printf("[%d] %s\n",
               (inicio + i) % TAM_BUFFER,
               buffer[(inicio + i) % TAM_BUFFER]);
    }

    printf("========================\n\n");
}


// THREAD USUARIO (PRODUTOR)
void* funcaoUsuario(void* arg) {

    int id = *((int*) arg);

    for(int i = 0; i < msgs_por_produtor; i++) {
        // Gerar texto da mensagem
        sleep(rand() % (max_atraso_produtor > 0 ? max_atraso_produtor : 1));

        char texto[100];
        sprintf(texto,
                "Mensagem %d do Usuario %d",
                i + 1,
                id);

        int espaco; // Verifica se o buffer está cheio para avisar que vai dormir
        sem_getvalue(&vazio, &espaco);
        if (espaco == 0) {
            printf("[USUARIO %d] Buffer CHEIO! Vou dormir (aguarda espaco)...\n",
                   id);
        }

        sem_wait(&vazio);

printf("[USUARIO %d] tentando entrar na regiao critica\n", id);

pthread_mutex_lock(&mutex);

printf("[USUARIO %d] ENTROU na regiao critica\n", id);

        printf("[USUARIO %d] inseriu mensagem: '%s'\n",
               id,
               texto);
        strcpy(buffer[fim], texto);
    
        fim = (fim + 1) % TAM_BUFFER;
        contador++;
        mostrarBuffer();
            printf("[USUARIO %d] SAIU da regiao critica\n", id);
        pthread_mutex_unlock(&mutex); // sai da região crítica
        sem_post(&cheio); // avisa consumidor 

    }
    free(arg); // Libera memória alocada para o ID do usuário
    return NULL; // Encerra a thread do usuário
}   

// THREAD PROCESSADOR (CONSUMIDOR)
void* funcaoProcessador(void* arg) {

    int total_mensagens = num_usuarios * msgs_por_produtor;

    int idProcessador = *((int*) arg);

    int mensagens_por_processador = (total_mensagens + num_processadores - 1) / num_processadores;

    for(int i = 0; i < mensagens_por_processador; i++) {
        // Verifica se o buffer está vazio para avisar que vai dormir
        int msg_na_fila;
        sem_getvalue(&cheio, &msg_na_fila);
        if (msg_na_fila == 0) {
            printf("[PROCESSADOR %d] Fila VAZIA! Servidor dormindo (aguarda mensagens)...\n",
                   idProcessador);
        }

        // espera mensagem
       sem_wait(&cheio);

printf("[PROCESSADOR %d] tentando entrar na regiao critica\n",
       idProcessador);

pthread_mutex_lock(&mutex);

printf("[PROCESSADOR %d] ENTROU na regiao critica\n",
       idProcessador);

        printf("Processador removeu do buffer: '%s'\n",
               buffer[inicio]);
               strcpy(buffer[inicio], ""); // Limpa a mensagem do buffer
        inicio = (inicio + 1) % TAM_BUFFER;
        contador--;
        mostrarBuffer();

        printf("[PROCESSADOR %d] SAIU da regiao critica\n",
       idProcessador);
        pthread_mutex_unlock(&mutex); // sai da região crítica
        sem_post(&vazio); // avisa produtor que liberou espaço
            sleep(1);
        sleep(rand() % (max_atraso_consumidor > 0 ? max_atraso_consumidor : 1)); // Atraso para processar a mensagem
    }
    free(arg); // Libera memória alocada para o ID do processador
    return NULL; // Encerra a thread do processador
}

int main() {

    srand(time(NULL)); // Semente para geração de números aleatórios

    for(int i = 0; i < TAM_BUFFER; i++) {
        strcpy(buffer[i], ""); // Inicializa o buffer com strings vazias
    }

    printf("=== (Servidor de Chat) ===\n");

    printf("Digite o número de processadores: ");
    fflush(stdout);
    scanf("%d", &num_processadores);

    printf("Digite o número de produtores (usuários): ");
    fflush(stdout);
    scanf("%d", &num_usuarios);

    printf("Digite o atraso máximo para os produtores (em segundos): ");
    fflush(stdout);
    scanf("%d", &max_atraso_produtor);

    printf("Digite o atraso máximo para os consumidores (em segundos): ");
    fflush(stdout);
    scanf("%d", &max_atraso_consumidor);


    // inicializa semáforos
    sem_init(&vazio, 0, TAM_BUFFER);

    sem_init(&cheio, 0, 0);

    // inicializa mutex
    pthread_mutex_init(&mutex, NULL);

    printf("SERVIDOR INICIANDO!\n\n");

    pthread_t threadsUsuarios[num_usuarios];
    pthread_t threadsProcessadores[num_processadores];

   
    // cria threads usuários
    for(int i = 0; i < num_usuarios; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(
            &threadsUsuarios[i],
            NULL,
            funcaoUsuario,
            id
        );
    }

    // cria threads processadores
    for(int i = 0; i < num_processadores; i++) {
        int* idProcessador = malloc(sizeof(int));
        *idProcessador = i + 1;
        // idsProcessadores[i] = i + 1;

        pthread_create(
            &threadsProcessadores[i],
            NULL,
            funcaoProcessador,
            idProcessador
        );
    }

    // espera threads
    for(int i = 0; i < num_usuarios; i++) { // Espera as threads produtoras terminarem
        pthread_join(
            threadsUsuarios[i],
            NULL
        );  
    }

    for(int i = 0; i < num_processadores; i++) {   // Espera as threads processadoras terminarem

        pthread_join(
            threadsProcessadores[i],
            NULL
        );
    }

    // destrói recursos
    sem_destroy(&vazio);

    sem_destroy(&cheio);

    pthread_mutex_destroy(&mutex);

    printf("Processamento terminado. Servidor finalizado.\n"); // Mensagem final indicando que o servidor foi finalizado

    return 0;
}
