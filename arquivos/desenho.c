/*============================================================================*/
/* FUNÇÕES PARA DESENHAR COISAS                                               */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Funções para desenhar elementos em imagens. */
/*============================================================================*/

#include "desenho.h"

#include <stdio.h>

/*============================================================================*/
/** Desenha uma linha reta entre dois pontos.
 *
 * Parâmetros: Coordenada p1: um ponto.
 *             Coordenada p2: outro ponto.
 *             Cor cor: cor a se usar.
 *             Imagem* img: desenha aqui.
 *
 * Valor de retorno: nenhum. */

void desenhaLinha (Coordenada p1, Coordenada p2, Cor cor, Imagem* img)
{
    int inicio, fim, x, y, canal;

    if (p1.x == p2.x) // Linha vertical.
    {
        inicio = MAX (0, MIN (p1.y, p2.y));
        fim = MIN (img->altura-1, MAX (p1.y, p2.y));

        for (y = inicio; y <= fim; y++)
            for (canal = 0; canal < img->n_canais; canal++)
                img->dados [canal][y][p1.x] = cor.canais [canal];
    }
    else if (p1.y == p2.y) // Linha horizontal.
    {
        inicio = MAX (0, MIN (p1.x, p2.x));
        fim = MIN (img->largura-1, MAX (p1.x, p2.x));

        for (x = inicio; x <= fim; x++)
            for (canal = 0; canal < img->n_canais; canal++)
                img->dados [canal][p1.y][x] = cor.canais [canal];
    }
    else
        printf ("TODO: desenhaLinha: implementar para linhas inclinadas!\n");
}

/*----------------------------------------------------------------------------*/
/** Desenha um retângulo.
 *
 * Parâmetros: Retangulo r: coordenadas do retângulo a desenhar.
 *             Cor cor: cor a se usar.
 *             Imagem* img: desenha aqui.
 *
 * Valor de retorno: nenhum. */

 void desenhaRetangulo (Retangulo r, Cor cor, Imagem* img)
 {
    // Esquerda.
    if (r.e >= 0 && r.e < img->largura)
        desenhaLinha (criaCoordenada (r.e, r.c), criaCoordenada (r.e, r.b), cor, img);

    // Direita.
    if (r.d >= 0 && r.d < img->largura)
        desenhaLinha (criaCoordenada (r.d, r.c), criaCoordenada (r.d, r.b), cor, img);

    // Cima.
    if (r.c >= 0 && r.c < img->altura)
        desenhaLinha (criaCoordenada (r.e, r.c), criaCoordenada (r.d, r.c), cor, img);

    // Baixo.
    if (r.b >= 0 && r.b < img->altura)
        desenhaLinha (criaCoordenada (r.e, r.b), criaCoordenada (r.d, r.b), cor, img);
 }

/*============================================================================*/
