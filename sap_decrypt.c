#include <stdlib.h>
#include <stdio.h>

char *decrypt(unsigned char *data, int size, int version, unsigned char **out)
{
	int i, keybyte = 0;
	unsigned char key[] = {66,67,51,55,51,65,67,65,50,55,57,50,52,69,66,69,65,50,57,68,50,65,50,50,69,51,52,56,65,67,66,52,0};

	(void)version;

	for(i = 0; i < size; i++) {
		unsigned char byte = data[i] + 106 - key[keybyte];

		byte += (i % 5) ? 2 : 0;
		byte -= (i % 7) ? 9 : 0;
		byte += (i % 3) ? 3 : 0;

		(*out)[i] = ~byte - 27;

		keybyte = key[++keybyte] ? keybyte : 0;
	}

	if((*out)[0] == 255 && (*out)[1] == 254)
		return "UTF16";
	else if((*out)[0] == 239 && (*out)[1] == 187 && (*out)[2] == 191)
		return "UTF8";
	else
		return "1252";
}

int main(int argc, char *argv[])
{
	char *enc;
	unsigned char *buffer, *outbuf;
	FILE *out, *file;
	size_t filesize = 0;

	if(argc > 1)
		file = fopen(argv[1], "rb");
	else
		file = fopen("RCData4.bin", "rb");

	if(!file)
	{
		fprintf(stderr, "failed to open RCData file");
		if(argc > 1)
			fprintf(stderr, ": %s\n", argv[1]);
		else
			fprintf(stderr, ": RCData4.bin\n");
		return 1;
	}

	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = calloc(filesize + 1, 1);
	outbuf = calloc(filesize + 1, 1);

	if(!outbuf || !buffer)
	{
		fprintf(stderr, "out of memory\n");
		return 1;
	}

	fread(buffer, filesize, 1, file);
	fclose(file);

	enc = decrypt(buffer, filesize, 0, &outbuf);
	out = fopen("out.txt", "wb");
	fprintf(stdout, "dumping decrypted contents to out.txt\n");
	fprintf(out, "%s", outbuf);
	fclose(out);
	free(outbuf);
	free(buffer);
	fprintf(stderr, "file encoding is %s\n", enc);

	return 0;
}
