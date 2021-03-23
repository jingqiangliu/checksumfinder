#include <stdio.h>
#include <stdlib.h>
#include "crc32.h"


unsigned long update_crc(unsigned long crc, const unsigned char *buf, int len) {
	unsigned long c = crc;

	for (int n = 0; n < len; n++) c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
	return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
unsigned long crc32_3(const unsigned char *buf, int len) {
	return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

int reading_input_from_file(const unsigned char *buffer, int len) //, const unsigned char *crc_buffer)
{

	if (fread((void*)buffer, 1, len, f) != len)
	{
		printf("file buffer small!\n");
		fclose(f);
		return 0;
	}

	return len;
}

int read_file(const unsigned char *buffer, int maxlen){
	fseek(f, 0, SEEK_END);
	int fileLen=ftell(f);
	fseek(f, 0, SEEK_SET);

	if (fileLen < maxlen){
		fread((void*)buffer, 1, fileLen, f);
		return fileLen;
	}else{
		printf("file too larger:%d limit::%d\n", fileLen, maxlen);
		return 0;
	}
}

FILE *f = NULL; 
unsigned char buffer[10*1024] = {0};
unsigned char crc_buffer[5] = {0};


int main(int argc, char* argv[])
{

	if (argc != 2){
		printf("USAGE: %s seed_path\n", argv[0]);
		exit(0);
	}

	f = fopen(argv[1], "rb");
	if (f == NULL)
	{
		printf("%s not found!\n", argv[1]);
		return 0;
	}

	int filelen = read_file(buffer, sizeof(buffer)-1);

	int crc = 0;

	int crc_pos = 0;
	int lastrightpos = 0;

	for (int data_pos = 0; data_pos < filelen; data_pos+=1){

		if (data_pos < lastrightpos){
			continue;
		}

		for (int data_len = 4; data_len < filelen; data_len += 1){

			int calc_crc = crc32( 0, buffer + data_pos, data_len);

			for (crc_pos = 0; crc_pos < filelen; crc_pos+=1){

				if (crc_pos >= data_pos && crc_pos < data_pos + data_len){
					continue;
				}

				// limit to 64 now
				if (crc_pos >= data_pos + data_len + 64 || crc_pos <= data_pos - 64 ){
					continue;
				}

				crc = *(int *)(buffer + crc_pos);
				

				crc  = ((crc & 0xff) << 24) + (((crc >> 8) & 0xff) << 16) +  (((crc >> 16) & 0xff) << 8) + ((crc >> 24) & 0xff);

				int right = crc == calc_crc;
				if (right){
					printf("crc32: data_pos:%d data_len:%d crc_pos:%d crc:%x\n", data_pos, data_len, crc_pos, crc);
					lastrightpos = data_pos + data_len > crc_pos ? data_pos + data_len : crc_pos;
				}
			}
		}
	}
	
	return 0;
}
