#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


FILE *inputStream;
FILE *outputStream;

#define IO_ERROR 0
#define IO_SUCCESS 1
int currentChar;
int numberLetter=0;
int numberDigit=0;

int readChar(void) {
  currentChar = getc(inputStream);
    if (isalpha(currentChar))
        numberLetter+=1;
    else if (isdigit(currentChar))
        numberDigit+=1;
  return currentChar;
}

int openInputStream(char *fileName) {
  inputStream = fopen(fileName, "rt");
  if (inputStream == NULL)
    return IO_ERROR;

  return IO_SUCCESS;
}

int writeToFile(char *filename){
    outputStream = fopen(filename,"w");
    if (outputStream == NULL)
        return IO_ERROR;
    fprintf(outputStream,"Number of digits: ",numberDigit);
    fprintf(outputStream,"Number of letters: ",numberLetter);

    fclose(outputStream);

    return IO_SUCCESS;
}

void closeInputStream() {
  fclose(inputStream);
}

int scan(char *fileName) {
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  while (!feof(inputStream)) {
    readChar();
  }

  closeInputStream();
  return IO_SUCCESS;
}

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }
  else if (argc<=2){
    printf("scanner: no output file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
  if(writeToFile(argv[2]) == IO_ERROR){
      printf("Can\'t read output file!\n");
      return -1;
  }
    
  printf("SUCCESS!");
  return 0;
}