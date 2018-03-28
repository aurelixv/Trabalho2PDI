#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pdi.h"

#define IMAGEM      "Exemplos/b01 - Original.bmp"
#define INGENUO     "ingenuo.bmp"
#define SEPARAVEL   "separavel.bmp"
#define INTEGRAL    "integral.bmp"
#define JARGURA     11
#define JALTURA     15

void ingenuo(Imagem *in, Imagem *out, int a, int l);
void separavel(Imagem *in, Imagem *out, int a, int l);
void integral(Imagem *in, Imagem *out, int a, int l);
void fazBorda(Imagem *in, int a, int l);

int main() {
    
    Imagem *imagem, *borrada;
    
    //Abre a imagem a ser borrada
    printf("A carregar a imagem [ %s ]... ", IMAGEM);
    imagem = abreImagem(IMAGEM, 3);
    if(!imagem) {
        printf("\n\x1b[31mERRO:\x1b[0m A imagem não pode ser aberta.\n");
        exit(1);
    }
    printf("\t\t[\x1b[32m OK \x1b[0m]\n");
    
    //Aloca memória para a imagem de saída
    printf("Criando imagem auxiliar... ");
    borrada = criaImagem(imagem->largura, imagem->altura, imagem->n_canais);
    if(!borrada) {
        printf("\n\x1b[31mERRO:\x1b[0m A imagem auxiliar não pode ser criada.\n");
        exit(1);
    }
    copiaConteudo(imagem, borrada);
    printf("\t\t\t\t\t[\x1b[32m OK \x1b[0m]\n");
    
    //Funções que borram a imagem
    //TODO: Algoritmo ingênuo
    ingenuo(imagem, borrada, JALTURA, JARGURA);

    printf("Salvando imagem borrada com o nome [ %s ]... ", INGENUO);
    salvaImagem(borrada, INGENUO);
    printf("\t\t[\x1b[32m OK \x1b[0m]\n");
    
    //Reescreve imagem de saída para o próximo algoritmo
    copiaConteudo(imagem, borrada);
    
    //TODO: Filtro separável
    separavel(imagem, borrada, JALTURA, JARGURA);

    //Faz a borda
    fazBorda(borrada, JALTURA, JARGURA);
    printf("Salvando imagem borrada com o nome [ %s ]... ", SEPARAVEL);
    salvaImagem(borrada, SEPARAVEL);
    printf("\t[\x1b[32m OK \x1b[0m]\n");
    
    //Reescreve imagem de saída para o próximo algoritmo
    copiaConteudo(imagem, borrada);
    
    //TODO: Algoritmo com imagens integrais
    integral(imagem, borrada, JALTURA, JARGURA);
    
    printf("Salvando imagem borrada com o nome [ %s ]... ", INTEGRAL);
    salvaImagem(borrada, INTEGRAL);
    printf("\t\t[\x1b[32m OK \x1b[0m]\n");
    
    //Libera a memória alocada antes de terminar
    printf("Liberando memória...\n");
    destroiImagem(imagem);
    destroiImagem(borrada);    
    
    printf("Encerrando...\n");
    
    return 0;
}

void fazBorda(Imagem *in, int a, int l) {

    int x, y, canal;

    for(canal = 0; canal < in->n_canais; canal += 1) {
        for(y = 0; y < in->altura; y += 1) {
            for(x = 0; x < in->largura; x += 1) {
                if((y < a/2) || (x < l/2) || (x > in->largura - l/2 - 1) || (y > in->altura - a/2 - 1))
                    in->dados[canal][y][x] = 0.0f;
            }
        }
    }
}

void ingenuo(Imagem *in, Imagem *out, int a, int l) {
    
    printf("Iniciando algoritmo ingenuo... ");
    int y, x, j, i, canal;
    int bordery = a/2;
    int borderx = l/2;
    int area = a * l;
    float soma = 0.0f;
    
    for(canal = 0; canal < in->n_canais; canal += 1) {
        for(y = bordery; y < in->altura - bordery; y += 1) {
            for(x = borderx; x < in->largura - borderx; x += 1) {
                for(j = y - bordery; j <= y + bordery; j += 1) {
                    for(i = x - borderx; i <= x + borderx; i += 1) {
                        soma += in->dados[canal][j][i];
                    }
                }
                out->dados[canal][y][x] = soma/area;
                soma = 0.0f;
            }
        }
    }
    
    printf("\t\t\t\t\t[\x1b[32m OK \x1b[0m]\n");
}

void separavel(Imagem *in, Imagem *out, int a, int l) {

    printf("Iniciando filro separável... ");
    int y, x, i, canal;
    int bordery = a/2;
    int borderx = l/2;
    float soma = 0.0f;
    Imagem *buffer = criaImagem(in->largura, in->altura, in->n_canais);
    
    /** Filtro horizontal **/
    for(canal = 0; canal < in->n_canais; canal++) {
        for(y = 0; y < in->altura; y++) {
            for(x = borderx; x < in->largura - borderx; x++) {
                for(i = x - borderx; i <= x + borderx; i++) {
                    soma += in->dados[canal][y][i];
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
                    soma += buffer->dados[canal][i][x];
                }
                out->dados[canal][y][x] = soma/a;
                soma = 0.0f;
            }
        }
    }
    
    destroiImagem(buffer);
    printf("\t\t\t\t\t[\x1b[32m OK \x1b[0m]\n");
}

void integral(Imagem *in, Imagem *out, int a, int l) {
    
    printf("Iniciando filtro integral... ");
    int y, x, canal;
    int bordery = a/2;
    int borderx = l/2;
    int area = a * l;
    Imagem *buffer = criaImagem(in->largura, in->altura, in->n_canais);
    
    for(canal = 0; canal < in->n_canais; canal += 1) {
        
        for(y = 0; y < in->altura; y += 1) {            
            buffer->dados[canal][y][0] = in->dados[canal][y][0];
            for(x = 1; x < in->largura; x += 1) {
                buffer->dados[canal][y][x] = in->dados[canal][y][x] + buffer->dados[canal][y][x - 1];
            }
        }
        
        for(y = 1; y < in->altura; y += 1) {
            for(x = 0; x < in->largura; x += 1) {
                buffer->dados[canal][y][x] = buffer->dados[canal][y][x] + buffer->dados[canal][y - 1][x];
            }
        }
    }

    //Borrar a imagem
    for(canal = 0; canal < in->n_canais; canal += 1) {
        //Primeiro elemento
        out->dados[canal][bordery][borderx] = buffer->dados[canal][a - 1][l - 1] / area;

        //Faz toda a primeira linha, fora o primeiro pixel
        for(x = borderx + 1; x < in->largura - borderx; x += 1) {
            out->dados[canal][bordery][x] = (buffer->dados[canal][a - 1][x + borderx] 
                                - buffer->dados[canal][a - 1][x - borderx - 1]) / area;        
        }

        //Faz toda a primeira coluna, fora o primeiro pixel
        for(y = bordery + 1; y < in->altura - bordery; y += 1) {
            out->dados[canal][y][borderx] = (buffer->dados[canal][y + bordery][l - 1] 
                                - buffer->dados[canal][y - bordery - 1][l - 1]) / area;
        }

        //Faz o resto da imagem
        for(y = bordery + 1; y < in->altura - bordery; y += 1) {
            for(x = borderx + 1; x < in->largura - borderx; x += 1) {
                out->dados[canal][y][x] = (buffer->dados[canal][y + bordery][x + borderx]
                                + buffer->dados[canal][y - bordery - 1][x - borderx - 1]
                                - buffer->dados[canal][y + bordery][x - borderx - 1]
                                - buffer->dados[canal][y - bordery - 1][x + borderx]) / area;
            }
        }
    }

    destroiImagem(buffer);
    printf("\t\t\t\t\t[\x1b[32m OK \x1b[0m]\n");
}






