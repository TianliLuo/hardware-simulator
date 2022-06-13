#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>

int main(int argc, char* argv[]) {
	struct crypt_data data;
  char* a = argv[2];
  // int b  = 1;
  //  int c1 = 1;
  //   int c2  = 1;
  //    int c3  = 1;
  // char b1 = (char) b;
  // char b2 = (char) c1;
  // char b3 = (char) c2;
  // char b4 = (char) c3;
	puts(crypt_r(argv[1], argv[2], &data));
  // printf("%s",c);
	return 0;
}
