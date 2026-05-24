Repositório do Trabalho Prático 1 de Sistemas Operacionais (UFAM). Implementação concorrente em C (pthread e semáforos) abordando dois problemas clássicos de sincronização: Leitores e Escritores (contextualizado em Servidor com um Histórico de Chat) e Produtores e Consumidores (usando um Buffer Circular). Contém versões com e sem controle de concorrência e simulação de atrasos.

1. Guia de ambientação e execução (Github Codespace) 

O projeto foi totalmente projetado e homologado para rodar em arquitetura Linux nativa através do GitHub Codespaces, dispensando configurações locais complexas em ambiente Windows.

1.1. Preparação do Ambiente
Abra o terminal integrado do seu Codespace e garanta o ferramental básico de compilação C rodando:
sudo apt-get update && sudo apt-get install -y build-essential unzip

1.2. Compilação das Versões
Devido à arquitetura POSIX do Linux, a vinculação das bibliotecas de concorrência exige a flag -lpthread acoplada ao GCC.

Parte 1: Leitores e Escritores
- Versão 1: gcc v1_sem_prioridade.c -o v1_sem_prioridade -lpthread && ./v1_sem_prioridade
- Versão 2: gcc v2_prioridade.c -o v2_prioridade -lpthread && ./v2_prioridade
- Versão 3: gcc v3_caos.c -o v3_caos -lpthread && ./v3_caos

Parte 2: Produtores e Consumidores
- Versão 1: gcc v1_ProdCons_1consumidor.c -o v1_prodcons_1cons -lpthread && ./v1_prodcons_1cons
- Versão 2: gcc v2_ProdCons_VáriosCons.c -o v2_prodcons_varios -lpthread && ./v2_prodcons_varios
- Versão 3: gcc v3_ProdCons_SemControle.c -o v3_prodcons_caos -lpthread && ./v3_prodcons_caos

2. Guia de ambientação e execução local (Via clone do git)

Caso prefira não utilizar o ambiente em nuvem do GitHub Codespaces e opte por clonar o repositório para testar os códigos diretamente em sua máquina local, siga as diretrizes abaixo. Observação sobre Portabilidade (POSIX): Este projeto foi desenvolvido utilizando as bibliotecas <pthread.h> e <semaphore.h>, que seguem o padrão POSIX. Sistemas Linux e macOS possuem suporte nativo. Caso esteja utilizando o Windows, a execução nativa via CMD/PowerShell convencional falhará por falta dessas bibliotecas; portanto, é obrigatório o uso do WSL (Windows Subsystem for Linux) ou de ambientes como o MinGW-w64/Cygwin.

2.1. Clonando o Repositório
Abra o terminal da sua máquina (ou o terminal do WSL/Linux) e execute o comando de clonagem:
git clone https://github.com/SpiderAndrew/Sistemas-Operacionais---Processos-Leitores-Escritores-Processos-Produtores-Consumidores.git
Em seguida, navegue para a pasta raiz do projeto:
cd Trab_Prático_1_SistOp 

2.2. Verificação e Instalação de Dependências
No Linux (Ubuntu / Debian e derivados) ou WSL: Garante o instalador de pacotes atualizado, o compilador GCC e a suíte de desenvolvimento essencial.

sudo apt-get update && sudo apt-get install -y build-essential

No macOS: As bibliotecas POSIX são nativas, mas necessitam das ferramentas de linha de comando do Xcode. Caso não as tenha, instale via terminal:

xcode-select --install

No Windows (Sem WSL - Via MinGW-w64): Caso opte por usar o MinGW explicitamente, certifique-se de instalar a versão que possua suporte a POSIX Threads (frequentemente identificada como mingw-w64-...-posix-seh) e adicione o diretório bin às variáveis de ambiente (PATH) do sistema.

2.3. Roteiro de Compilação e Execução Local
Os códigos estão organizados modularmente em subpastas. antes de usar os comandos abaixo, certifique-se de ter criado um terminal integrado para cada pasta, para isso, basta selecionar com o seu mouse por cima da pasta que deseja fazer os testes, Clique com o botão direito do mouse em cima da pasta Part1_Versao-1-2-3 ou Part2_Versao-1-2-3 e selecione a opção Open in Integrated Terminal (Abrir no Terminal Integrado). Para compilar, certifique-se de incluir a flag -lpthread ao final do comando para que o linker do GCC possa vincular corretamente a biblioteca de concorrência.

Exemplo 1: Testando a Parte 1 (Leitores e Escritores - Versão 1)
Use o seguinte comando em seu terminal:
gcc v1_sem_prioridade.c -o v1_sem_prioridade -lpthread 
./v1_sem_prioridade

Exemplo 2: Testando a Parte 1 (Leitores e Escritores - Versão 2)
Use o seguinte comando em seu terminal:
gcc v2_prioridade.c -o v2_prioridade -lpthread
./v2_prioridade

Exemplo 3: Testando a Parte 1 (Leitores e Escritores - Versão 3)
Use o seguinte comando em seu terminal:
gcc v3_caos.c -o v3_caos -lpthread
./v3_caos

Exemplo 4: Testando a Parte 1 (Produtores e Consumidores - Versão 1)
Use o seguinte comando em seu terminal:
gcc v1_ProdCons_1consumidor.c -o v1_prodcons_1cons -lpthread
./v1_prodcons_1cons

Exemplo 5: Testando a Parte 1 (Produtores e Consumidores - Versão 2)
Use o seguinte comando em seu terminal:
gcc v2_ProdCons_VáriosCons.c -o v2_prodcons_varios -lpthread
./v2_prodcons_varios

Exemplo 6: Testando a Parte 2 (Produtores e Consumidores - Versão 3)
Use o seguinte comando em seu terminal:
gcc v3_ProdCons_SemControle.c -o v3_prodcons_caos -lpthread
./v3_prodcons_caos

3. Exemplificação dos Testes de Funcionamento

*Parte 1: Leitores e Escritores*
- Teste 1: A Prova da "Leitura Suja" e Inanição do Escritor (Parte 1 - Versão 1)

O que digitar: 10 threads LEITORAS, 1 thread ESCRITORA.

O que observar: O código usa um semáforo que protege o dado, mas o acesso ao semáforo é monopolizado pelos leitores contínuos. O Escritor fica bloqueado (sofrendo starvation) enquanto leitores entram na região crítica um após o outro. Todos leem a mensagem inicial antiga, provando que o escritor perdeu a vez e os leitores o ignoraram.

- Teste 2: A Prova da Concorrência de Leitura (Parte 1 - Versão 1)

O que digitar: 5 threads LEITORAS, 0 threads ESCRITORAS.

O que observar: Os leitores entram na região crítica simultaneamente. Como o bloqueio (mutex) só afeta o primeiro leitor a entrar e o último a sair, os demais acessam o chat livremente, sem bloqueio entre si, validando a natureza paralela da leitura.

- Teste 3: Prioridade Absoluta e Exclusão Mútua (Parte 1 - Versão 2)

O que digitar: 5 threads LEITORAS, 2 threads ESCRITORAS.

O que observar: Graças à implementação de múltiplos semáforos (incluindo o readTry e wmutex), os escritores agora têm prioridade. Quando um escritor sinaliza que quer escrever, novos leitores são impedidos de entrar na região crítica. O programa exibe uma pausa clara durante a escrita e garante que a leitura sempre traga o dado atualizado, eliminando a "leitura suja" do Teste 1.

- Teste 4: A Prova do Caos Simutâneo (Parte 1 - Versão 3)

O que observar: Como não há mecanismos de controle (semáforos/mutex removidos no código v3_caos.c), ao executar múltiplas threads simultâneas, você verá leitores entrando na região crítica no exato momento em que os escritores ainda estão montando a string caractere por caractere. O resultado é a leitura de dados corrompidos ou incompletos na tela.

*Parte 2: Produtores e Consumidores*
- Teste 5: Forçar o Consumidor a Dormir / Fila Vazia (Parte 2 - Versão 1)

O que digitar: Qtd Produtoras: 2 | Atraso Produtores: 4s | Atraso Consumidor: 0s.

O que observar: Como o servidor (consumidor) não tem atraso, ele esvazia a fila do chat instantaneamente e entra em estado de bloqueio (dormindo), aguardando novos dados. Isso demonstra o funcionamento rigoroso do semáforo sem_cheio, que impede processamento em buffers vazios (evitando busy wait).

- Teste 6: Alta Concorrência e Estresse (Parte 2 - Versão 2)

O que editar no código (v2_ProdCons_VáriosCons.c): Ajustar #define NUM_USUARIOS 10 e #define NUM_PROCESSADORES 10.

O que observar: O buffer circular de tamanho 20 mantém um equilíbrio dinâmico, oscilando em níveis intermediários. O mutex utilizado na região crítica do array garante que, apesar da altíssima disputa de 20 threads tentando acessar a variável global de controle de índice ao mesmo tempo, nenhum dado seja sobrescrito incorretamente. As mensagens entram e saem em perfeita ordem.

- Teste 7: A Prova do "Caos" por Condição de Corrida (Parte 2 - Versão 3)

O que editar no código (v3_ProdCons_SemControle.c): Ajustar #define TAM_BUFFER 3 (um buffer pequeno propositalmente) sem atrasos.

O que observar: Como a versão 3 tem as chamadas de pthread_mutex_lock, sem_wait e sem_post comentadas (desativadas), o caos ocorre rapidamente. Você verá os consumidores lendo lixo de memória (posições vazias), variáveis de contador estourando limites absurdos maiores que o tamanho do buffer (ex: contador indo a 5, 10 num buffer de tamanho 3) e produtores sobrescrevendo mensagens antes que o servidor consiga processá-las
