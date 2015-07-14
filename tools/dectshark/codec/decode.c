/*
 * decode.c
 *
 * CCITT ADPCM decoder
 * Modified in 2009 by Arne Bochem <g726 dot decode at ps-auxw dot de> to support G.726 decoding.
 *
 * Usage : decode [-3|4|5] [-a|u|l] < infile > outfile
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "g72x.h"
#include "spandsp/g726.h"

#define BLOCKSIZE 1024

//typedef struct g726_state_s g726_state_t;

/*
 * Unpack input codes and pass them back as bytes.
 * Returns 1 if there is residual input, returns -1 if eof, else returns 0.
 */
int
unpack_input(
	unsigned char		*code,
	int			bits)
{
	static unsigned int	in_buffer = 0;
	static int		in_bits = 0;
	unsigned char		in_byte;

	if (in_bits < bits) {
		if (fread(&in_byte, sizeof (char), 1, stdin) != 1) {
			*code = 0;
			return (-1);
		}
		in_buffer |= (in_byte << in_bits);
		in_bits += 8;
	}
	*code = in_buffer & ((1 << bits) - 1);
	in_buffer >>= bits;
	in_bits -= bits;
	return (in_bits > 0);
}

main(
	int			argc,
	char			**argv)
{
	short			sample;
	unsigned char		code;
	int			n;
	struct g72x_state	state;
	int			out_coding;
	int			out_size;
	int			(*dec_routine)();
	int			dec_bits;
	g726_state_t *dec_state = NULL;
	int g726_out_coding;
	int g726_packing;
	uint8_t in_buf[BLOCKSIZE];
	uint16_t *out_buf;
	int len;
	int samp;

	g72x_init_state(&state);
	out_coding = AUDIO_ENCODING_ULAW;
	out_size = sizeof (char);
	dec_routine = NULL; //g721_decoder;
	dec_bits = 4;
	g726_out_coding = G726_ENCODING_LINEAR;
	g726_packing = G726_PACKING_LEFT;

	/* Process encoding argument, if any */
	while ((argc > 1) && (argv[1][0] == '-')) {
		switch (argv[1][1]) {
		case '3':
			dec_routine = g723_24_decoder;
			dec_bits = 3;
			break;
		case '4':
			dec_routine = g721_decoder;
			dec_bits = 4;
			break;
		case '5':
			dec_routine = g723_40_decoder;
			dec_bits = 5;
			break;
		case 'u':
			out_coding = AUDIO_ENCODING_ULAW;
			g726_out_coding = G726_ENCODING_ULAW;
			out_size = sizeof (char);
			break;
		case 'a':
			out_coding = AUDIO_ENCODING_ALAW;
			g726_out_coding = G726_ENCODING_ALAW;
			out_size = sizeof (char);
			break;
		case 'l':
			out_coding = AUDIO_ENCODING_LINEAR;
			g726_out_coding = G726_ENCODING_LINEAR;
			out_size = sizeof (short);
			break;
		case 'n':
			g726_packing = G726_PACKING_NONE;
			break;
		case 'L':
			g726_packing = G726_PACKING_LEFT;
			break;
		case 'R':
			g726_packing = G726_PACKING_RIGHT;
			break;
		case '6':
				switch (argv[1][3]) {
					case '2':
						dec_routine = NULL;
						dec_bits = 2;
						break;
					case '3':
						dec_routine = NULL;
						dec_bits = 3;
						break;
					case '4':
						dec_routine = NULL;
						dec_bits = 4;
						break;
					case '5':
						dec_routine = NULL;
						dec_bits = 5;
						break;
				}
			break;
		default:
fprintf(stderr, "CCITT ADPCM Decoder -- usage:\n");
fprintf(stderr, "\tdecode [-3|4|5|62|63|64|65] [-a|u|l] [-n|L|R] < infile > outfile\n");
fprintf(stderr, "where:\n");
fprintf(stderr, "\t-3\tProcess G.723 24kbps (3-bit) input data\n");
fprintf(stderr, "\t-4\tProcess G.721 32kbps (4-bit) input data\n");
fprintf(stderr, "\t-5\tProcess G.723 40kbps (5-bit) input data\n");
fprintf(stderr, "\t-62\tProcess G.726 16kbps (2-bit) input data\n");
fprintf(stderr, "\t-63\tProcess G.726 24kbps (3-bit) input data\n");
fprintf(stderr, "\t-64\tProcess G.726 32kbps (4-bit) input data [default]\n");
fprintf(stderr, "\t-65\tProcess G.726 40kbps (5-bit) input data\n");
fprintf(stderr, "\t-a\tGenerate 8-bit A-law data (broken with G.726)\n");
fprintf(stderr, "\t-u\tGenerate 8-bit u-law data (broken with G.726) [default]\n");
fprintf(stderr, "\t-l\tGenerate 16-bit linear PCM data [default for G.726]\n");
fprintf(stderr, "\t-n\tG.726 samples are unpacked\n");
fprintf(stderr, "\t-L\tG.726 samples are left packed [default]\n");
fprintf(stderr, "\t-R\tG.726 samples are right packed\n");
			exit(1);
		}
		argc--;
		argv++;
	}


	if (dec_routine != NULL) {
		/* Read and unpack input codes and process them */
		while (unpack_input(&code, dec_bits) >= 0) {
			sample = (*dec_routine)(code, out_coding, &state);
			if (out_size == 2) {
				fwrite(&sample, out_size, 1, stdout);
			} else {
				code = (unsigned char)sample;
				fwrite(&code, out_size, 1, stdout);
			}
		}
	} else {
		/* bitrate in 16000 24000 32000 40000 */
		/* ext_coding in G726_ENCODING_LINEAR G726_ENCODING_ULAW G726_ENCODING_ALAW */
		/* packing in G726_PACKING_NONE G726_PACKING_LEFT G726_PACKING_RIGHT */
		/* amp is uint16_t buffer f :: bitrate -> bits := 16k -> 2bit, 24k -> 3bit, 32k -> 4bit, 40k -> 5bit */
		/*                        so... length * 8 / f + 1 for good measure? */
		/* g726_data is uint8_t buffer */
		/* length of input data */
		/* g726_init(dec_state, bit_rate, ext_coding, packing); */
		/* g726_decode(dec_state, amp, g726_data, length); */
		dec_state = g726_init(dec_state, 8000 * dec_bits, g726_out_coding, g726_packing);
		if (dec_state == NULL) { fprintf(stderr, "Initialisation of decoder failed.\n"); exit(1); }
		out_buf = malloc(sizeof(uint16_t) * (BLOCKSIZE * 8 / dec_bits + 1));
		do {
			len = fread(in_buf, 1, BLOCKSIZE, stdin);
			samp = g726_decode(dec_state, out_buf, in_buf, len);
			fwrite(out_buf, sizeof(uint16_t), samp, stdout);
		} while (len == BLOCKSIZE);
	}
	fclose(stdout);
}
