# Simulador de Escalonador Round Robin

Projeto desenvolvido para a disciplina de Arquitetura de Computadores e Sistemas Operacionais do curso de Ciência da Computação - UFRJ.

Este simulador implementa um escalonador baseado no algoritmo **Round Robin**, incluindo conceitos importantes de sistemas operacionais como preempção, filas de prioridade, operações de entrada e saída (I/O) e criação dinâmica de processos.

---

## Conceitos Implementados

### Escalonamento Round Robin

Os processos recebem uma fatia de tempo (*quantum*) para execução. Ao término do quantum, caso ainda não tenham finalizado, sofrem preempção e retornam para a fila de prontos.

### Filas de Prioridade

O simulador utiliza duas filas de prontos:

* Alta prioridade
* Baixa prioridade

Novos processos entram inicialmente na fila de alta prioridade.

### Operações de I/O

Os processos podem solicitar operações de:

* Disco
* Fita
* Impressora

Durante uma operação de I/O o processo é bloqueado e transferido para a fila correspondente.

Após a conclusão da operação:

* Processos que utilizaram Disco retornam para a fila de baixa prioridade.
* Processos que utilizaram Fita ou Impressora retornam para a fila de alta prioridade.

### Criação Dinâmica de Processos

Durante a execução, processos possuem chance de criar processos filhos, simulando o comportamento de sistemas multitarefa reais.

### CPU Ociosa

O simulador identifica situações em que não existem processos prontos para execução, mas ainda existem processos aguardando operações de I/O.

---

## Configurações da Simulação

| Parâmetro                  | Valor      |
| -------------------------- | ---------- |
| Número máximo de processos | 8          |
| Quantum                    | 5 unidades |
| Chance de I/O              | 50%        |
| Tempo I/O Disco            | 4 ciclos   |
| Tempo I/O Fita             | 2 ciclos   |
| Tempo I/O Impressora       | 3 ciclos   |

---

## Estruturas Utilizadas

O projeto foi desenvolvido em C utilizando:

* Structs
* Filas encadeadas
* Alocação dinâmica de memória
* Ponteiros
* Simulação baseada em ciclos

Cada processo possui informações como:

* PID
* PPID
* Prioridade
* Tempo restante
* Tempo executado
* Status
* Operação de I/O associada

---

## Modos de Execução

### Execução Passo a Passo

Permite acompanhar cada ciclo da simulação pressionando ENTER.

Ideal para fins didáticos e análise do comportamento do escalonador.

### Execução Direta

Executa toda a simulação sem interrupções.

---

## Exemplo de Saída

```text
[Ciclo 5]

Executando processo 3 (prioridade alta) por 2 unidades

Processo 3 foi para I/O de Disco

Alta prioridade: P4(10) P5(7)
Baixa prioridade: P1(4) P2(3)
I/O Disco: P3(4)
```

---

## Como Compilar

### Linux / macOS

```bash
gcc round_robin.c -o round_robin
./round_robin
```

### Windows (MinGW)

```bash
gcc round_robin.c -o round_robin.exe
round_robin.exe
```

---

## Objetivos de Aprendizado

Durante o desenvolvimento deste projeto foram praticados:

* Escalonamento de processos
* Algoritmos de Sistemas Operacionais
* Estruturas de dados em C
* Filas encadeadas
* Gerenciamento de memória
* Simulação de concorrência
* Modelagem de estados de processos

---

## Autor

Vitor Nunes

- GitHub: github.com/Vitornunees38

