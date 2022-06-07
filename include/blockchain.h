#pragma once
#include "../include/openssl/crypto.h"
#include "../include/openssl/sha.h"
#include "../include/mtwister/mtwister.h"
// Semente para o algoritmo mtwister
#define SEED 1234567
#define MAX_TRANSACOES 61
#define MAX_ENDERECOS 255
#define MAX_BITCOINS_TRANSACAO 50

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

unsigned int getRandInt(MTRand *r, unsigned int min, unsigned int max);

BlocoNaoMinerado *alocaBlocoNaoMinerado();

void initBloco(BlocoNaoMinerado *bNM, BlocoMinerado *bAnt, MTRand *r);

BlocoMinerado *minerarBloco(BlocoNaoMinerado *bNM);

BlocoMinerado *buscaBloco(BlocoMinerado *bM, unsigned int chave);

// unsigned char *buscaHash(NoLista *raiz, unsigned int chave);

unsigned int getBitcoins(unsigned int end, int *vEnderecos);

int getMaxBitcoins(int *vEnderecos);

void listarEnderecos(int *vEnderecos);
