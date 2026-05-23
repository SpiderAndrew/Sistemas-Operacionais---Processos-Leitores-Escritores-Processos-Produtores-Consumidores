#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define TAMANHO_BUFFER 5 // Tamanho da fila do chat (pode alterar se quiser)

// --- DADOS COMPARTILHADOS (BUFFER) ---
char buffer_chat[TAMANHO_BUFFER][100];
int in = 0;  // Índice onde o próximo produtor vai inserir
int out = 0; // Índice de onde o consumidor vai retirar

// --- MECANISMOS DE SINCRONIZAÇÃO ---
sem_t sem_vazio;       // Conta os espaços vazios (bloqueia produtor se 0)
sem_t sem_cheio;       // Conta os espaços preenchidos (bloqueia consumidor se 0)
pthread_mutex_t mutex; // Garante exclusão mútua na hora de mexer no buffer

// --- CONFIGURAÇÕES ---
int max_atraso_produtor;
int max_atraso_consumidor;
int num_produtores;
int msgs_por_produtor = 2; // Quantas mensagens cada produtor vai mandar

// Função auxiliar para desenhar o Buffer na tela
void mostrar_buffer() {
    printf("   [Fila do Servidor] [ ");
    for(int i = 0; i < TAMANHO_BUFFER; i++) {
        if(strlen(buffer_chat[i]) > 0) 
            printf("'%s' ", buffer_chat[i]);
        else 
            printf("(Vazio) ");
    }
    printf("]\n\n");
}

// --- THREADS PRODUTORAS (Usuários mandando mensagem) ---
void* thread_produtora(void* arg) {
    int id = *(int*)arg;

    for (int i = 0; i < msgs_por_produtor; i++) {
        // Atraso gerando a mensagem (tempo que o usuário digita)
        sleep(rand() % (max_atraso_produtor > 0 ? max_atraso_produtor : 1));

        char msg[100];
        sprintf(msg, "Msg %d do Produtor %d", i + 1, id);

        // Verifica se o buffer está cheio para avisar que vai dormir
        int vagas;
        sem_getvalue(&sem_vazio, &vagas);
        if (vagas == 0) {
            printf("[Produtor %d] Buffer CHEIO! Vou dormir aguardando espaco...\n", id);
        }

        sem_wait(&sem_vazio); // Aguarda ter espaço vazio
        pthread_mutex_lock(&mutex); // Entra na Região Crítica

        printf("[Produtor %d produzindo] Inseriu no chat: '%s'\n", id, msg);
        strcpy(buffer_chat[in], msg);
        in = (in + 1) % TAMANHO_BUFFER;
        mostrar_buffer();

        pthread_mutex_unlock(&mutex); // Sai da Região Crítica
        sem_post(&sem_cheio); // Avisa ao consumidor que tem mensagem nova
    }

    free(arg);
    return NULL;
}

// --- THREAD CONSUMIDORA (Servidor salvando histórico) ---
void* thread_consumidora(void* arg) {
    // O consumidor precisa rodar até processar todas as mensagens geradas
    int total_mensagens = num_produtores * msgs_por_produtor;

    for (int i = 0; i < total_mensagens; i++) {
        // Verifica se o buffer está vazio para avisar que vai dormir
        int mensagens_na_fila;
        sem_getvalue(&sem_cheio, &mensagens_na_fila);
        if (mensagens_na_fila == 0) {
            printf("[Consumidor] Fila VAZIA! Servidor dormindo aguardando mensagens...\n");
        }

        sem_wait(&sem_cheio); // Aguarda ter mensagem na fila
        pthread_mutex_lock(&mutex); // Entra na Região Crítica

        printf("[Consumidor processando] Retirou do chat: '%s'\n", buffer_chat[out]);
        strcpy(buffer_chat[out], ""); // Limpa o espaço (opcional, apenas para o print ficar bonito)
        out = (out + 1) % TAMANHO_BUFFER;
        mostrar_buffer();

        pthread_mutex_unlock(&mutex); // Sai da Região Crítica
        sem_post(&sem_vazio); // Avisa aos produtores que liberou espaço

        // Atraso para processar a gravação no histórico
        sleep(rand() % (max_atraso_consumidor > 0 ? max_atraso_consumidor : 1));
    }

    return NULL;
}

int main() {
    srand(time(NULL));

    // Inicializa o buffer com strings vazias
    for(int i = 0; i < TAMANHO_BUFFER; i++) strcpy(buffer_chat[i], "");

    printf("=== SIMULADOR DE PRODUTOR-CONSUMIDOR (Servidor de Chat) ===\n");
    
    printf("Digite a quantidade de threads Produtoras (usuarios): ");
    fflush(stdout);
    scanf("%d", &num_produtores);
    
    printf("Digite o atraso maximo (segundos) dos Produtores: ");
    fflush(stdout);
    scanf("%d", &max_atraso_produtor);
    
    printf("Digite o atraso maximo (segundos) do Consumidor (Servidor): ");
    fflush(stdout);
    scanf("%d", &max_atraso_consumidor);

    // INICIALIZAÇÃO:
    // sem_vazio começa com o tamanho do buffer (todas as vagas livres)
    // sem_cheio começa em 0 (nenhuma mensagem ainda)
    sem_init(&sem_vazio, 0, TAMANHO_BUFFER);
    sem_init(&sem_cheio, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    printf("\n=== INICIANDO EXECUCAO ===\n\n");

    pthread_t produtores[num_produtores];
    pthread_t consumidor;

    // Cria a thread do Consumidor (apenas 1, conforme requisito)
    pthread_create(&consumidor, NULL, thread_consumidora, NULL);

    // Cria as threads dos Produtores (várias)
    for (int i = 0; i < num_produtores; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&produtores[i], NULL, thread_produtora, id);
    }

    // Aguarda todos finalizarem
    for (int i = 0; i < num_produtores; i++) {
        pthread_join(produtores[i], NULL);
    }
    pthread_join(consumidor, NULL);

    // Limpeza
    sem_destroy(&sem_vazio);
    sem_destroy(&sem_cheio);
    pthread_mutex_destroy(&mutex);

    printf("\nProcessamento concluido. Servidor finalizado.\n");
    return 0;
}