/*============================================================================*/
/* MANIPULAÇÃO DE ARQUIVOS BMP                                                */
/*----------------------------------------------------------------------------*/
/* Autor: Bogdan T. Nassu - nassu@dainf.ct.utfpr.edu.br                       */
/*============================================================================*/
/** Este arquivo traz declarações de um tipo e rotinas para manipulação de
 * arquivos bmp. Como temos um propósito puramente didático, apenas um sub-
 * conjunto mínimo do formato foi implementado. Usamos aqui matrizes alocadas
 * dinamicamente e números de ponto flutuante. Esta não é a forma mais eficiente
 * de se trabalhar com imagens, mas procuramos priorizar a clareza e a
 * facilidade de uso. */
/*============================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "imagem.h"

/*============================================================================*/

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

unsigned long getLittleEndianULong (unsigned char* buffer);
int leHeaderBitmap (FILE* stream, unsigned long* offset);
int leHeaderDIB (FILE* stream, unsigned long* largura, unsigned long* altura);
int leDados (FILE* stream, Imagem* img);

void putLittleEndianULong (unsigned long val, unsigned char* buffer);
void putLittleEndianUShort (unsigned short val, unsigned char* buffer);
unsigned char float2uchar (float x);
int salvaHeaderBitmap (FILE* stream, Imagem* img);
int salvaHeaderDIB (FILE* stream, Imagem* img);
int salvaDados (FILE* stream, Imagem* img);

/*============================================================================*/
/* FUNÇÕES DO MÓDULO                                                          */
/*============================================================================*/
/** Cria uma imagem vazia.
 *
 * Parâmetros: int largura: largura da imagem.
 *             int altura: altura da imagem.
 *             int n_canais: número de canais.
 *
 * Valor de retorno: a imagem alocada. A responsabilidade por desalocá-la é do
 *                   chamador. */

Imagem* criaImagem (int largura, int altura, int n_canais)
{
	int i, j;
	Imagem* img;

	if (largura <= 0 || altura <= 0 || n_canais <= 0)
    {
        printf ("criaImagem: imagens devem ter altura, largura e n_canais maiores que 0.\n");
        return (NULL);
    }

	img = (Imagem*) malloc (sizeof (Imagem));

	img->largura = largura;
	img->altura = altura;
	img->n_canais = n_canais;

    img->dados = (float***) malloc (sizeof (float**) * n_canais); /* Uma matriz por canal. */
	for (i = 0; i < n_canais; i++)
	{
		img->dados [i] = (float**) malloc (sizeof (float*) * altura);
		for (j = 0; j < altura; j++)
			img->dados [i][j] = (float*) malloc (sizeof (float) * largura);
	}

	return (img);
}

/*----------------------------------------------------------------------------*/
/** Destroi uma imagem dada.
 *
 * Parâmetros: Imagem* img: a imagem a destruir.
 *
 * Valor de retorno: nenhum. */

void destroiImagem (Imagem* img)
{
	int i, j;

	for (i = 0; i < img->n_canais; i++)
	{
		for (j = 0; j < img->altura; j++)
			free (img->dados [i][j]);
		free (img->dados [i]);
	}
	free (img->dados);
	free (img);
}

/*----------------------------------------------------------------------------*/
/** Abre um arquivo de imagem dado.
 *
 * Parâmetros: char* arquivo: caminho do arquivo a abrir.
 *             int n_canais: número de canais. Deve ser 1 ou 3. Se for 1,
 *               converte a imagem para escala de cinza.
 *
 * Valor de retorno: uma imagem alocada contendo os dados do arquivo, ou NULL
 *                   se não for possível abrir a imagem. */

Imagem* abreImagem (char* arquivo, int n_canais)
{
	FILE* stream;
	unsigned long data_offset = 0, largura = 0, altura = 0;
	Imagem* img;

    if (n_canais != 1 && n_canais != 3)
	{
        printf ("abreImagem: so pode abrir imagens com 1 ou 3 canais.\n");
        return (NULL);
	}

	/* Abre o arquivo. */
	stream = fopen (arquivo, "rb");
	if (!stream)
		return (NULL);

	if (!leHeaderBitmap (stream, &data_offset))
	{
		fclose (stream);
		return (NULL);
	}

	if (!leHeaderDIB (stream, &largura, &altura))
	{
		fclose (stream);
		return (NULL);
	}

	/* Pronto, cabeçalhos lidos! Vamos agora colocar o fluxo nos dados... */
	if (fseek (stream, data_offset, SEEK_SET) != 0)
	{
		printf ("abreImagem: erro lendo dados do arquivo.\n");
		fclose (stream);
		return (NULL);
	}

	/* ... e tudo pronto para criar nossa imagem! */
	img = criaImagem (largura, altura, 3);

	/* Lê os dados. */
	if (!leDados (stream, img))
	{
		printf ("abreImagem: erro lendo dados do arquivo.\n");
		fclose (stream);
		free (img);
		return (NULL);
	}

	fclose (stream);

    /* Se o chamador espera uma imagem de 1 canal, converte para escala de cinza. */
    if (n_canais == 1)
    {
        int i, j;
        float** ptr_canal0;

        /* Para não ficar tão ineficiente, reaproveita a memória da imagem RGB original. */
        for (i = 0; i < img->altura; i++)
            for (j = 0; j < img->largura; j++)
                img->dados [0][i][j] = img->dados [0][i][j] * 0.299f + img->dados [1][i][j] * 0.587f + img->dados [2][i][j] * 0.114f;

        /* Modifica a imagem original. */
        ptr_canal0 = img->dados [0];
        for (i = 1; i < img->n_canais; i++)
        {
            for (j = 0; j < img->altura; j++)
                free (img->dados [i][j]);
            free (img->dados[i]);
        }
        img->n_canais = 1;
        free (img->dados);
        img->dados = (float***) malloc (sizeof (float**));
        img->dados [0] = ptr_canal0;
    }

    return (img);
}

/*----------------------------------------------------------------------------*/
/** Salva uma imagem em um arquivo dado.
 *
 * Parâmetros: Imagem* img: imagem a salvar.
 *             char* arquivo: caminho do arquivo a salvar.
 *
 * Valor de retorno: 0 se ocorreu algum erro, 1 do contrário. */

int salvaImagem (Imagem* img, char* arquivo)
{
	FILE* stream;

	if (img->n_canais != 1 && img->n_canais != 3)
	{
        printf ("salvaImagem: so pode salvar imagens com 1 ou 3 canais.\n");
        return (0);
	}

	/* Abre o arquivo. */
	stream = fopen (arquivo, "wb");
	if (!stream)
		return (0);

	/* Escreve os blocos. */
	if (!salvaHeaderBitmap (stream, img))
	{
		fclose (stream);
		return (0);
	}

	if (!salvaHeaderDIB (stream, img))
	{
		fclose (stream);
		return (0);
	}

	if (!salvaDados (stream, img))
	{
		fclose (stream);
		return (0);
	}

	fclose (stream);
	return (1);
}

/*----------------------------------------------------------------------------*/
/** Cria uma cópia de uma imagem.
 *
 * Parâmetros: Imagem* img: imagem a clonar.
 *
 * Valor de retorno: uma nova imagem, igual à imagem dada. */

Imagem* clonaImagem (Imagem* img)
{
    int i, j, k;
    Imagem* clone = criaImagem (img->largura, img->altura, img->n_canais);

    for (i = 0; i < img->n_canais; i++)
        for (j = 0; j < img->altura; j++)
            for (k = 0; k < img->largura; k++)
                clone->dados [i][j][k] = img->dados [i][j][k];

    return (clone);
}

/*----------------------------------------------------------------------------*/
/** Copia o conteúdo de uma imagem para outra.
 *
 * Parâmetros: Imagem* in: copia desta imagem...
 *             Imagem* out: ... para esta.
 *
 * Valor de retorno: nenhum. */

void copiaConteudo (Imagem* in, Imagem* out)
{
    int i, j, k;

    if (in->largura != out->largura || in->altura != out->altura || in->n_canais != out->n_canais)
    {
        printf ("ERRO: copiaConteudo: as imagens precisam ter o mesmo tamanho e numero de canais.\n");
        exit (1);
    }

    for (i = 0; i < in->n_canais; i++)
        for (j = 0; j < in->altura; j++)
            for (k = 0; k < in->largura; k++)
                out->dados [i][j][k] = in->dados [i][j][k];
}

/*============================================================================*/
/* FUNÇÕES INTERNAS (LEITURA)                                                 */
/*============================================================================*/
/** Pega os 4 primeiros bytes do buffer e coloca em um unsigned long,
 * considerando os bytes em ordem little endian.
 *
 * Parâmetros: unsigned char* buffer: lê 4 bytes daqui.
 *
 * Valor de Retorno: um unsigned long com os dados do buffer reorganizados. */

unsigned long getLittleEndianULong (unsigned char* buffer)
{
	return (buffer [3] << 24) | (buffer [2] << 16) | (buffer [1] << 8) | buffer [0];
}

/*----------------------------------------------------------------------------*/
/** Lê o header de 14 bytes do formato BMP.
 *
 * Parâmetros: FILE* stream: arquivo a ser lido. Supomos que já está aberto.
 *             unsigned long* offset: parâmetro de saída, é o deslocamento dos
 *               dados a partir do início do arquivo.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int leHeaderBitmap (FILE* stream, unsigned long* offset)
{
	unsigned char data [14]; /* O bloco tem exatamente 14 bytes. */

	if (fread ((void*) data, 1, 14, stream) != 14)
	{
		printf ("leHeaderBitmap: erro lendo header.\n");
		return (0);
	}

	/* Os 2 primeiros bytes precisam ser 'B' e 'M'. */
	if (data [0] != 'B' || data [1] != 'M')
	{
		printf ("leHeaderBitmap: pode ler apenas formato BM.\n");
		return (0);
	}

	/* Vou pular todo o resto e ir direto para o offset. */
	*offset = getLittleEndianULong (&(data [10]));
	return (1);
}

/*----------------------------------------------------------------------------*/
/** Lê o header DIB.
 *
 * Parâmetros: FILE* stream: arquivo a ser lido. Supomos que já está aberto.
 *             unsigned long* largura: parâmetro de saída. Largura da imagem.
 *             unsigned long* altura: parâmetro de saída. Altura da imagem.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int leHeaderDIB (FILE* stream, unsigned long* largura, unsigned long* altura)
{
	unsigned long size = 0; /* O tamanho do cabeçalho DIB. */

	if (fread ((void*) &size, 4, 1, stream) != 1)
	{
		printf ("leHeaderDIB: erro lendo header.\n");
		return (0);
	}

	if (size == 12) /* Formato BITMAPCOREHEADER. */
	{
		printf ("leHeaderDIB: BITMAPCOREHEADER nao suportado (arquivo antigo!?)\n");
		return (0);
	}
	else if (size >= 40) /* Outros formatos. */
	{
		unsigned short tmp_short = 0;
		unsigned long tmp_long = 0;

		/* Largura. */
		if (fread ((void*) largura, 4, 1, stream) != 1 || *largura <= 0)
		{
			printf ("leHeaderDIB: largura invalida.\n");
			return (0);
		}

		/* Altura. */
		if (fread ((void*) altura, 4, 1, stream) != 1 || *altura <= 0)
		{
			printf ("leHeaderDIB: altura invalida.\n");
			return (0);
		}

		/* Color planes. Precisa ser 1. */
		if (fread ((void*) &tmp_short, 2, 1, stream) != 1 || tmp_short != 1)
		{
			printf ("leHeaderDIB: erro lendo header.\n");
			return (0);
		}

		/* Bpp. Aqui, estou forçando 24 bpp. */
		if (fread ((void*) &tmp_short, 2, 1, stream) != 1 || tmp_short != 24)
		{
			printf ("leHeaderDIB: supporta apenas arquivos com 24 bpp.\n");
			return (0);
		}

		/* Compressão. Vou aceitar só imagens sem compressão. */
		if (fread ((void*) &tmp_long, 4, 1, stream) != 1 || tmp_long != 0)
		{
			printf ("leHeaderDIB: suporta apenas arquivos sem compressao.\n");
			return (0);
		}

		/* Pula os próximos 12 bytes. */
		if (fseek (stream, 12, SEEK_CUR) != 0)
		{
			printf ("leHeaderDIB: erro lendo header.\n");
			return (0);
		}

		/* Paleta. Não é para usar! */
		if (fread ((void*) &tmp_long, 4, 1, stream) != 1 || tmp_long != 0)
		{
			printf ("leHeaderDIB: sem suporte a arquivos com paletas de cores.\n");
			return (0);
		}

		return (1);
	}

	return (0);
}

/*----------------------------------------------------------------------------*/
/** Lê os dados de um arquivo.
 *
 * Parâmetros: FILE* stream: arquivo a ser lido. Supomos que já está aberto.
 *             Imagem* img: imagem a preencher.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int leDados (FILE* stream, Imagem* img)
{
	long long i, j, k;
	int line_padding;
	unsigned char data;

	/* Calcula quantos bytes preciso pular no fim de cada linha.
	  Aqui, cada linha precisa ter um múltiplo de 4. */
	line_padding = (int) ceil (img->largura*3.0/4.0)*4 - (img->largura*3);

	/* Lê! */
	for (i = img->altura-1; i >= 0; i--)
	{
		for (j = 0; j < img->largura; j++)
		{
		    /* Lê 3 bytes, na ordem BGR. */
		    for (k = 0; k < 3; k++)
		    {
                if (fread (&data, 1, 1, stream) != 1)
                    return (0);

                img->dados [2-k][i][j] = (float) data / 255.0f; // 2-k -> 0 = R, 1 = G, 2 = B. Coloca na faixa [0,1]
		    }
		}

		if (fseek (stream, line_padding, SEEK_CUR) != 0)
			return (0);
	}

	return (1);
}

/*============================================================================*/
/* FUNÇÕES INTERNAS (ESCRITA)                                                 */
/*============================================================================*/
/** Coloca um unsigned long nos 4 primeiros bytes do buffer, em ordem little
 * endian.
 *
 * Parâmetros: unsigned long val: valor a escrever.
 *             unsigned char* buffer: coloca o valor aqui.
 *
 * Valor de Retorno: NENHUM */

void putLittleEndianULong (unsigned long val, unsigned char* buffer)
{
	buffer [0] = (unsigned char) val;
	buffer [1] = (unsigned char) (val >> 8);
	buffer [2] = (unsigned char) (val >> 16);
	buffer [3] = (unsigned char) (val >> 24);
}

/*----------------------------------------------------------------------------*/
/** Coloca um unsigned short nos 2 primeiros bytes do buffer, em ordem little
 * endian.
 *
 * Parâmetros: unsigned short val: valor a escrever.
 *             unsigned char* buffer: coloca o valor aqui.
 *
 * Valor de Retorno: NENHUM */

void putLittleEndianUShort (unsigned short val, unsigned char* buffer)
{
	buffer [0] = (unsigned char) val;
	buffer [1] = (unsigned char) (val >> 8);
}

/*----------------------------------------------------------------------------*/
/** Converte UM valor float para unisgned char. Remove o sinal e arredonda
 * para o número mais próximo, truncando em 255.
 *
 * Parâmetros: floax x: valor a converter.
 *
 * Valor de retorno: ver acima. */

unsigned char float2uchar (float x)
{
    if (x < 0)
        x = -x;

    return ((unsigned char) MIN (255, (255.0f*x) + 0.5f));
}

/*----------------------------------------------------------------------------*/
/** Escreve o header Bitmap.
 *
 * Parâmetros: FILE* file: arquivo a ser escrito. Supomos que já está aberto.
 *             Imagem* img: imagem a ser salva.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int salvaHeaderBitmap (FILE* stream, Imagem* img)
{
	unsigned char data [14]; /* O bloco tem exatamente 14 bytes. */
	int pos = 0;
	unsigned long bytes_por_linha;

	data [pos++] = 'B';
	data [pos++] = 'M';

	/* Tamanho do arquivo. Definimos como sendo 14+40 (dos cabeçalhos) + o espaço dos dados. */
	bytes_por_linha = (unsigned long) ceil (img->largura*3.0/4.0)*4;
	putLittleEndianULong (14+40+img->altura*bytes_por_linha, &(data [pos]));
	pos+=4;

	/* Reservado. */
	putLittleEndianULong (0, &(data [pos]));
	pos+=4;

	/* Offset. Definimos como 14+40 (o tamanho dos cabeçalhos). */
	putLittleEndianULong (14+40, &(data [pos]));

	if (fwrite ((void*) data, 1, 14, stream) != 14)
	{
		printf ("salvaHeaderBitmap: erro escrevendo header.\n");
		return (0);
	}

	return (1);
}

/*----------------------------------------------------------------------------*/
/** Escreve o header DIB.
 *
 * Parâmetros: FILE* file: arquivo a ser escrito. Supomos que já está aberto.
 *             Imagem* img: imagem a ser salva.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int salvaHeaderDIB (FILE* stream, Imagem* img)
{
	unsigned char data [40]; /* O bloco tem exatamente 40 bytes. */
	int pos = 0;
	unsigned long bytes_por_linha;

	/* Tamanho do header. Vamos usar um BITMAPINFOHEADER. */
	putLittleEndianULong (40, &(data [pos]));
	pos += 4;

	/* Largura. */
	putLittleEndianULong (img->largura, &(data [pos]));
	pos += 4;

	/* Altura. */
	putLittleEndianULong (img->altura, &(data [pos]));
	pos += 4;

	/* Color planes. */
	putLittleEndianUShort (1, &(data [pos]));
	pos += 2;

	/* bpp. */
	putLittleEndianUShort (24, &(data [pos]));
	pos += 2;

	/* Compressão. */
	putLittleEndianULong (0, &(data [pos]));
	pos += 4;

	/* Tamanho dos dados. */
	bytes_por_linha = (unsigned long) ceil (img->largura*3.0/4.0)*4;
	putLittleEndianULong (img->altura*bytes_por_linha, &(data [pos]));
	pos += 4;

	/* Resolução horizontal e vertical (simplesmente copiei este valor de algum arquivo!). */
	putLittleEndianULong (0xF61, &(data [pos]));
	pos += 4;
	putLittleEndianULong (0xF61, &(data [pos]));
	pos += 4;

	/* Cores. */
	putLittleEndianULong (0, &(data [pos]));
	pos += 4;
	putLittleEndianULong (0, &(data [pos]));
	pos += 4;

	if (fwrite ((void*) data, 1, 40, stream) != 40)
	{
		printf ("salvaHeaderDIB: erro escrevendo header.\n");
		return (0);
	}

	return (1);
}

/*----------------------------------------------------------------------------*/
/** Escreve o bloco de dados.
 *
 * Parâmetros: FILE* file: arquivo a ser escrito. Supomos que já está aberto.
 *             Imagem* img: imagem a ser salva.
 *
 * Valor de Retorno: 1 se não ocorreram erros, 0 do contrário. */

int salvaDados (FILE* stream, Imagem* img)
{
    unsigned char dado;
	long long i, j, k;
	unsigned long largura_linha, line_padding;
	unsigned char* linha;
	unsigned long pos_linha;

	/* Calcula quantos bytes preciso pular no fim de cada linha.
	  Aqui, cada linha precisa ter um múltiplo de 4. */
	largura_linha = (unsigned long) ceil (img->largura*3.0/4.0)*4;
	line_padding = largura_linha - (img->largura*3);
	linha = (unsigned char*) malloc (sizeof (unsigned char) * largura_linha);

    for (i = img->altura-1; i >= 0; i--)
	{
		pos_linha = 0;
		for (j = 0; j < img->largura; j++)
		{
		    /* Imagem de 3 canais: converte para uchar na ordem BGR. */
		    if (img->n_canais == 3)
		    {
		        for (k = 0; k < 3; k++)
		            linha [pos_linha++] = float2uchar (img->dados [2-k][i][j]);
		    }
		    else /* Imagem de 1 canal: converte para uchar e replica 3 vezes. */
		    {
		        dado = float2uchar (img->dados [0][i][j]);
                for (k = 0; k < 3; k++)
                    linha [pos_linha++] = dado;
            }
		}

		for (j = 0; j < line_padding; j++)
			linha [pos_linha++] = 0;

		if (fwrite ((void*) linha, 1, largura_linha, stream) != largura_linha)
		{
			printf ("salvaDados: errro escrevendo dados da imagem.\n");
			free (linha);
			return (0);
		}
	}

	free (linha);
	return (1);
}

/*============================================================================*/
