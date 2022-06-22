#pragma once
#include "../include/openssl/crypto.h"
#include "../include/openssl/sha.h"
#include "../include/mtwister/mtwister.h"
// Semente para o algoritmo mtwister
#define SEED 1234567
#define MAX_TRANSACOES 61
#define MAX_ENDERECOS 256
#define MAX_BITCOINS_TRANSACAO 50

#define CAMINHO_TRANSACAO "./carteiras.bin"

short int dificuldade = 4;
// Bloco que ainda não foi minerado
struct BlocoNaoMinerado
{
  unsigned int numero;                              // 4
  unsigned int nonce;                               // 4
  unsigned char data[184];                          // nao alterar. Deve ser inicializado com zeros.
  unsigned char hashAnterior[SHA256_DIGEST_LENGTH]; // 32
};
typedef struct BlocoNaoMinerado BlocoNaoMinerado;

// Bloco que já foi minerado
struct BlocoMinerado
{
  BlocoNaoMinerado bloco;
  unsigned char hash[SHA256_DIGEST_LENGTH]; // 32 bytes
};
typedef struct BlocoMinerado BlocoMinerado;

struct arvore
{
  unsigned int btc;
  int endereco;
  struct arvore *esq, *dir;
};
typedef struct arvore arvore;

struct Metadados
{
  int quantidade;
  int tamanho_bloco;
  unsigned int indice_mtrand;
  int qtd_estouros;
};
typedef struct Metadados Metadados;

void ordenaBitcoins(
    unsigned int *Carteira,
    unsigned int *cartAux,
    unsigned int *endereco);

void quicksort(
    unsigned int *carteira,
    unsigned int *endereco,
    int esquerda,
    int direita);

void inicializaEnderecos(
    unsigned int *Carteira,
    MTRand *r,
    Metadados *cabecalho,
    unsigned int *endereco);

void arquivaBitcoins(unsigned int *Carteira);

void criarHash(BlocoNaoMinerado *bNM, unsigned char *hash);

void printBloco(BlocoNaoMinerado *bNM);

// void addBlocoMinerado(BlocoMinerado *bM, NoLista **raiz);
void inicializarMetadados(
    const char *caminho,
    Metadados *cabecalho,
    MTRand *rand);

void arquivaBlocosMinerados(
    int n,
    BlocoMinerado *bM,
    const char *caminho);

void attMetadados(
    const char *caminho,
    int n,
    Metadados *cabecalho,
    MTRand *rand);

BlocoMinerado *procuraUltimoBloco(const char *caminho, int n);

void preencheCarteiras(MTRand *r, unsigned int *Carteira, Metadados *cabecalho);

void transacoes(
    BlocoNaoMinerado *bNM,
    MTRand *r,
    unsigned int *Carteira,
    int nTrans,
    Metadados *cabecalho);

unsigned int getRandInt(
    MTRand *r,
    unsigned int min,
    unsigned int max,
    Metadados *cabecalho);

BlocoNaoMinerado *alocaBlocoNaoMinerado();

void initBloco(
    BlocoNaoMinerado *bNM,
    BlocoMinerado *bAnt,
    MTRand *r,
    unsigned int *Carteira,
    Metadados *cabecalho);

BlocoMinerado *minerarBloco(BlocoNaoMinerado *bNM);

BlocoMinerado *buscaBloco(BlocoMinerado *bM, unsigned int chave);

// unsigned char *buscaHash(NoLista *raiz, unsigned int chave);
