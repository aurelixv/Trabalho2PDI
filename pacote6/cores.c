/*============================================================================*/
/* MANIPULAÇÃO DE CORES                                                       */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e funções para manipulação de cores. */
/*============================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include "cores.h"

/*============================================================================*/
/* TIPO Cor                                                                   */
/*============================================================================*/
/** Apenas uma função útil para "criar" uma instância do tipo Cor.
 *
 * Parâmetros: float r: valor do canal R.
 *             float g: valor do canal G.
 *             float b: valor do canal B.
 *
 * Valor de retorno: a Cor criada. */

Cor criaCor (float r, float g, float b)
{
    Cor c;
    c.canais [0] = r;
    c.canais [1] = g;
    c.canais [2] = b;
    return (c);
}

/*============================================================================*/
/* CONVERSÕES DE CORES                                                        */
/*============================================================================*/
/** Converte uma imagem de RGB para escala de cinza.
 *
 * Parâmetros: Imagem* in: imagem de entrada de 3 canais.
 *             Imagem* out: imagem de saída de 1 canal, com o mesmo tamanho.
 *
 * Valor de retorno: nenhum. */

void RGBParaCinza (Imagem* in, Imagem* out)
{
    if (in->n_canais != 3)
    {
        printf ("ERRO: RGBParaCinza: a imagem de origem precisa ter 3 canais.\n");
        exit (1);
    }

    if (out->n_canais != 1)
    {
        printf ("ERRO: RGBParaCinza: a imagem de destino precisa ter 1 canal.\n");
        exit (1);
    }

    if (in->largura != out->largura || in->altura != out->altura)
    {
        printf ("ERRO: RGBParaCinza: as imagens precisam ter o mesmo tamanho.\n");
        exit (1);
    }

    /* Percorre a imagem e converte. Usamos aqui os mesmos fatores de conversão
       do OpenCV, o que mantém certas propriedades de percepção (i.e. não são
	   parâmetros "mágicos"). */
    int i, j;
	for (i = 0; i < in->altura; i++)
        for (j = 0; j < in->largura; j++)
            out->dados [0][i][j] = in->dados [0][i][j] * 0.299f + in->dados [1][i][j] * 0.587f + in->dados [2][i][j] * 0.114f;
}

/*----------------------------------------------------------------------------*/
/** Converte uma imagem de escala de cinza para RGB.
 *
 * Parâmetros: Imagem* in: imagem de entrada de 1 canal.
 *             Imagem* out: imagem de saída de 3 canais, com o mesmo tamanho.
 *
 * Valor de retorno: nenhum. */

void cinzaParaRGB (Imagem* in, Imagem* out)
{
    if (in->n_canais != 1)
    {
        printf ("ERRO: cinzaParaRGB: a imagem de origem precisa ter 1 canal.\n");
        exit (1);
    }

    if (out->n_canais != 3)
    {
        printf ("ERRO: cinzaParaRGB: a imagem de destino precisa ter 3 canais.\n");
        exit (1);
    }

    if (in->largura != out->largura || in->altura != out->altura)
    {
        printf ("ERRO: cinzaParaRGB: as imagens precisam ter o mesmo tamanho.\n");
        exit (1);
    }

    /* Percorre a imagem e converte. Simplesmente copia 3 vezes cada valor. */
	int i, j, k;
    for (i = 0; i < 3; i++)
        for (j = 0; j < in->altura; j++)
            for (k = 0; k < in->largura; k++)
                out->dados [i][j][k] = in->dados [0][j][k];
}

/*============================================================================*/
/* TRANSFORMAÇÕES DE CORES                                                    */
/*============================================================================*/
/** Inverte as cores de uma imagem, usando o complemento. Supomos pixels com
 * valores no intervalo [0,1].
 *
 * Parâmetros: Imagem* in: imagem de entrada.
 *             Imagem* out: imagem de saída, com o mesmo tamanho e número de
 *               canais.
 *
 * Valor de retorno: nenhum. */

void inverte (Imagem* in, Imagem* out)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: inverte: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

	int i, j, k;
    for (i = 0; i < in->n_canais; i++)
        for (j = 0; j < in->altura; j++)
            for (k = 0; k < in->largura; k++)
                out->dados [i][j][k] = 1.0f - in->dados [i][j][k];
}

/*============================================================================*/
