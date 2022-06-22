#include <stdio.h>
#include "../include/blockchain.h"
#include "../include/mtwister/mtwister.h"

int main(int argc, char *argv[])
{

  MTRand rand = seedRand(SEED);

  unsigned int Carteira[MAX_ENDERECOS];

  unsigned int carteiraOrdenada[MAX_ENDERECOS];
  unsigned int endereco[MAX_ENDERECOS];
  int a = -1;

  const char *arquivo = "./blockchain.bin";
  Metadados *cabecalho = (Metadados *)malloc(sizeof(Metadados));
  inicializarMetadados(arquivo, cabecalho, &rand);
  inicializaEnderecos(Carteira, &rand, cabecalho, endereco);

  BlocoMinerado blocos[2];
  BlocoNaoMinerado *bNM = NULL;
  BlocoMinerado *bMAtual = NULL;
  BlocoMinerado *bMAnterior = NULL;

  int n = 0;

  // NoLista *raiz = NULL;
  // addBlocoMinerado(genMinerado, &raiz);

  ordenaBitcoins(Carteira, carteiraOrdenada, endereco);
  int minerar = 1;
  while (a != 0)
  {
    printf("\nDigite o numero referente a operação que deseja realizar\n");
    printf("1-Pesquisar hash de um bloco\n");
    printf("2-Pesquisar Carteira\n");
    printf("3-Buscar carteira com mais BitCoins\n");
    printf("4-Listar Carteiras e suas quantias de BitCoin(s)\n");
    printf("5-Minerar Bloco\n");
    printf("0-Sair\n");
    scanf("%d", &a);

    switch (a)
    {
    case 1:
      // Fazer a busca do hash de um bloco
      break;
    case 2:
      printf("Digite o endereco da Carteira que deseja pesquisar\n");
      int end;
      scanf("%d", &end);
      printf("A Carteira %d tem %u BitCoin(s)\n", end, Carteira[end]);
      break;
    case 3:
      printf("%3d - %3u\n", endereco[255], carteiraOrdenada[255]);
      break;
    case 4:
      printf("Endereco   -   Bitcoin(s)\n");
      for (int k = 0; k < 256; k++)
        printf("%3d - %3u\n", endereco[k], carteiraOrdenada[k]);
      break;
    case 5:
      if (cabecalho->quantidade == 0)
      {
        // Bloco Genesis
        bNM = alocaBlocoNaoMinerado();
        initBloco(bNM, NULL, &rand, Carteira, cabecalho);

        bMAtual = minerarBloco(bNM);
        bMAnterior = bMAtual;
        n += 1;
        blocos[n - 1] = *bMAtual;
      }
      else if (cabecalho->quantidade > 0)
      {
        bMAnterior = procuraUltimoBloco(arquivo, cabecalho->quantidade);
      }
      while (minerar)
      {
        // Aloca um novo
        bNM = alocaBlocoNaoMinerado();
        // Inicializa
        initBloco(bNM, bMAnterior, &rand, Carteira, cabecalho);
        // Minera
        bMAtual = minerarBloco(bNM);
        n += 1;
        blocos[n - 1] = *bMAtual;
        if (n == 2)
        {
          arquivaBlocosMinerados(n, blocos, arquivo);
          attMetadados(arquivo, n, cabecalho, &rand);
          arquivaBitcoins(Carteira);
          n = 0;
          minerar = 0;
        }
        // Adiciona o bloco minerado na blockchain
        bMAnterior = bMAtual;
      }
      ordenaBitcoins(Carteira, carteiraOrdenada, endereco);
      break;
    default:
      printf("Digite uma opcao valida!\n");
    }
  }

  return 0;
}

void inicializarMetadados(const char *caminho, Metadados *cabecalho, MTRand *rand)
{
  FILE *arq = fopen(caminho, "rb");
  if (arq == NULL)
  {
    arq = fopen(caminho, "wb");
    cabecalho->quantidade = 0;
    cabecalho->tamanho_bloco = (int)sizeof(BlocoMinerado);
    cabecalho->indice_mtrand = 0;
    cabecalho->qtd_estouros = 0;
    if (fwrite(cabecalho, sizeof(Metadados), 1, arq) != 1)
    {
      fclose(arq);
      printf("Erro ao escrever no arquivo\n");
      exit(1);
    }
    printf("Ok!\n");
    fclose(arq);
    return;
  }
  // Verifica se existem metadados
  rewind(arq);
  if (fread(cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro na leitura do arquivo\n");
    exit(1);
  }
  // Se existem o que está escrito neles?
  for (int k = 0; k < cabecalho->qtd_estouros; k++)
  {
    rand->index = 624;
    genRandLong(rand);
  }
  rand->index = cabecalho->indice_mtrand;
  if (cabecalho->quantidade >= 0 && cabecalho->tamanho_bloco == sizeof(BlocoMinerado))
  {
    fclose(arq);
    printf("%d, %d, %d\n", cabecalho->quantidade, cabecalho->tamanho_bloco, cabecalho->indice_mtrand);
    return;
  }
  fclose(arq);
}

BlocoMinerado *procuraUltimoBloco(const char *caminho, int n)
{
  // abre o arquivo para leitura
  FILE *arq = fopen(caminho, "rb");
  if (arq == NULL)
  {
    printf("Falhaa ao abrir o arquivo");
    exit(1);
  }

  long int offset = 0;
  // calcula a posição do ponteiro leitor
  if (n == 0)
  {
    offset = sizeof(Metadados);
  }
  else
  {
    offset = (sizeof(BlocoMinerado) * (n - 1)) + sizeof(Metadados);
  }
  // posiciona o ponteiro
  rewind(arq);
  if (fseek(arq, offset, SEEK_SET) != 0)
  {
    fclose(arq);
    exit(1);
  }
  BlocoMinerado *aux = (BlocoMinerado *)malloc(sizeof(BlocoMinerado));
  if (fread(aux, sizeof(BlocoMinerado), 1, arq) != 1)
  {
    printf("Falha na leitura do arquivo");
    fclose(arq);
    exit(1);
  }
  fclose(arq);

  return aux;
}

void arquivaBlocosMinerados(int n, BlocoMinerado *bM, const char *caminho)
{

  FILE *arq = fopen(caminho, "rb");
  if (arq == NULL)
  {
    printf("Erro ao abrir o arquivo");
    exit(1);
  }
  rewind(arq);
  Metadados cabecalho;

  if (fread(&cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro na leitura do arquivo");
    exit(1);
  }
  fclose(arq);
  arq = fopen(caminho, "ab");
  if (fwrite(bM, sizeof(BlocoMinerado), n, arq) != n)
  {
    fclose(arq);
    printf("Erro na gravação no arquivo\n");
    exit(1);
  }
  fclose(arq);
}

void attMetadados(const char *caminho, int n, Metadados *cabecalho, MTRand *rand)
{
  FILE *arq = fopen(caminho, "r+b");
  if (arq == NULL)
  {
    printf("Erro na abertura do arquivo\n");
    exit(1);
  }
  rewind(arq);
  if (fread(cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro na leitrua do arquivo\n");
    exit(1);
  }
  rewind(arq);
  cabecalho->quantidade += n;
  cabecalho->indice_mtrand = rand->index;
  if (fwrite(cabecalho, sizeof(Metadados), 1, arq) != 1)
  {
    fclose(arq);
    printf("Erro ao escrever no arquivo\n");
    exit(1);
  }
  fclose(arq);
}

void criarHash(BlocoNaoMinerado *bNM, unsigned char *hash)
{
  SHA256((unsigned char *)bNM, sizeof(BlocoNaoMinerado), hash);
}

void printBloco(BlocoNaoMinerado *bNM)
{
  printf("Numero: %u\n", bNM->numero);
  printf("Nonce: %u\n", bNM->nonce);
  for (int i = 0; i < 183; i += 3)
  {
    printf("Transacao: Origem: %d, Destino: %d, BitCoins: %d\n", bNM->data[i], bNM->data[i + 1], bNM->data[i + 2]);
  }
}

BlocoMinerado *minerarBloco(BlocoNaoMinerado *bNM)
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  // Para não demorar muito na mineração
  // 0 para nao minerado e 1 para minerado
  char minerado = 0;
  do
  {
    criarHash(bNM, hash);
    int *chave = (int *)hash;
    if (*chave == 0)
      minerado = 1;
    bNM->nonce += 1;
  } while (minerado == 0);
  bNM->nonce -= 1;
  printBloco(bNM);
  BlocoMinerado *aux = (BlocoMinerado *)malloc(sizeof(BlocoMinerado));
  aux->bloco = *bNM;
  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++)
  {
    aux->hash[k] = hash[k];
    printf("%x", hash[k]);
  }
  printf("\n");
  return aux;
}

unsigned int getRandInt(MTRand *r, unsigned int min, unsigned int max, Metadados *cabecalho)
{
  if (r->index >= 624)
    cabecalho->qtd_estouros += 1;
  cabecalho->indice_mtrand = r->index;
  return (unsigned int)(genRandLong(r) % (max - min + 1)) + min;
}

BlocoNaoMinerado *alocaBlocoNaoMinerado()
{
  BlocoNaoMinerado *aux = (BlocoNaoMinerado *)malloc(sizeof(BlocoNaoMinerado));
  return aux;
}

void initBloco(
    BlocoNaoMinerado *bNM,
    BlocoMinerado *bAnt,
    MTRand *r,
    unsigned int *Carteira,
    Metadados *cabecalho)
{

  int i = 0;
  if (bAnt == NULL)
  {
    bNM->numero = 1;
    bNM->nonce = 170598660;
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
  unsigned int nTrans = getRandInt(r, 0, MAX_TRANSACOES, cabecalho);

  transacoes(bNM, r, Carteira, nTrans, cabecalho);
}

void inicializaEnderecos(
    unsigned int *Carteira,
    MTRand *r,
    Metadados *cabecalho,
    unsigned int *endereco)
{

  for (int k = 0; k < MAX_ENDERECOS; k++)
    endereco[k] = k;

  FILE *arq = fopen(CAMINHO_TRANSACAO, "r+b");
  if (arq == NULL)
  {
    arq = fopen(CAMINHO_TRANSACAO, "wb");
    preencheCarteiras(r, Carteira, cabecalho);

    if (fwrite(Carteira, sizeof(unsigned int), MAX_ENDERECOS, arq) != MAX_ENDERECOS)
    {
      fclose(arq);
      printf("Erro ao escrever no arquivo\n");
      exit(1);
    }
    printf("Ok!\n");
    fclose(arq);
    return;
  }

  rewind(arq);
  if (fread(Carteira, sizeof(unsigned int), MAX_ENDERECOS, arq) != MAX_ENDERECOS)
  {
    fclose(arq);
    printf("Erro na leitura do arquivo\n");
    exit(1);
  }
  fclose(arq);
}

void arquivaBitcoins(unsigned int *Carteira)
{
  FILE *arq = fopen(CAMINHO_TRANSACAO, "r+b");
  if (arq == NULL)
  {
    printf("Nao foi possivel abrir o arquivo!\n");
    exit(1);
  }
  rewind(arq);
  if (fwrite(Carteira, sizeof(unsigned int), MAX_ENDERECOS, arq) != MAX_ENDERECOS)
  {
    fclose(arq);
    printf("Erro ao escrever no arquivo\n");
    exit(1);
  }
  fclose(arq);
  arq = fopen(CAMINHO_TRANSACAO, "rb");
  rewind(arq);
  if (fread(Carteira, sizeof(unsigned int), MAX_ENDERECOS, arq) != MAX_ENDERECOS)
  {
    fclose(arq);
    printf("Erro na leitura do arquivo\n");
    exit(1);
  }
  printf("Ok!\n");
  fclose(arq);
  return;
}

void preencheCarteiras(MTRand *r, unsigned int *Carteira, Metadados *cabecalho)
{
  int i = 0;
  while (i < MAX_ENDERECOS)
  {
    Carteira[i] = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO, cabecalho);
    i++;
  }
}

void transacoes(
    BlocoNaoMinerado *bNM,
    MTRand *r,
    unsigned int *Carteira,
    int nTrans,
    Metadados *cabecalho)
{
  for (int i = 0; i < nTrans; i += 3)
  {
    short int origem = getRandInt(r, 0, 255, cabecalho);
    short int destino = getRandInt(r, 0, 255, cabecalho);

    if (origem == destino)
    {
      nTrans++;
      continue;
    }

    short int btc = getRandInt(r, 0, MAX_BITCOINS_TRANSACAO, cabecalho);

    if (btc > Carteira[origem])
    {
      Carteira[origem] = 0;
    }
    else
      Carteira[origem] -= btc;

    Carteira[destino] += btc;

    bNM->data[i] = origem;
    bNM->data[i + 1] = destino;
    bNM->data[i + 2] = btc;
  }
}

void ordenaBitcoins(
    unsigned int *Carteira,
    unsigned int *cartAux,
    unsigned int *endereco)
{

  for (int j = 0; j < MAX_ENDERECOS; j++)
    cartAux[j] = Carteira[j];

  quicksort(cartAux, endereco, 0, MAX_ENDERECOS - 1);
}

void quicksort(
    unsigned int *carteira,
    unsigned int *endereco,
    int esquerda,
    int direita)
{
  int i, j, aux, y;

  i = esquerda;
  j = direita;
  aux = carteira[(esquerda + direita) / 2];

  while (i <= j)
  {
    while (carteira[i] < aux && i < direita)
    {
      i++;
    }
    while (carteira[j] > aux && j > esquerda)
    {
      j--;
    }
    if (i <= j)
    {
      y = carteira[i];
      carteira[i] = carteira[j];
      carteira[j] = y;

      y = endereco[i];
      endereco[i] = endereco[j];
      endereco[j] = y;
      i++;
      j--;
    }
  }

  if (j > esquerda)
  {
    quicksort(carteira, endereco, esquerda, j);
  }
  if (i < direita)
  {
    quicksort(carteira, endereco, i, direita);
  }
}