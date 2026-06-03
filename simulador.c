#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Premissas usadas na construção do código
#define MAX_PROCESSOS 8
#define QUANTUM 5
#define CHANCE_IO 50

// Valores para identificar o tipo de prioridade de cada I/O
#define IO_DISCO 1
#define IO_FITA 2
#define IO_IMPRESSORA 3

// Tempo de cada I/O
#define TEMPO_IO_DISCO 4
#define TEMPO_IO_FITA 2
#define TEMPO_IO_IMPRESSORA 3

// Função para esperar tecla multiplataforma
#ifdef _WIN32
    #include <conio.h>
    void esperar_tecla() {
        getch();
    }
#else
    void esperar_tecla() {
        int c;
        // Limpa qualquer caractere no buffer
        while ((c = getchar()) != '\n' && c != EOF);
    }
#endif

typedef struct Processo {
    int pid;
    int ppid;
    int prioridade; // 0 = alta, 1 = baixa
    int tempo_restante;
    int tempo_executado;
    int status;     // 0 = pronto, 1 = executando, 2 = I/O, 3 = finalizado
    int tempo_io_restante;
    int tipo_io;
    int instante_io;
    struct Processo* prox;
} Processo;

typedef struct {
    Processo* inicio;
    Processo* fim;
} Fila;

int proximo_pid=1; // variável para controlar os pid's

void inicializar_fila(Fila* fila) {
    fila->inicio = fila->fim = NULL;
}

void enfileirar(Fila* fila, Processo* p) {
    p->prox = NULL;
    if (fila->fim == NULL) {
        fila->inicio = fila->fim = p;
    } else {
        fila->fim->prox = p;
        fila->fim = p;
    }
}

Processo* desenfileirar(Fila* fila) {
    if (fila->inicio == NULL) return NULL;
    Processo* p = fila->inicio;
    fila->inicio = fila->inicio->prox;
    if (fila->inicio == NULL) fila->fim = NULL;
    return p;
}

Processo* criar_novo_processo(int ppid){
    int tempo_servico = rand() % 16 + 5; // 5 a 20 unidades

    Processo* p = malloc(sizeof(Processo));
    p->pid = proximo_pid++;
    p->ppid = ppid;
    p->tempo_restante = tempo_servico; 
    p->tempo_executado = 0;
    p->prioridade = 0;
    p->status = 0;

    // Define se o processo terá I/O baseado na CHANCE_IO
    if (rand() % 100 < CHANCE_IO) {
        p->tipo_io = (rand() % 3) + 1; // valores de 1 a 3
        p->instante_io = (rand() % (tempo_servico - 1)) + 1; // valores de 1 a tempo_serviço-1
    } else {
        p->tipo_io = 0;
        p->instante_io = 0; 
    }

    p->tempo_io_restante = 0;

    return p;
}

// Função para imprimir as filas
void imprimir_filas(Fila* alta, Fila* baixa, Fila* io_disco, Fila* io_fita, Fila* io_impressora) {
    printf("\n=== Filas: ===\n");
    
    // Fila alta prioridade
    printf("  Alta prioridade: ");
    Processo* p = alta->inicio;
    if (p == NULL) {
        printf("vazia");
    } else {
        while (p != NULL) {
            printf("P%d(%d) ", p->pid, p->tempo_restante);
            p = p->prox;
        }
    }
    printf("\n");
    
    // Fila baixa prioridade
    printf("  Baixa prioridade: ");
    p = baixa->inicio;
    if (p == NULL) {
        printf("vazia");
    } else {
        while (p != NULL) {
            printf("P%d(%d) ", p->pid, p->tempo_restante);
            p = p->prox;
        }
    }
    printf("\n");
    
    // Filas de I/O
    printf("  I/O Disco: ");
    p = io_disco->inicio;
    if (p == NULL) {
        printf("vazia");
    } else {
        while (p != NULL) {
            printf("P%d(%d) ", p->pid, p->tempo_io_restante);
            p = p->prox;
        }
    }
    printf("\n");
    
    printf("  I/O Fita: ");
    p = io_fita->inicio;
    if (p == NULL) {
        printf("vazia");
    } else {
        while (p != NULL) {
            printf("P%d(%d) ", p->pid, p->tempo_io_restante);
            p = p->prox;
        }
    }
    printf("\n");
    
    printf("  I/O Impressora: ");
    p = io_impressora->inicio;
    if (p == NULL) {
        printf("vazia");
    } else {
        while (p != NULL) {
            printf("P%d(%d) ", p->pid, p->tempo_io_restante);
            p = p->prox;
        }
    }
    printf("\n");
}

// Função para processar uma fila de I/O específica
int processar_fila_io(Fila* fila_io, Fila* alta, Fila* baixa, int tipo_io, const char* nome_io) {
    Fila fila_aux;
    inicializar_fila(&fila_aux);

    int fim = 0;
    
    while (fila_io->inicio) {
        Processo* p = desenfileirar(fila_io);
        p->tempo_io_restante--;
        
        if (p->tempo_io_restante <= 0) {
            printf("Processo %d terminou I/O de %s\n", p->pid, nome_io);
            p->status = 0; // volta para pronto
            p->tipo_io = 0; // zera o tipo_io do processo
            
            // Decide para qual fila retorna baseado no tipo de I/O
            if (tipo_io == IO_DISCO) {
                p->prioridade = 1; // baixa prioridade 
                enfileirar(baixa, p);
                printf("Processo %d retornou para fila de baixa prioridade\n", p->pid);
                fim = 1;
            } else {
                p->prioridade = 0; // alta prioridade
                enfileirar(alta, p);
                printf("Processo %d retornou para fila de alta prioridade\n", p->pid);
                fim = 1;
            }
        } else {
            enfileirar(&fila_aux, p);
        }
    }
    
    *fila_io = fila_aux;
    return fim;
}

void limpa_tela(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

int contar_processos_ativos(Fila* alta, Fila* baixa, Fila* io_disco, Fila* io_fita, Fila* io_impressora) {
    int count = 0;
    Fila* filas[] = {alta, baixa, io_disco, io_fita, io_impressora};
    for (int i = 0; i < 5; i++) {
        Processo* p = filas[i]->inicio;
        while (p != NULL) {
            count++;
            p = p->prox;
        }
    }
    return count;
}

void simular(int modo_passo_a_passo) {

    // Inicializa filas
    Fila alta, baixa, io_disco, io_fita, io_impressora;
    inicializar_fila(&alta);
    inicializar_fila(&baixa);
    inicializar_fila(&io_disco);
    inicializar_fila(&io_fita);
    inicializar_fila(&io_impressora);

    // Cria processos iniciais
    for (int i = 0; i < 5; i++) {
        Processo* p = criar_novo_processo(1);

        if (p->tipo_io == 0) {
            printf("Processo %d criado: tempo=%d, prioridade=alta, SEM I/O\n",
                   p->pid, p->tempo_restante);
        } else {
            printf("Processo %d criado: tempo=%d, prioridade=alta, I/O=%s no instante %d\n",
                   p->pid, p->tempo_restante, 
                   (p->tipo_io == IO_DISCO) ? "Disco" : (p->tipo_io == IO_FITA) ? "Fita" : "Impressora",
                   p->instante_io);
        }

        enfileirar(&alta, p); // todo processo novo vai para fila alta
    }

    imprimir_filas(&alta, &baixa, &io_disco, &io_fita, &io_impressora);
    
    int ciclo = 1;
    while (alta.inicio || baixa.inicio || io_disco.inicio || io_fita.inicio || io_impressora.inicio) {
        

        // Se for modo passo a passo, espera o usuário apertar uma tecla
        if (modo_passo_a_passo) {
            printf("\n[Pressione ENTER para continuar...]");
            esperar_tecla();
        }
        
        printf("\n[Ciclo %d]\n", ciclo);

        // Processa todas as filas de I/O
        int fim_disco = processar_fila_io(&io_disco, &alta, &baixa, IO_DISCO, "Disco");
        int fim_fita = processar_fila_io(&io_fita, &alta, &baixa, IO_FITA, "Fita");
        int fim_impressora = processar_fila_io(&io_impressora, &alta, &baixa, IO_IMPRESSORA, "Impressora");

        if(fim_disco == 1 || fim_fita == 1 || fim_impressora == 1){
            imprimir_filas(&alta, &baixa, &io_disco, &io_fita, &io_impressora);
            printf("\n");
        }

        // Executa um processo (dando prioridade para a fila alta)
        Processo* atual = desenfileirar(&alta);
        if (atual == NULL) 
            atual = desenfileirar(&baixa); // se não há processos na fila alta, vai para a fila baixa

        if (atual) {
            atual->status = 1;
            int tempo_exec = (atual->tempo_restante < QUANTUM) ? atual->tempo_restante : QUANTUM;
            
            // Verifica se o I/O vai ocorrer durante este quantum
            int tempo_ate_io = atual->instante_io - atual->tempo_executado;
            int falta_io = (atual->tipo_io != 0);
            
            // Se ainda não fez I/O e vai fazer durante este quantum, executa só até o I/O
            if (falta_io && tempo_ate_io < tempo_exec) {
                tempo_exec = tempo_ate_io;
            }
            
            printf("Executando processo %d (prioridade %s) por %d unidades\n",
                   atual->pid, atual->prioridade == 0 ? "alta" : "baixa", tempo_exec);
            
            atual->tempo_restante -= tempo_exec;
            atual->tempo_executado += tempo_exec;

            if (atual->tempo_restante <= 0) {
                printf("Processo %d finalizado.\n", atual->pid);
                atual->status = 3;
                free(atual);
            } else {
                // Verifica se chegou o instante do I/O
                if (falta_io && atual->tempo_executado == atual->instante_io) {
                    atual->status = 2;
                    switch (atual->tipo_io) {
                        case IO_DISCO:
                            atual->tempo_io_restante = TEMPO_IO_DISCO;
                            enfileirar(&io_disco, atual);
                            printf("Processo %d foi para I/O de Disco (tempo em io: %d) - interrompeu quantum\n", atual->pid, TEMPO_IO_DISCO);
                            break;
                        case IO_FITA:
                            atual->tempo_io_restante = TEMPO_IO_FITA;
                            enfileirar(&io_fita, atual);
                            printf("Processo %d foi para I/O de Fita (tempo em io: %d) - interrompeu quantum\n", atual->pid, TEMPO_IO_FITA);
                            break;
                        case IO_IMPRESSORA:
                            atual->tempo_io_restante = TEMPO_IO_IMPRESSORA;
                            enfileirar(&io_impressora, atual);
                            printf("Processo %d foi para I/O de Impressora (tempo em io: %d) - interrompeu quantum\n", atual->pid, TEMPO_IO_IMPRESSORA);
                            break;
                    }
                } else {
                    // Se não fez I/O nem foi finalizado, e movido para baixa prioridade e tem o status atualizado
                    atual->prioridade = 1; 
                    atual->status = 0;
                    enfileirar(&baixa, atual);

                    //Processo tem 20% de chance de gerar um processo filho
                    int gera_filho = (rand() % 10);  
                    if(contar_processos_ativos(&alta, &baixa, &io_disco, &io_fita, &io_impressora) < MAX_PROCESSOS && gera_filho < 2){ 
                        Processo* p_filho = criar_novo_processo(atual->pid);
                        enfileirar(&alta, p_filho);
                        printf("\n*** NOVO PROCESSO CRIADO ***\n");
                        if (p_filho->tipo_io == 0) {
                            printf("Processo %d criado: tempo=%d, pai=%d, SEM I/O\n", 
                            p_filho->pid, p_filho->tempo_restante, p_filho->ppid);
                        } else {
                            printf("Processo %d criado: tempo=%d, pai=%d, I/O=%s no instante %d\n", 
                            p_filho->pid, p_filho->tempo_restante, p_filho->ppid,
                            (p_filho->tipo_io == IO_DISCO) ? "Disco" : (p_filho->tipo_io == IO_FITA) ? "Fita" : "Impressora",
                            p_filho->instante_io);
                        }
                        printf("***************************\n");
                    }

                    printf("Processo %d sofreu preempcao e foi para fila baixa.\n", atual->pid);
                }
            }
        } else {
            //Informa que a CPU está ociosa caso não haja processos em alta ou baixa, mas ainda haja processo nas filas de I/O
            printf("CPU ociosa.\n");
        }

        
        ciclo++;

        imprimir_filas(&alta, &baixa, &io_disco, &io_fita, &io_impressora);
        

    }

    printf("\nTodos os processos foram finalizados.\n");
}


int main() {
    srand(time(NULL));
    int opcao = 0;
    printf("===== Simulador de Escalonador Round Robin =====\n");

    // Interação com usuário para escolher o modo de execução
    while(opcao != 1 && opcao != 2){
        printf("Selecione o modo de execucao: \n");
        printf("1. Execucao passo a passo (pressione tecla para continuar)\n");
        printf("2. Execucao direta (sem pausas)\n\n");
        printf("Opcao: ");
        
        if(scanf("%d", &opcao) != 1){
            while(getchar() != '\n');
            opcao = 0;
        }

        if(opcao != 1 && opcao != 2){
           limpa_tela();
            printf("Opcao invalida! Escolha 1 ou 2.\n");
        }
    }

    // Limpa o buffer após o scanf
    while(getchar() != '\n');
    
    limpa_tela();
    
    // Chama a função simular com a opção do usuário
    if(opcao == 1){
        printf("Modo passo a passo ativado!\n");
        printf("Pressione ENTER para avancar cada ciclo.\n\n");
        simular(1);
    } else {
        printf("Modo execucao direta ativado!\n\n");
        simular(0);
    }

    return 0;
}