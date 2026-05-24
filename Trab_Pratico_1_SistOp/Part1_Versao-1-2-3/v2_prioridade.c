#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

char chat_buffer[100] = "Chat Vazio";
int readcount = 0, writecount = 0;
sem_t rmutex, wmutex, readTry, resource;

void escrever_mensagem(int id) {
    char msg[100];
    sprintf(msg, "Mensagem final do Escritor %d", id);
    for(int i = 0; msg[i] != '\0'; i++) {
        chat_buffer[i] = msg[i];
        usleep(50000);
    }
    chat_buffer[strlen(msg)] = '\0';
}

void* thread_leitora(void* arg) {
    int id = *(int*)arg;
    sleep(rand() % 3);
    printf("[Leitor %d] Criado e aguardando...\n", id);

    sem_wait(&readTry);
    sem_wait(&rmutex);
    readcount++;
    if (readcount == 1) sem_wait(&resource);
    sem_post(&rmutex);
    sem_post(&readTry);

    printf(" -> [Leitor %d] Entrou na Regiao Critica.\n", id);
    printf("    [Leitor %d] LEU: '%s'\n", id, chat_buffer);
    sleep(1);
    printf(" <- [Leitor %d] Saiu da Regiao Critica.\n", id);

    sem_wait(&rmutex);
    readcount--;
    if (readcount == 0) sem_post(&resource);
    sem_post(&rmutex);

    free(arg);
    return NULL;
}

void* thread_escritora(void* arg) {
    int id = *(int*)arg;
    sleep(rand() % 5);
    printf("[Escritor %d] Criado e aguardando...\n", id);

    sem_wait(&wmutex);
    writecount++;
    if (writecount == 1) sem_wait(&readTry);
    sem_post(&wmutex);

    sem_wait(&resource);
    printf(" -> [Escritor %d] Entrou na Regiao Critica.\n", id);
    escrever_mensagem(id);
    printf(" <- [Escritor %d] Atualizou o chat e saiu.\n", id);
    sem_post(&resource);

    sem_wait(&wmutex);
    writecount--;
    if (writecount == 0) sem_post(&readTry);
    sem_post(&wmutex);

    free(arg);
    return NULL;
}

int main() {
    srand(time(NULL));
    int num_leitores = 5, num_escritores = 2;
    pthread_t leitores[num_leitores], escritores[num_escritores];

    sem_init(&rmutex, 0, 1);
    sem_init(&wmutex, 0, 1);
    sem_init(&readTry, 0, 1);
    sem_init(&resource, 0, 1);

    printf("=== V2: PRIORIDADE P/ ESCRITORES (Sem Leitura Suja) ===\n\n");

    for (int i = 0; i < num_leitores; i++) {
        int* id = malloc(sizeof(int)); *id = i + 1;
        pthread_create(&leitores[i], NULL, thread_leitora, id);
    }
    for (int i = 0; i < num_escritores; i++) {
        int* id = malloc(sizeof(int)); *id = i + 1;
        pthread_create(&escritores[i], NULL, thread_escritora, id);
    }

    for (int i = 0; i < num_leitores; i++) pthread_join(leitores[i], NULL);
    for (int i = 0; i < num_escritores; i++) pthread_join(escritores[i], NULL);

    sem_destroy(&rmutex); sem_destroy(&wmutex);
    sem_destroy(&readTry); sem_destroy(&resource);

    printf("\nProcessamento concluido. Chat: '%s'\n", chat_buffer);
    return 0;
}