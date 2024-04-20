#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#define MAX 1000

typedef struct info{
    char accName[50];
    char accPin[7];
    int accNum;
    int balance;
    int last;
}list;

list l;
list acc[MAX];


char regName[50];
char regPin[7];
char confirmPin[7];
char* flashDrive[2] = {"D:\\checkingFlashDrive.dbf", "G:\\CheckingFlashDrive.dbf"};
char* drives[2] = {"D:\\Acc.dbf","G:\\Acc.dbf"};
int regPinAcc, regAccNum, regAccBal, ctr = -1, exist = 0, checkName = 0, checkPin = 0, charPin = 0;

void makenull();
int insertCard();
void retrieve();
int checkFd();
void registration();
int isEmpty();
int isFull();
void insert();
int locate(int x);
void save();
void saving();

void delay(int ms);
void regInterface();
void gotoxy(int x, int y);
void printToxy(int x, int y, char* label);
COORD coord = {0,0};
void UI(int x, int y, int len, char *label);
void clear(char *label);
void readFd(int input);

int main(){
    int i;
    system("color E0");
    makenull();
    if(insertCard()){
        _getch();
        system("cls");
        retrieve();
        regInterface();
        registration();
    }
    i = remove(flashDrive[ctr]);
    gotoxy(0,25);
    _getch();
    return 0;
}

void makenull(){
    l.last = -1;
}

int insertCard(){
    FILE *fp;
    printToxy(44,2,"I N S E R T   C A R D . . .");
    UI(40, 7, 72, "");

    do{
        ctr = (ctr + 1) % 2;
        fp = fopen(flashDrive[ctr],"w");
    }while(fp == NULL);
    fclose(fp);

    fp = fopen(drives[ctr],"r");
    if(fp == NULL){
        printToxy(42,9,"N O T   R E G I S T E R E D .");
        readFd(0);
        printToxy(42,9,"                             ");
        printToxy(43,9,"P R E S S   A N Y   K E Y");
        return 1;
    }
    else{
        printToxy(46,9,"R E G I S T E R E D");
        return 0;
    }
    fclose(fp);
}

int checkFd(){
    FILE *fp;
    fp = fopen(flashDrive[ctr],"r");
    if(fp == NULL)
        return 1;
    return 0;
    fclose(fp);
}

void retrieve(){
    FILE *fp;
    int i = 0;
    fp = fopen("AtmBank.dbf","r+");

    if (fp==NULL){
        regInterface();
        printToxy(42,22,"D A T A B A S E   I S   E M P T Y");
        _getch();
        system("cls");
    }
    else{
        while(!feof(fp)){
            l.last++;
            fscanf(fp,"%d %[^\t]%d %[^\n]",&acc[l.last].accNum,acc[l.last].accName,&acc[l.last].balance,acc[l.last].accPin);
        }
    }
    fclose(fp);
}

void registration(){
    int end;
    do{
        end = 1;
        l.last++;
        checkName = 0;
        gotoxy(42, 6);
        scanf(" %[^\n]s",regName);
        strlwr(regName);

        do{
            srand(time(0));
            regAccNum = 10000+(rand()%2000);
        }while(locate(1));

        gotoxy(36, 10);
        printf("%d",regAccNum);

       do{
            printToxy(69,10,"                 ");
            gotoxy(69, 10);
            scanf("%d",&regAccBal);

            if(regAccBal < 4999)
                printToxy(32,22,"I N I T I A L   D E P O S I T   M U S T   B E   (PHP): 5000");
            else{
                clear("result");
                break;
            }
        }while(1);

        do{
            checkPin = 0;
            charPin = 0;
            gotoxy(55, 14);
            for(int i = 0; i < 6; i++){
                regPin[i] = _getch() + 9;
                putchar('*');
                putchar(' ');
            }
            regPin[6] = '\0';

            gotoxy(55, 18);
            for(int i = 0; i < 6; i++){
                confirmPin[i] = _getch() + 9;
                putchar('*');
                putchar(' ');
            }
            confirmPin[6] = '\0';

            end = strcmp(confirmPin,regPin);

            for(int i = 0; i < 6; i++){
                if(isdigit(confirmPin[i] - 9)  == 0){
                    charPin = 1;
                    break;
                }
            }

            if(end != 0){
                printToxy(42,22,"P I N   D I D   N O T   M A T C H !");
                _getch();
                clear("result");
                clear("pinBoxes");
            }

            else if(locate(0) && checkPin == 1){
                printToxy(46,22,"P I N   N O T   U N I Q U E");
                _getch();
                clear("result");
                clear("pinBoxes");
            }
            else if(charPin == 1){
                printToxy(36,22,"P I N   C O N T A I N S   A   C H A R A C T E R");
                _getch();
                clear("result");
                clear("pinBoxes");
            }

        }while(end != 0 || checkPin || charPin);

        if(locate(0) && checkName == 1){
            l.last--;
            printToxy(39,22,"A C C O U N T   E X I S T   A L R E A D Y");
            _getch();
            system("cls");
            regInterface();
        }

    }while(checkName);

    if(isFull())
        printToxy(43,22,"D A T A B A S E   I S   F U L L");
    else
        insert();
}

int isEmpty(){
    return(l.last == 0);
}

int isFull(){
    return(l.last == MAX);
}

void insert(){
    if(checkFd()){
        printToxy(43,22,"F L A S H   D R I V E   E R R O R");
        readFd(1);
        gotoxy(0,25);
        exit(0);
    }
    strcpy(acc[l.last].accName,regName);
    acc[l.last].accNum = regAccNum;
    acc[l.last].balance = regAccBal;
    strcpy(acc[l.last].accPin,regPin);
    printToxy(47,22,"R E G I S T E R E D !!!");
    save();
    saving();
}

int locate(int x){
    if(x == 0){
        for(int i = 0; i < l.last + 1; i++){
            if(strcmp(acc[i].accName,regName) == 0 || strcmp(acc[i].accPin,regPin) == 0){
                if(strcmp(acc[i].accName,regName) == 0)
                    checkName = 1;
                if(strcmp(acc[i].accPin,regPin) == 0)
                    checkPin = 1;
                return 1;
            }
        }
        return 0;
    }
    else if(x == 1){
        for(int i = 0; i < l.last + 1; i++){
            if(acc[i].accNum == regAccNum)
                return 1;
        }
        return 0;
   }
}

void save(){
    FILE *fp;
    fp = fopen("AtmBank.dbf","w");
    for(int i = 0; i <= l.last; i++){
        fprintf(fp,"%d\t%s\t%d\t%s",acc[i].accNum,acc[i].accName,acc[i].balance,acc[i].accPin);
        if(i != l.last)
            fprintf(fp,"\n");
    }
    fclose(fp);
}

void saving(){
    FILE *fp;
    fp = fopen(drives[ctr],"w");
    fprintf(fp,"%d\t%s",regAccNum,regPin);
    fclose(fp);
}

void delay(int ms){
    clock_t timeDelay = ms + clock();
    while(timeDelay > clock());
}

void regInterface(){
    printToxy(48,2,"R E G I S T R A T I O N");
    UI(30, 4, 90, "Full Name:");
    UI(30, 8, 45, "No.:");
    UI(47, 8, 90, "Initial Deposit: Php");
    UI(30, 12, 90, "Enter PIN:");
    UI(30, 16, 90, "Confirm PIN:");
    UI(30, 20, 90, " ");
    printToxy(57,22,"- - -");
}

void gotoxy(int x, int y){
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}

void printToxy(int x, int y, char* label){
    gotoxy(x,y);
    printf("%s",label);
}

void UI(int x, int y, int len, char *label){
    int i;
    for(i = x; i < len; i++){
        printToxy(i,y,"~");
        printToxy(i,y + 3,"~");
    }
    for(i = y + 1; i < y + 3; i++){
        printToxy(x - 1,i,":");
        printToxy(len,i,":");
    }
    printToxy(x + 1, y + 2,label);
}

void clear(char *label){
    if(strcmp(label,"result") == 0){
        printToxy(32,22,"                                                       ");
        printToxy(57,22,"- - -");
    }
    else if(strcmp(label,"pinBoxes") == 0){
        printToxy(55,18,"                 ");
        printToxy(55, 14,"                 ");
    }
}

void readFd(int error){
    int i = 0, x, y, counter;
    if(error == 1){
        system("color 04");
        for(int i = 11; i < 19; i++)
            printToxy(44,i,"                                ");
        for(int i = 44; i < 76; i++){
            printToxy(i,10,"_");
            printToxy(i, 18,"_");
        }
        for(int i = 11; i < 19; i++){
            printToxy(43,i,"|");
            printToxy(76,i,"|");
        }
        x = 49;
        y = 11;
        counter = 4;
    }
    else if(error == 0){
        x = 46;
        y = 12;
        counter = 2;
        printToxy(29,21,"[  D O   N O T   R E M O V E   W H E N   I N   U S E  ]");
        printToxy(27,23,"[  A C C O U N T   W I L L   B E   I N V A L I D A T E D  ]");
    }
}
