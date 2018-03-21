/*============================================================================*/
/* SEGMENTAÇÃO                                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Tipos e funções para segmentação. */
/*============================================================================*/

#include "base.h"
#include "segmenta.h"
#include <stdio.h>
#include <stdlib.h>

/*============================================================================*/
/* CLASSIFICAÇÃO DE PIXELS                                                    */
/*============================================================================*/
/** Binarização simples por limiarização.
 *
 * Parâmetros: Imagem* in: imagem de entrada. Se tiver mais que 1 canal,
 *               binariza cada canal independentemente.
 *             Imagem* out: imagem de saída. Deve ter o mesmo tamanho da
 *               imagem de entrada.
 *             float threshold: limiar.
 *
 * Valor de retorno: nenhum (usa a imagem de saída). */

void binariza (Imagem* in, Imagem* out, float threshold)
{
    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: binariza: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    int channel, row, col;
    for (channel = 0; channel < in->n_canais; channel++)
        for (row = 0; row < in->altura; row++)
            for (col = 0; col < in->largura; col++)
                out->dados [channel][row][col] = (in->dados [channel][row][col] > threshold)? 1 : 0;
}

/*============================================================================*/
/* ROTULAGEM                                                                  */
/*============================================================================*/
/** Rotulagem usando flood fill. Marca os objetos da imagem com os valores
 * [0.1,0.2,etc].
 *
 * Parâmetros: Imagem* img: imagem de entrada E saída.
 *             ComponenteConexo** componentes: um ponteiro para um vetor de
 *               saída. Supomos que o ponteiro inicialmente é inválido. Ele irá
 *               apontar para um vetor que será alocado dentro desta função.
 *               Lembre-se de desalocar o vetor criado!
 *             int largura_min: descarta componentes com largura menor que esta.
 *             int altura_min: descarta componentes com altura menor que esta.
 *             int n_pixels_min: descarta componentes com menos pixels que isso.
 *
 * Valor de retorno: o número de componentes conexos encontrados. */

int rotulaFloodFill (Imagem* img, ComponenteConexo** componentes, int largura_min, int altura_min, int n_pixels_min)
{
    int row, col, n;

    // Marca todos os objetos com valores negativos.
    n = 0;
    for (row = 0; row < img->altura; row++)
        for (col = 0; col < img->largura; col++)
            if (img->dados [0][row][col] > 0)
            {
                img->dados [0][row][col] = -1;
                n++;
            }

    // Aloca o vetor de saída. Inicialmente, vamos reservar espaço como se cada pixel fosse um componente.
    *componentes = malloc (sizeof (ComponenteConexo) * n);

    // Aloca a pilha (para flood fill com pilha).
    Coordenada* pilha = malloc (sizeof (Coordenada) * n);

    // Rotula.
    n = 0;
    float label = 0.1f;
    for (row = 0; row < img->altura; row++)
    {
        for (col = 0; col < img->largura; col++)
        {
            // Achou um componente não rotulado.
            if (img->dados [0][row][col] < 0)
            {
                ComponenteConexo* c = &((*componentes) [n]);
                c->label = label;
                c->roi = criaRetangulo (row, row, col, col);
                c->n_pixels = 0;

				pilha [0] = criaCoordenada (col,row);
				floodFill (img, pilha, c);

                // Verifica se este componente não ficou pequeno demais.
                if (c->n_pixels >= n_pixels_min &&
                    c->roi.d - c->roi.e + 1 >= largura_min &&
                    c->roi.b - c->roi.c + 1 >= altura_min)
                    n++;

                label += 0.1f;
            }
        }
    }

    // Descarta a pilha.
	free (pilha);

    // Reduz o número de componentes ao necessário.
    *componentes = realloc (*componentes, sizeof (ComponenteConexo) * n);
    return (n);
}

/*----------------------------------------------------------------------------*/
/** Flood fill com pilha.
 *
 * Parâmetros: Imagem* img: imagem a se inundar.
 *             Coordenada* pilha: buffer de memória a se usar. Consideramos que
 *               a primeira posição contém o ponto inicial da inundação.
 *             int valor: valor usado na inundação.
 *             ComponenteConexo* componente: dados sobre o blob inundado.
 *
 * Valor de retorno: nenhum. */

void floodFill (Imagem* img, Coordenada* pilha, ComponenteConexo* componente)
{
    int n_pilha = 1;

	// Rotula a semente.
    img->dados [0][pilha [0].y][pilha [0].x] = componente->label;

    // Enquanto a pilha não esvaziar...
    while (n_pilha)
    {
        // Remove o topo da pilha.
        Coordenada c = pilha [--n_pilha];
		componente->n_pixels++;

        // Atualiza a região de interesse.
        if (c.y < componente->roi.c)
            componente->roi.c = c.y;
        if (c.y > componente->roi.b)
            componente->roi.b = c.y;
        if (c.x < componente->roi.e)
            componente->roi.e = c.x;
        if (c.x > componente->roi.d)
            componente->roi.d = c.x;

		// Coloca os vizinhos não marcados do pixel na pilha.
		if (c.x > 0 && img->dados [0][c.y][c.x-1] < 0)
        {
            img->dados [0][c.y][c.x-1] = componente->label;
            pilha [n_pilha++] = criaCoordenada (c.x-1, c.y);
        }
        if (c.x < img->largura-1 && img->dados [0][c.y][c.x+1] < 0)
        {
            img->dados [0][c.y][c.x+1] = componente->label;
            pilha [n_pilha++] = criaCoordenada (c.x+1, c.y);
        }
        if (c.y > 0 && img->dados [0][c.y-1][c.x] < 0)
        {
            img->dados [0][c.y-1][c.x] = componente->label;
            pilha [n_pilha++] = criaCoordenada (c.x, c.y-1);
        }
        if (c.y < img->altura-1 && img->dados [0][c.y+1][c.x] < 0)
        {
            img->dados [0][c.y+1][c.x] = componente->label;
            pilha [n_pilha++] = criaCoordenada (c.x, c.y+1);
        }
	}
}

/*----------------------------------------------------------------------------*/
/** Rotulagem em 2 passadas usando uma union find que representa uma lista de
 * equivalências. Marca os objetos da imagem com os valores [1,2,etc].
 *
 * Parâmetros: Imagem* img: imagem de entrada E saída.
 *             ComponenteConexo** componentes: um ponteiro para um vetor de
 *               saída. Supomos que o ponteiro inicialmente é inválido. Ele irá
 *               apontar para um vetor que será alocado dentro desta função.
 *               Lembre-se de desalocar o vetor criado!
 *             int largura_min: descarta componentes com largura menor que esta.
 *             int altura_min: descarta componentes com altura menor que esta.
 *             int n_pixels_min: descarta componentes com menos pixels que isso.
 *
 * Valor de retorno: o número de componentes conexos encontrados. */

// Funções auxiliares para implementar o union-find.
int rotulaFind (int* equivalencias, int classe)
{
    while (equivalencias [classe] != 0)
        classe = equivalencias [classe];

    return (classe);
}

void rotulaUnion (int* equivalencias, int classe1, int classe2)
{
    int raiz1 = rotulaFind (equivalencias, classe1);
    int raiz2 = rotulaFind (equivalencias, classe2);

    if (raiz1 == raiz2)
        return; // As duas classes já são equivalentes.

    if (raiz1 < raiz2)
        equivalencias [raiz2] = raiz1;
    else
        equivalencias [raiz1] = raiz2;
}

int rotulaUnionFind (Imagem* img, ComponenteConexo** componentes, int largura_min, int altura_min, int n_pixels_min)
{
    int i, j, n;

    // Marca os objetos com valores negativos.
    n = 0;
    for (i = 0; i < img->altura; i++)
        for (j = 0; j < img->largura; j++)
            if (img->dados [0][i][j] > 0)
            {
                img->dados [0][i][j] = -1;
                n++;
            }

    // Aloca a lista de equivalências.
    int* equivalencias = malloc (sizeof (int) * n);
    int n_classes = 1;
    equivalencias [0] = 0;

    // Rotula.
    // Primeira passada: vai marcando e criando a lista de equivalências.
    for (i = 0; i < img->altura; i++)
    {
        for (j = 0; j < img->largura; j++)
        {
            if (img->dados [0][i][j] < 0) // Pixel não marcado.
            {
                // Tem um vizinho já marcado à esquerda ou acima?
                float label_cima = (i > 0)? img->dados [0][i-1][j] : 0;
                float label_esquerda = (j > 0)? img->dados [0][i][j-1]: 0;

                if (label_cima <= 0 && label_esquerda <= 0)  // Marca o pixel com uma nova label.
                {
                    equivalencias [n_classes] = 0;
                    img->dados [0][i][j] = (float) n_classes;
                    n_classes++;
                }
                else if (label_cima == label_esquerda)
                    img->dados [0][i][j] = label_cima;
                else if (label_cima > 0 && label_esquerda > 0)
                {
                    // Achamos um conflito! Seleciona a menor label e indica uma equivalencia.
                    img->dados [0][i][j] = MIN (label_esquerda, label_cima);
                    rotulaUnion (equivalencias, (int) label_esquerda, (int) label_cima);
                }
                else if (label_cima > 0) // Marca o pixel com a label do pixel acima.
                    img->dados [0][i][j] = label_cima;
                else if (label_esquerda > 0) // Marca o pixel com a label do pixel à esquerda.
                    img->dados [0][i][j] = label_esquerda;
            }
        }
    }

    // Segunda passada: limpa a imagem.
    for (i = 0; i < img->altura; i++)
        for (j = 0; j < img->largura; j++)
            if (img->dados [0][i][j] > 0)
                img->dados [0][i][j] = rotulaFind (equivalencias, (int) img->dados [0][i][j]);

    // Descobrimos o número de classes únicas. Associamos cada classe a uma
    // posição na lista de componentes. Reutilizamos a lista de equivalências.
    n = 0;
    for (i = 1; i < n_classes; i++)
        if (equivalencias [i] == 0)
        {
            equivalencias [i] = n;
            n++;
        }

    // Aloca o vetor de saída.
    *componentes = malloc (sizeof (ComponenteConexo) * n);

    // Inicializa os componentes.
    for (i = 0; i < n; i++)
    {
        (*componentes) [i].n_pixels = 0;
        (*componentes) [i].roi.c = img->altura;
        (*componentes) [i].roi.b = -1;
        (*componentes) [i].roi.e = img->largura;
        (*componentes) [i].roi.d = -1;
    }

    // Agora, percorremos a imagem, procurando dados sobre cada classe.
    for (i = 0; i < img->altura; i++)
        for (j = 0; j < img->largura; j++)
            if (img->dados [0][i][j] > 0)
            {
                ComponenteConexo* c = &((*componentes) [equivalencias [(int) img->dados [0][i][j]]]);
                c->label = img->dados [0][i][j];
                c->n_pixels++;
                if (i < c->roi.c)
                    c->roi.c = i;
                if (i > c->roi.b)
                    c->roi.b = i;
                if (j < c->roi.e)
                    c->roi.e = j;
                if (j > c->roi.d)
                    c->roi.d = j;
            }

    // Elimina componentes pequenos demais.
    int n_mantidos = 0;

    for (i = 0; i < n; i++)
    {
        ComponenteConexo* c = &((*componentes) [i]);
        if (c->n_pixels >= n_pixels_min &&
            c->roi.d - c->roi.e + 1 >= largura_min &&
            c->roi.b - c->roi.c + 1 >= altura_min)
        {
            // Move o componente para a sua posição final.
            if (i != n_mantidos)
                (*componentes) [n_mantidos] = *c;

            n_mantidos++;
        }
    }

    // Reduz o número de componentes ao necessário.
    if (n != n_mantidos)
        *componentes = realloc (*componentes, sizeof (ComponenteConexo) * n_mantidos);

	free (equivalencias);
    return (n_mantidos);
}


/*============================================================================*/
