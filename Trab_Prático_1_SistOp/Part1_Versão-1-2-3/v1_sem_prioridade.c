#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

// 1. Estrutura da Variável Compartilhada (O Histórico do Chat)
typedef struct {
    int id_mensagem;
    char conteudo[256];
} ChatHistory;

ChatHistory chat_global; // Variável compartilhada por todas as threads

// 2. Variáveis de Controle e Semáforos
int leitores_ativos = 0;
sem_t mutex;  // Protege o contador 'leitores_ativos'
sem_t db_sem; // Protege o acesso ao 'chat_global'

// Array de mensagens simulando o que os escritores vão digitar
const char* mensagens_mock[] = {
    "Bem-vindos ao servidor!",
    "Manutencao agendada para as 22h.",
    "O servidor sera reiniciado em 5 minutos.",
    "Atualizacao concluida com sucesso."
};

// 3. Função da Thread Leitora
void* leitor(void* arg) {
    int id = *((int*)arg);
    
    printf("[Leitor %d] CRIADO. Tentando acessar o chat...\n", id);
    printf("[Leitor %d] BLOQUEADO aguardando acesso...\n", id);

    // Protocolo de Entrada
    sem_wait(&mutex); // Bloqueia o acesso ao contador
    leitores_ativos++;
    if (leitores_ativos == 1) {
        // Se for o primeiro leitor, tranca a porta para os escritores
        sem_wait(&db_sem); 
    }
    sem_post(&mutex); // Libera o acesso ao contador

    // --- REGIÃO CRÍTICA (Lendo o Chat) ---
    printf("[Leitor %d] ENTROU na regiao critica.\n", id);
    printf("   -> ACAO [Leitor %d]: Leu a Mensagem %d - \"%s\"\n", id, chat_global.id_mensagem, chat_global.conteudo);
    
    sleep(1); // Atraso: Leitores são rápidos
    
    printf("[Leitor %d] SAIU da regiao critica.\n", id);
    // -------------------------------------

    // Protocolo de Saída
    sem_wait(&mutex);
    leitores_ativos--;
    if (leitores_ativos == 0) {
        // Se for o último leitor a sair, destranca a porta para os escritores
        sem_post(&db_sem); 
    }
    sem_post(&mutex);

    printf("[Leitor %d] FINALIZADO.\n", id);
    free(arg);
    return NULL;
}

// 4. Função da Thread Escritora
void* escritor(void* arg) {
    int id = *((int*)arg);
    
    printf("[Escritor %d] CRIADO. Preparando para enviar mensagem...\n", id);
    printf("[Escritor %d] BLOQUEADO aguardando acesso exclusivo...\n", id);

    // Protocolo de Entrada (Escritor precisa do acesso total)
    sem_wait(&db_sem);

    // --- REGIÃO CRÍTICA (Atualizando o Chat) ---
    printf("[Escritor %d] ENTROU na regiao critica.\n", id);
    
    // Atualiza os dados compartilhados
    chat_global.id_mensagem++;
    // Escolhe uma mensagem do mock com base no ID para variar
    strcpy(chat_global.conteudo, mensagens_mock[id % 4]); 
    
    printf("   -> ACAO [Escritor %d]: Atualizou o chat para: \"%s\"\n", id, chat_global.conteudo);
    
    sleep(3); // Atraso: Escritores são mais lentos que leitores, como pedido nas orientacoes
    
    printf("[Escritor %d] SAIU da regiao critica.\n", id);
    // -------------------------------------

    // Protocolo de Saída
    sem_post(&db_sem); // Libera o acesso para outros leitores ou escritores

    printf("[Escritor %d] FINALIZADO.\n", id);
    free(arg);
    return NULL;
}

// 5. Função Principal (Recebendo a definição dos processos)
int main() {
    int qtd_leitores, qtd_escritores;

    printf("=== SERVIDOR DE CHAT (V1: Sem Prioridade) ===\n");
    printf("Digite a quantidade de threads LEITORAS: ");
    scanf("%d", &qtd_leitores);
    printf("Digite a quantidade de threads ESCRITORAS: ");
    scanf("%d", &qtd_escritores);

    // Inicializa a variável compartilhada
    chat_global.id_mensagem = 0;
    strcpy(chat_global.conteudo, "Servidor iniciado. Nenhuma mensagem ainda.");

    // Inicializa os semáforos
    // sem_init(ponteiro, 0 para threads do mesmo processo, valor_inicial)
    sem_init(&mutex, 0, 1);
    sem_init(&db_sem, 0, 1);

    pthread_t leitores[qtd_leitores];
    pthread_t escritores[qtd_escritores];

    // Cria as threads de forma intercalada para gerar concorrência
    int max_threads = (qtd_leitores > qtd_escritores) ? qtd_leitores : qtd_escritores;
    
    for (int i = 0; i < max_threads; i++) {
        if (i < qtd_leitores) {
            int* id = malloc(sizeof(int));
            *id = i + 1;
            pthread_create(&leitores[i], NULL, leitor, id);
        }
        if (i < qtd_escritores) {
            int* id = malloc(sizeof(int));
            *id = i + 1;
            pthread_create(&escritores[i], NULL, escritor, id);
        }
    }

    // Aguarda todas as threads finalizarem
    for (int i = 0; i < qtd_leitores; i++) {
        pthread_join(leitores[i], NULL);
    }
    for (int i = 0; i < qtd_escritores; i++) {
        pthread_join(escritores[i], NULL);
    }

    // Destroi os semáforos para limpar a memória
    sem_destroy(&mutex);
    sem_destroy(&db_sem);

    printf("=== EXECUCAO FINALIZADA ===\n");
    return 0;
}