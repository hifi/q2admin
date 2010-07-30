#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 10240

int main(int argc, char **argv)
{
	unsigned char *buf;
	ssize_t size, i;

	buf = malloc(BUF_SIZE);
	if(!buf) {
		perror("Error allocating memory");
		return 1;
	}

	size = read(0, buf, BUF_SIZE);
	if(size < 0) {
		perror("Error reading from stdin");
		return 1;
	}

	if(size > 0) {
		printf("unsigned char %s[%d] = {\n\t", argc > 1 ? argv[1] : "bin2hex", size);

		for(i=0; i<size; i++) {

			printf("0x%02X", *(buf+i));

			if((i+1) < size) {
				printf(", ");
				if((i+1) % 8 == 0) {
					printf("\n\t");
				}
			}
		}

		printf("\n};\n");
	}

	free(buf);

	return 0;
}
