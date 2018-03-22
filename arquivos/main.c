#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

#define IMAGEM "Exemplos/a01 - Original.bmp"
#define BORRADA "ingenuo.bmp"
#define JALTURA 13
#define JARGURA 3

void ingenuo(Imagem *in, Imagem *out, int a, int l);

int main() {
    
    Imagem *imagem;
    
    //Abre a imagem a ser borrada
    printf("A carregar a imagem [ %s ]... ", IMAGEM);
    imagem = abreImagem(IMAGEM, 3);
    if(!imagem) {
        printf("\nERRO: A imagem não pode ser aberta.\n");
        exit(1);
    }
    printf("Sucesso!\n");
    
    printf("Criando imagem auxiliar... ");
    Imagem *borrada = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);
    if(!borrada) {
        printf("\nERRO: A imagem auxiliar não pode ser criada.\n");
        exit(1);
    }
    printf("Sucesso!\n");
    
    //Funções que borram a imagem
    //TODO: Algoritmo ingênuo
    ingenuo(imagem, borrada, JALTURA, JARGURA);
    
    //TODO: Filtro separável
    
    //TODO: Algoritmo com imagens integrais
    
    printf("Salvando imagem borrada com o nome [ %s ]... ", BORRADA);
    salvaImagem(borrada, "ingenuo.bmp");
    printf("Sucesso!\n");
    
    //Libera a memória alocada antes de terminar
    printf("Liberando memória...\n");
    destroiImagem(imagem);
    
    printf("Encerrando...\n");
    
    return 0;
}

void ingenuo(Imagem *in, Imagem *out, int a, int l) {
    
    printf("Iniciando algoritmo ingenuo... ");
    int y, x, j, i, canal;
    int bordery = a/2;
    int borderx = l/2;
    float soma = 0.0f;
    
    for(canal = 0; canal < in->n_canais; canal += 1) {
        for(y = bordery; y < in->altura - bordery; y += 1) {
            for(x = borderx; x < in->largura - borderx; x += 1) {
                for(j = y - bordery; j <= y + bordery; j += 1) {
                    for(i = x - borderx; i <= x + borderx; i += 1) {
                        soma += in->dados[canal][j][i];
                    }
                }
                out->dados[canal][y][x] = soma/(a*l);
                soma = 0.0f;
            }
        }
    }
    
    printf("Sucesso!\n");
}
