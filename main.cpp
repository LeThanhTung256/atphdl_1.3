#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int sizeOfBlock = 12;
const int sizeOfEncryptedBlock = 16;
const char* encryptedFile = "encrypt.txt";
const char* decryptedFile = "decrypt.txt";

char* encryptBlock(char* data, int len) {
  int lenOfEncr = len + ceil(len / 3.0);
  char* encryptedBlock = new char[lenOfEncr];
  srand(time(NULL));

  //B1: Tăng byte thứ hai lên và giảm byte thứ 3 đi n giá trị (với n là giá trị của byte số 1);
  for (int i = 0; i < len; i++) {
    if (i % 3 == 1) {
      data[i] = char((data[i] + data[i - 1]) % 256);
    } else if (i % 3 == 2) {
      data[i] = char((data[i] - data[i - 2] + 256) % 256);
    }
  }

  //B2: Đổi chỗ byte 1 với byte 3, thêm rác ở byte 4
  for (int i = 0; i < len / 3; i++) {
    encryptedBlock[i * 4] = data[i * 3 + 2];
    encryptedBlock[i * 4 + 1] = data[i * 3 +1];
    encryptedBlock[i * 4 + 2] = data[i * 3];
    unsigned int trash = rand() % 256;
    encryptedBlock[i * 4 + 3] = char(trash);
  }

  //nhóm cuối không đủ 3 byte thì giữ nguyên vị trí và thêm rác vào byte cuối cùng
  if (len % 3 != 0) {
    int remain = len % 3;
    for (int i = remain; i > 0 ; i--) {
      encryptedBlock[lenOfEncr - i - 1] = data[len - i];
    }
    unsigned int trash = rand() % 256;
    encryptedBlock[lenOfEncr - 1] = char(trash);
  }

  //B3: Xor với hash của pashwork

  return encryptedBlock;
}

char* decryptBlock(char* data, int len) {
  int lenOfDecry = len - ceil(len / 4.0);
  char* decryptedBlock = new char[lenOfDecry];

  // B1: Xor với hash của pashwork

  // B2: Đổi chỗ byte 1 và 3, xoá byte 4
  for (int i = 0; i < len / 4; i++) {
    decryptedBlock[i * 3] = data[i * 4 + 2];
    decryptedBlock[i * 3 + 1] = data[i * 4 + 1];
    decryptedBlock[i * 3 + 2] = data[i * 4];
  }

  // Nếu k đủ 4 byte thì xoá rác và dữ nguyên tất cả
  if (len % 4 != 0) {
    int remain = len % 4;
    for (int i = remain - 1; i > 0; i--) {
      decryptedBlock[lenOfDecry - i] = data[len - i -1];
    }
  }

  // B3: Giảm byte 2 và tăng byte 3 lên byte 1 giá trị
  for (int i = 0; i < lenOfDecry; i++) {
    if (i % 3 == 1) {
      decryptedBlock[i] = char((decryptedBlock[i] - decryptedBlock[i - 1] + 256) % 256);
    } else if (i % 3 == 2) {
      decryptedBlock[i] = char((decryptedBlock[i] + decryptedBlock[i - 2]) % 256);
    }
  }

  return decryptedBlock;
}

int encryption(char* fileInName, char* fileOutName) {
  //Nhâp tên file muốn mã hoá
  // const size_t BUFFER_SIZE = 1024;
  // char *inputFileName = new char[BUFFER_SIZE];
  // cout << "Filename: ";
  // cin >> inputFileName;
  cout << "File cần mã hoá: " << fileInName << endl;
  cout << "File kết quả: " << fileOutName << endl;
  
  //Mở file đọc và file ghi
  fstream fileIn, fileOut;
  fileIn.open(fileInName, ios::in | ios::binary);
  fileOut.open(fileOutName, ios::out);

  if (!fileIn) {
    cerr << "Error: file not opened" << endl;
    return 0;
  }

  //Tính số lượng bytes của file
  fileIn.seekg(0, ios::end);
  long sizeOfFile = fileIn.tellg();
  fileIn.seekg(0);

  // Mã hoá và ghi vào file kết quả
  long remainingBytes = sizeOfFile;
  while(!fileIn.eof()) {
    char* data = new char [sizeOfBlock];
    if (remainingBytes > sizeOfBlock) {
      fileIn.read(data, sizeOfBlock);
      char* encryt = encryptBlock(data, sizeOfBlock);
      fileOut.write(encryt, strlen(encryt));
      remainingBytes -= sizeOfBlock;
      delete [] encryt;
      delete[] data;
    } else {
      char *lastBlock =  new char [remainingBytes];
      fileIn.read(lastBlock, remainingBytes);
      char* encryt = encryptBlock(lastBlock, remainingBytes);
      fileOut.write(encryt, strlen(encryt));
      delete [] encryt;
      delete [] lastBlock;

      break;
    }
  }

  fileIn.close();
  fileOut.close();
  cout << "Mã hoá thành công!" << endl;

  return 1;
}

int decryption(char* fileInName, char* fileOutName) {
  //Nhâp tên file muốn giải mã
  // const size_t BUFFER_SIZE = 1024;
  // char *inputFileName = new char[BUFFER_SIZE];
  // cout << "Filename: ";
  // cin >> inputFileName;

  cout << "File cần giải mã: " << fileInName << endl;
  cout << "File kết quả: " << fileOutName << endl;
  
  //Mở file đọc và file ghi
  fstream fileIn, fileOut;
  fileIn.open(fileInName, ios::in | ios::binary);
  fileOut.open(fileOutName, ios::out);

  if (!fileIn) {
    cerr << "Error: file not opened" << endl;
    return 0;
  }

  //Tính số lượng bytes của file
  fileIn.seekg(0, ios::end);
  long sizeOfFile = fileIn.tellg();
  fileIn.seekg(0);

  // giải mã và ghi vào file kết quả
  long remainingBytes = sizeOfFile;
  while(fileIn) {
    char* data = new char[sizeOfEncryptedBlock];
    if (remainingBytes >= sizeOfEncryptedBlock) {
      fileIn.read(data, sizeOfEncryptedBlock);
      char* decryt = decryptBlock(data, sizeOfEncryptedBlock);
      fileOut.write(decryt, strlen(decryt));
      remainingBytes -= sizeOfEncryptedBlock;
      delete [] decryt;
    } else {
      fileIn.read(data, remainingBytes);
      char* decryt = decryptBlock(data, remainingBytes);
      fileOut.write(decryt, strlen(decryt));
      delete [] decryt;

      break;
    }
  }

  fileIn.close();
  fileOut.close();
  cout << "Giải mã thành công!" << endl;

  return 1;
}

void printTutorial() {
  cout << "- - - - - - - - - - - - - - - -       Mã hoá dữ liệu      - - - - - - - - - - - - - - -" << endl;
  cout << "| * ./run.out encrypt fileIn fileOut: mã hoá fileIn, lưu kết quả trong fileout        |" << endl;
  cout << "|                                                                                     |" << endl;
  cout << "| * ./run.out decrypt fileIn fileOut: giải mã fileIn, luư kết quả trong file fileout  |" << endl;
  cout << "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << endl;
}

int main( int argc, char *argv[]) {
  // int a = decryption();
	if (argc != 4) {
    cout << "Sai cú pháp!" << endl;
    printTutorial();
  }

  if (!strcmp(argv[1], "encrypt")) {
    int result = encryption(argv[2], argv[3]);
    return result;
  }

  if (!strcmp(argv[1], "decrypt")) {
    int result = decryption(argv[2], argv[3]);
    return result;
  }
	
	return 0;
}