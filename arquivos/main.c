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

    //Faz a borda
    fazBorda(borrada, JALTURA, JARGURA);

    //Grava o resultado
    printf("Salvando imagem borrada com o nome [ %s ]... ", INGENUO);
    salvaImagem(borrada, INGENUO);
    printf("\t\t[\x1b[32m OK \x1b[0m]\n");

    //Reescreve imagem de saída para o próximo algoritmo
    copiaConteudo(imagem, borrada);

    //TODO: Filtro separável
    separavel(imagem, borrada, JALTURA, JARGURA);

    //Faz a borda
    fazBorda(borrada, JALTURA, JARGURA);

    //Grava o resultado
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

/** Torna as bordas pretas **/
void fazBorda(Imagem *in, int a, int l) {

    int x, y, canal;

    // Preenche todas as bordas com o valor 0.0f
    for(canal = 0; canal < in->n_canais; canal += 1) {
        for(y = 0; y < in->altura; y += 1) {
            for(x = 0; x < in->largura; x += 1) {
                if((y < a/2) || (x < l/2) || (x > in->largura - l/2 - 1) || (y > in->altura - a/2 - 1))
                    in->dados[canal][y][x] = 0.0f;
            }
        }
    }
}

/** Filtra a imagem deixando bordas **/
void ingenuo(Imagem *in, Imagem *out, int a, int l) {

    printf("Iniciando algoritmo ingenuo... ");
    int y, x, j, i, canal;
    int bordery = a/2;
    int borderx = l/2;
    int area = a * l;
    float soma = 0.0f;

    // Percorre todos os pixels da janela por toda a imagem e atribui a imagem de saida
    // o valor da média entre os pixels da janela
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

/** Filtra a imagem deixando bordas **/
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

/** Filtra toda a imagem, inclusive as bordas **/
void integral(Imagem *in, Imagem *out, int a, int l) {

    printf("Iniciando filtro integral... ");
    int y, x, canal;
    int bordery = a/2;
    int borderx = l/2;
    Imagem *buffer = criaImagem(in->largura, in->altura, in->n_canais);

    // Preenche o buffer com a soma de todos os pixels a cima e a esquerda
    for(canal = 0; canal < in->n_canais; canal += 1) {
        // Soma todos os pixels da esquerda da imagem original
        for(y = 0; y < in->altura; y += 1) {
            buffer->dados[canal][y][0] = in->dados[canal][y][0];
            for(x = 1; x < in->largura; x += 1) {
                buffer->dados[canal][y][x] = in->dados[canal][y][x] + buffer->dados[canal][y][x - 1];
            }
        }
        // Soma todos os pixels de cima da imagem original
        for(y = 1; y < in->altura; y += 1) {
            for(x = 0; x < in->largura; x += 1) {
                buffer->dados[canal][y][x] = buffer->dados[canal][y][x] + buffer->dados[canal][y - 1][x];
            }
        }
    }

    int j, i, area, aux;
    float soma;

    // Borra a imagem de saida
    for(canal = 0; canal < in->n_canais; canal += 1) {
        for(y = 0; y < in->altura; y += 1) {
            for(x = 0; x < in->largura; x += 1) {
                // Reinicializa as variaveis para as proximas iterações
                area = 0;
                soma = 0.0f;
                // Percorre a janela e atribui a soma o maior valor (soma) do buffer
                for(j = y - bordery; j <= y + bordery; j += 1) {
                    if(j > -1 && j < in->altura) {
                        for(i = x - borderx; i <= x + borderx; i += 1) {
                            if(i > -1 && i < in->largura) {
                                area += 1;
                                if(buffer->dados[canal][j][i] > soma)
                                    soma = buffer->dados[canal][j][i];
                            }
                        }
                    }
                }

                /** Depois de obter a soma total da janela, é necessário subtrair
                os valores de cima, da esquerda e somar com o valor da diagonal
                esquerda superior, para obter o valor "borrado" do pixel:
                        + x x -
                        x x x x
                        x x x x
                        - x x +
                **/

                // Subtrai os valores de cima
                if(y > bordery) {
                    aux = x + borderx;
                    while(aux >= in->largura)
                        aux -= 1;

                    soma -= buffer->dados[canal][y - bordery - 1][aux];
                }

                // Subtrai os valores da esquerda
                if(x > borderx) {
                    aux = y + bordery;
                    while(aux >= in->altura)
                        aux -= 1;

                    soma -= buffer->dados[canal][aux][x - borderx - 1];
                }

                // Soma os valores da diagonal esquerda superior
                if(y > bordery && x > borderx)
                    soma += buffer->dados[canal][y - bordery - 1][x - borderx - 1];

                // Divide a soma pela área da janela e atribui o valor para a imagem de saida
                out->dados[canal][y][x] = soma / area;
            }
        }
    }

    destroiImagem(buffer);
    printf("\t\t\t\t\t[\x1b[32m OK \x1b[0m]\n");
}
