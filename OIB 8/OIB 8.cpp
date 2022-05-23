// OIB 8.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
                                                                        
typedef  unsigned char BYTE;
typedef union {
    unsigned short word;
    BYTE str[2];
} WORD;
typedef union {
    unsigned long dword;
    BYTE str[4];
} DWORD;


typedef enum enumMode {
    ONE_BIT,
    TWO_BIT,
} Mode;

void readBYTE(FILE* file, BYTE *byte) {
    fscanf(file, "%c", byte);
}
void readWORD(FILE* file, WORD* word) {
    fread(word->str, 2, 1, file);
}
void readDWORD(FILE* file, DWORD *dword) {
    fread(dword->str, 4, 1, file);
}


unsigned long findSize(FILE* file) {
    DWORD lenght;
    DWORD hight;
    fseek(file, 18, SEEK_SET);
    readDWORD(file, &lenght);
    fseek(file, 22, SEEK_SET);
    readDWORD(file, &hight);
    
    return (lenght.dword * hight.dword * 3);

}
unsigned long compareSize(long byteSizePic, char* str, Mode mode_work) {              // возвращает длину записываемой строки в битах
    unsigned long bitStrLen = strlen(str) * 8;
    byteSizePic -= 11;

    if (mode_work == TWO_BIT) byteSizePic *= 2;
                                                                              // 10 байт для информации о длине, 1 байт для информации о кодировке
    
    if (byteSizePic >= bitStrLen) return bitStrLen;
    else {
        printf("Error. Bytes in bmp (%lu) < bits from string (%lu) \n", byteSizePic, bitStrLen);
        return 0;
    }
}

void stegText(FILE* infile, FILE* outfile, char* str, Mode mode_work) {
    DWORD entryPoint;

    long int posEND;
    fseek(infile, -11, SEEK_END);
    posEND = ftell(infile);

    fseek(infile, 10, SEEK_SET);
    readDWORD(infile, &entryPoint);
    rewind(infile);

    BYTE tmp;
    int lenght = strlen(str);

    for (unsigned long i = 0; i < entryPoint.dword; i++) {
        readBYTE(infile, &tmp);
        fprintf(outfile, "%c", tmp);
    }

    if (mode_work == ONE_BIT) {
        for (int i = 0; i < lenght; i++) {
            for (int j = 0; j < 8; j++) {
                readBYTE(infile, &tmp);
                if (str[i] % 2 == 1) tmp |= (1 << 0);
                else tmp &= ~(1 << 0);
                fprintf(outfile, "%c", tmp);
                str[i] = str[i] / 2;
            }
        }
    }
    else {
        for (int i = 0; i < lenght; i++) {
            for (int j = 0; j < 4; j++) {
                readBYTE(infile, &tmp);
                if (str[i] % 2 == 1) tmp |= (1 << 0);
                else tmp &= ~(1 << 0);
                str[i] = str[i] / 2;
                if (str[i] % 2 == 1) tmp |= (1 << 1);
                else tmp &= ~(1 << 1);
                str[i] = str[i] / 2;
                fprintf(outfile, "%c", tmp);
            }
        }
    }

    while (ftell(infile) != posEND) {
        readBYTE(infile, &tmp);
        fprintf(outfile, "%c", tmp);

    }

    readBYTE(infile, &tmp);
    if(mode_work == ONE_BIT) tmp &= ~(1 << 0);               // метка о кодировании один бит в один байт
    else tmp |= (1 << 0);                                   // аналогично для двух
    fprintf(outfile, "%c", tmp);

    for (int i = 0; i < 10; i++) {
        readBYTE(infile, &tmp);
        if (lenght % 2 == 1) tmp |= (1 << 0); 
        else tmp &= ~(1 << 0);
        fprintf(outfile, "%c", tmp);
        lenght = lenght / 2;
    }
}

// x &= ~(1 << 0); - НОЛЬ В МЛАДШИЙ БИТ
// x |= (1 << 0);  - ЕДИНИЦА В МЛАДШИЙ БИТ
int main()
{
    Mode stegMode;
    char str[1000];
    memset(str, 0, 1000);
    FILE* picture = fopen("C:/Users/vilda/OneDrive/Рабочий стол/OIB 8/stego.bmp", "rb");

    printf("Choose mode:\n1) One bit\n2) Two bits\n");
    int choice;
    scanf("%d", &choice);
    
    if (choice == 1)stegMode = ONE_BIT;
    if (choice == 2)stegMode = TWO_BIT;

    printf("Enter the text\n");
    while (getchar() != '\n') {};
    fgets(str, 1000, stdin);
    
    unsigned long pictureBytes = findSize(picture);
    unsigned long strLenBits = compareSize(pictureBytes, str, stegMode);
    if (strLenBits == 0)    return 0;
    FILE* stegPicture = fopen("C:/Users/vilda/OneDrive/Рабочий стол/OIB 8/stegogg.bmp", "wb");

    stegText(picture, stegPicture, str, stegMode);

    fclose(picture);
    fclose(stegPicture);
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
