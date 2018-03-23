#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

#define IMAGEM "Exemplos/a01 - Original.bmp"
#define BORRADA "ingenuo.bmp"
#define SEPARAVEL "separavel.bmp"
#define JALTURA 13
#define JARGURA 3

void ingenuo(Imagem *in, Imagem *out, int a, int l);
void separavel(Imagem *in, Imagem *out, int a, int l);

int main() {
    
    Imagem *imagem;
    
    //Abre a imagem a ser borrada
    printf("A carregar a imagem [ %s ]... ", IMAGEM);
    imagem = abreImagem(IMAGEM, 3);
    if(!imagem) {
        printf("\n\x1b[31mERRO:\x1b[0m A imagem não pode ser aberta.\n");
        exit(1);
    }
    printf("[\x1b[32m OK \x1b[0m]\n");
    
    printf("Criando imagem auxiliar... ");
    Imagem *borrada = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);
    if(!borrada) {
        printf("\n\x1b[31mERRO:\x1b[0m A imagem auxiliar não pode ser criada.\n");
        exit(1);
    }
    borrada = imagem;
    printf("[\x1b[32m OK \x1b[0m]\n");
    
    //Funções que borram a imagem
    //TODO: Algoritmo ingênuo
    ingenuo(imagem, borrada, JALTURA, JARGURA);

    printf("Salvando imagem borrada com o nome [ %s ]... ", BORRADA);
    salvaImagem(borrada, "ingenuo.bmp");
    printf("[\x1b[32m OK \x1b[0m]\n");

    printf("Criando imagem auxiliar... ");
    Imagem *borradaS = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);
    if(!borradaS) {
        printf("\n\x1b[31mERRO:\x1b[0m A imagem auxiliar não pode ser criada.\n");
        exit(1);
    }
    borradaS = imagem;
    
    //TODO: Filtro separável
    separavel(imagem, borradaS, JALTURA, JARGURA);

    printf("Salvando imagem borrada com o nome [ %s ]... ", SEPARAVEL);
    salvaImagem(borradaS, "separavel.bmp");
    printf("[\x1b[32m OK \x1b[0m]\n");

    //TODO: Algoritmo com imagens integrais
    
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
    
    printf("[\x1b[32m OK \x1b[0m]\n");
}

void separavel(Imagem *in, Imagem *out, int a, int l) {

    printf("Iniciando filro separável... ");
    int y, x, i, canal;
    int bordery = a/2;
    int borderx = l/2;
    float soma = 0.0f;
    //TODO: ALOCAR NOVA IMAGEM ANTES DA IGUALDADE
    Imagem *buffer = in;
    
    /** Filtro horizontal **/
    for(canal = 0; canal < in->n_canais; canal++) {
        for(y = 0; y < in->altura; y++) {
            for(x = borderx; x < in->largura - borderx; x++) {
                for(i = x - borderx; i <= x + borderx; i++) {
                    //TODO: MUDAR OPERACAO
                    soma += buffer->dados[canal][y][i];
                }
                buffer->dados[canal][y][x] = soma/l;
                soma = 0.0f;
            }
        }
    }

    /** Filtro vertical **/
    for(canal = 0; canal < in->n_canais; canal++) {
        for(x = 0; x < in->largura; x++) {
            for(y = bordery; y < in->altura - bordery; y++) {
                for(i = y - bordery; i <= y + bordery; i++) {
                    //TODO: MUDAR OPERACAO
                    soma += buffer->dados[canal][i][x];
                }
                buffer->dados[canal][y][x] = soma/a;
                soma = 0.0f;
            }
        }
    }
    
    *out = *buffer;

    printf("[\x1b[32m OK \x1b[0m]\n");
}
