#include <stdio.h>
#include <stdlib.h>
#define MAXLEN 512

//unsigned char keystr[] = "\
30820139020100024100B1012F96D1FF953C23053A80CB8973CF08D09172BAC949654CF2075FC85A0C1A0FE0590803876AB580A78B0FAF5E2CF21BB9177163125DB3FFDDD52A11AAACCF0203010001024018808185EF2BE20CA10C683CCBF00B07EDC07161E0595CBF991727CE2A7DCC8ED3CE1C5BE2C08EC0AA7CABF511A436CF0D4F575F7469750DA5ECBB8BEDB4E581022100C13F97BE9392950280A17FF4A11AA5C1DF09D8F2535E3379FF4BC6C3C897BD53022100EA7B46A6050D2C5A3EDB6BAD68C0CE6EE113584408AE0742B8F0D45CFE8DA71502200414BBB7A2962DDF67A80173C11336FCE920E41BF9F6BB4F903CA16DF10683510220596E934F96F5A348F85870053FC2E9F958572C7F266059DC94F525CA223C9CD1022016016C52B990330A5A32747C77D599B287F016426EEE3D77F5DE3DD402EBB2FA";


unsigned char keystr[] = "hello_snow";
int main(int argc, char *argv[]) {

	FILE * outfile;
	unsigned char buf[MAXLEN];
	int outlen = sizeof(keystr);
	int i = 0;
/*
	for(i=0; i<outlen; i++) {
		unsigned char tp = keystr[i*2];
		unsigned char sum = (tp>='A' ? tp-'A'+10 : tp-'0');
		sum <<= 4;
		tp = keystr[i*2+1];
		sum += (tp>='A' ? tp-'A'+10 : tp-'0');
		buf[i] = sum;
	}
*/
	for(i=0; i<outlen; i++)
		buf[i] = keystr[i];
	outfile = fopen("msg", "wb" );
	if( outfile == NULL)
	{
		printf("outfile error/n");
		exit(1);
	}   
	fwrite(buf, sizeof(unsigned char), outlen-1, outfile);
	fclose(outfile);
	printf("outlen %d\n", outlen);
	return 0;
}
