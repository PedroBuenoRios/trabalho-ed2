#include <stdio.h>
#include "../include/blockchain.h"
#include "../include/mtwister/mtwister.h"

void criarHash(BlocoNaoMinerado *bNM, unsigned char *hash);
void printBloco(BlocoNaoMinerado *bNM);

int main(int argc, char *argv[])
{

  MTRand r = seedRand(SEED);
  BlocoNaoMinerado *gNaoMinerado;
  // Aloca um novo
  gNaoMinerado = alocaBlocoNaoMinerado();
  // Inicializa
  initBloco(gNaoMinerado, NULL, &r);

  BlocoMinerado *gMinerado = minerarBloco(gNaoMinerado);
  //  Minera
  //  BlocoMinerado *bM = minerarBloco(genesis);
  //  Adiciona o bloco minerado na blockchain
  //  addBlockChain(bM);

  int n = 1;
  BlocoNaoMinerado *bNMAtual;
  BlocoMinerado *bMAtual;
  BlocoMinerado *bMAnterior;
  bMAnterior = gMinerado;
  do
  {
    // Aloca um novo
    bNMAtual = alocaBlocoNaoMinerado();
    // Inicializa
    initBloco(bNMAtual, bMAnterior, &r);
    // Minera
    bMAtual = minerarBloco(bNMAtual);
    // Adiciona o bloco minerado na blockchain
    // addBlockChain();
    bMAnterior = bMAtual;
    n++;
  } while (n < 5);

  return 0;
}

/*
 * A função criarHash recebe 1 parâmetro como entrada:
 * O bloco que ainda não foi minerado -> bNM (bloco Não Minerado)
 */
void criarHash(BlocoNaoMinerado *bNM, unsigned char *hash)
{
  SHA256((unsigned char *)bNM, sizeof(BlocoNaoMinerado), hash);
}

void printBloco(BlocoNaoMinerado *bNM)
{
  printf("Numero: %d\n", bNM->numero);
  printf("Nonce: %d\n", bNM->nonce);
  for (int i = 0; i < 183; i += 3)
  {
    printf("Transacao: Origem: %x, Destino: %x, BitCoins: %x\n", bNM->data[i], bNM->data[i + 1], bNM->data[i + 2]);
  }
}

BlocoMinerado *minerarBloco(BlocoNaoMinerado *bNM)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  // Para não demorar muito na mineração
  unsigned int maxnonce = 500000;
  do
  {
    criarHash(bNM, hash);
    if (hash[0] == 0 && hash[1] == 0)
      break;
    bNM->nonce += 1;
  } while (bNM->nonce <= maxnonce);
  printBloco(bNM);

  BlocoMinerado *aux = (BlocoMinerado *)malloc(sizeof(BlocoMinerado));
  aux->bloco = *bNM;
  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++)
    aux->hash[k] = hash[k];
  return aux;
}

unsigned int getRandInt(MTRand *r, unsigned int min, unsigned int max)
{
  return (unsigned int)(genRandLong(r) % (max - min + 1)) + min;
}

BlocoNaoMinerado *alocaBlocoNaoMinerado()
{
  BlocoNaoMinerado *aux = (BlocoNaoMinerado *)malloc(sizeof(BlocoNaoMinerado));
  return aux;
}

void initBloco(BlocoNaoMinerado *bNM, BlocoMinerado *bAnt, MTRand *r)
{

  int i = 0;
  if (bAnt == NULL)
  {
    bNM->numero = 1;
    bNM->nonce = 99231;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
      bNM->hashAnterior[i] = 0;
  }
  else
  {
    // Definindo o número do bloco
    bNM->numero = bAnt->bloco.numero + 1;

    // Definindo nonce um número aleatório entre [0, 10000]
    bNM->nonce = 0;

    // Definindo a assinatura do bloco anterior
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
      bNM->hashAnterior[i] = bAnt->hash[i];
  }

  // Inicializando o campo data com 0
  i = 0;
  for (i = 0; i < 184; i++)
    bNM->data[i] = 0;

  // Gerando o número de transações
  // unsigned int nTrans = getRandInt(r, 0, MAX_TRANSACOES);
  i = 0;
  // Gerando os valores das transações
  // !! Criar uma função para realizar as transações
  // !! Otimizar o algoritmo para as transações

  // do
  // {
  //   // Endereço de origem
  //   unsigned int origem = getRandInt(r, 0, MAX_ENDERECOS);
  //   // Endereço de destino
  //   unsigned int destino = getRandInt(r, 0, MAX_ENDERECOS);
  //   // Número de bitcoins na transação
  //   unsigned int nBitcoins = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO);

  //   bNM->data[i] = (unsigned char)origem;
  //   bNM->data[i + 1] = (unsigned char)destino;
  //   bNM->data[i + 2] = (unsigned char)nBitcoins;
  //   i += 3;
  // } while (i <= nTrans);
}

/*void transacoes(MTRand *r, EnderecoAVL **end, BitCoinAVL **btc)
{

  // for (int i = getRandInt(r, 0, 61); i > 0; i--)
  // {
  //   unsigned int origem, destino;

  //   origem = getRandInt(r, 0, MAX_ENDERECOS);
  //   destino = getRandInt(r, 0, MAX_ENDERECOS);

  //   if (origem != destino)
  //   { // Verificação para evitar que seja realizada uma transação com origem e destino iguais

  //     // Endereço de origem
  //     unsigned int origemCarteira = origem;
  //     // Endereço de destino
  //     unsigned int destinoCarteira = destino;
  //     // Número de bitcoins na transação
  //     unsigned int nBitcoins = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO);
  //   }
  //   else // caso sejam iguais o i é incrementado para que uma nova transacao valida ocorra
  //     i++;
  // }
}*/
