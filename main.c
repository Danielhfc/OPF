#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Este será a quantidade de valores que a estrutura irá aceitar.
#define VALORES_MAXIMOS 2

typedef struct {
  double class_label;               // Classe pode ser -1 ou 1
  double features[VALORES_MAXIMOS]; // Valor existente, no banana datase há 2
                                    // valores
} Amostra;

// Calcula a distância euclidiana entre dois vetores de características
double distancia(double *valores1, double *valores2, int quantidadeFeatures) {
  double soma = 0.0;
  for (int i = 0; i < quantidadeFeatures; i++) {
    soma += pow(valores1[i] - valores2[i], 2);
    // Calcula a diferença entre os elementos correspondentes
    // dos dois vetores, eleva ao quadrado e adiciona à soma
  }
  return sqrt(soma); // Retorna a raiz quadrada da soma, que representa a
                     // distância euclidiana entre os vetores de características
}

// Treina o algoritmo Optimum Path Forest (OPF)
void opf_train(Amostra *samples, int quantidadeAmostras,
               int quantidadeFeatures) {
  for (int i = 0; i < quantidadeAmostras; i++) {
    double min_dist = INFINITY; // Variável para armazenar a menor distância
                                // entre a amostra atual e outras amostras
    int min_class = -1;         // Variável para armazenar o rótulo de classe
                                // correspondente à menor distância

    for (int j = 0; j < quantidadeAmostras; j++) {
      if (i != j) {
        // Evita calcular a distância entre a amostra atual e ela mesma
        double dist = distancia(samples[i].features, samples[j].features,
                                quantidadeFeatures);

        if (dist < min_dist) {
          min_dist = dist; // Atualiza a menor distância se a distância
                           // calculada for menor
          min_class = samples[j].class_label;
          // Atualiza o rótulo de classe
          // correspondente à menor distância
        }
      }
    }

    samples[i].class_label = min_class;
    // Atribui o rótulo de classe correspondente à menor
    // distância à amostra atual
  }
}

// Classifica uma amostra utilizando o algoritmo OPF
int opf_classify(Amostra *samples, int quantidadeAmostras,
                 int quantidadeFeatures, double *features) {

  int min_index = -1;         // Índice da amostra mais próxima
  double min_dist = INFINITY; // Distância mínima encontrada até o momento

  for (int i = 0; i < quantidadeAmostras; i++) {
    double dist = distancia(samples[i].features, features, quantidadeFeatures);
    // Calcula a distância entre a amostra
    // atual e a amostra de teste
    if (dist < min_dist) {
      min_dist = dist; // Atualiza a menor distância encontrada até o momento
      min_index = i;   // Atualiza o índice da amostra mais próxima
    }
  }

  return samples[min_index].class_label;
  // Retorna o rótulo de classe correspondente à amostra mais
  // próxima
}

int main() {
  srand(time(0)); // Inicializar a semente do gerador de números aleatórios
  int quantidadeAmostras, quantidadeFeatures, classes;
  FILE *arquivo = fopen("banana.txt", "r");
  // Abre o arquivo que sera utilizado para o treinamento
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo.\n");
    return 1; // Retorna 1 em caso de falha na abertura do arquivo
  }
  fscanf(arquivo, "%d %d %d", &quantidadeAmostras, &quantidadeFeatures,
         &classes);
  // A primeira linha contem 3 valores com a quantidade de
  // amostras, quantidade de features e quantidade de classes

  // Inicializar vetor de treinamento
  Amostra training_samples[quantidadeAmostras];

  printf("Vetor de treinamento:\n");
  for (int i = 0; i < quantidadeAmostras; i++) {
    // Serão lidas i amostras, sendo esse o valor obtido na leitura do
    // arquivo
    printf("Amostra %d:\n", i + 1);
    for (int j = 0; j < quantidadeFeatures; j++) {
      // Serão lidas j valores para cada amostra
      fscanf(arquivo, "%lf", &training_samples[i].features[j]);
      printf("Característica %d: %lf\n", j + 1,
             training_samples[i].features[j]);

      // Imprime o valor contido no dataset
    }
    fscanf(arquivo, "%lf", &training_samples[i].class_label);
    // É feita a leitura da classe
    printf("Rótulo de classe: %.2lf\n\n", training_samples[i].class_label);
    // Imprime-se a classe
  }

  fclose(arquivo);
  // Treinamento do OPF
  opf_train(training_samples, quantidadeAmostras, quantidadeFeatures);

  // Vetor de teste
  double test_features[quantidadeFeatures];
  int quantidadeAmostrasTeste, predicted_class;

  // Variáveis utilizadas na interface
  int escolha, opcaoValida = 0, sair = 1;

  while (!opcaoValida) {
    printf("Digite a opção para realizar o teste: \n");
    printf("1 - Testar com N amostras aleatórias \n");
    printf("2 - Testar com valores colocados manualmente\n");
    scanf("%d", &escolha);

    switch (escolha) {
    case 1:
      printf("Digite a quantidade de amostras que irá gerar.\n");
      scanf("%d", &quantidadeAmostrasTeste);
      // O usuário pode escolher a quantidade
      // de amostras para testar o algoritmo

      // Serão criados os vetores de teste com valores aleátorios
      for (int i = 0; i < quantidadeAmostrasTeste; i++) {
        printf("Amostra de teste : %d\n", i + 1);
        for (int j = 0; j < quantidadeFeatures; j++) {
          test_features[j] = ((double)rand() / RAND_MAX) * 4 - 2;
          // Valor aleatório entre -2 e 2
          printf("Característica %d: %lf\n", j + 1, test_features[j]);
        }
        // Classificação do OPF
        predicted_class = opf_classify(training_samples, quantidadeAmostras,
                                       quantidadeFeatures, test_features);

        printf("Classe prevista: %d\n\n", predicted_class);
      }
      opcaoValida = 1;
      break;
    case 2:

      printf("Para sair do programa, digite qualquer valor fora do intervalo "
             "de -2 e 2\n");

      while (sair != -1) {
        for (int j = 0; j < quantidadeFeatures; j++) {
          printf("\nDigite o %d valor da amostra de teste: ", j + 1);
          scanf("%lf", &test_features[j]);
          if (test_features[j] < -2 || test_features[j] > 2) {
            sair = -1;
            break;
          }
        }
        if (sair != -1) {
          // Classificação do OPF
          predicted_class = opf_classify(training_samples, quantidadeAmostras,
                                         quantidadeFeatures, test_features);

          printf("Classe prevista: %d\n\n", predicted_class);
        }
      }

      opcaoValida = 1;
      break;
    default:
      printf("Opção inválida. Tente novamente.\n");
      break;
    }
  }

  return 0;
}
