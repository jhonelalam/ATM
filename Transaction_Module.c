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


char inputPin[7];
char regPin[7];
char confirmPin[7];
char cardPin[7];
char* flashDrive[2] = {"D:\\checkingFlashDrive.dbf", "G:\\checkingFlashDrive.dbf"};
char* drives[2] = {"D:\\Acc.dbf","G:\\Acc.dbf"};
int accountNoFromCard, regAccountNo, regAccBal, accInput, otherAccIndex, ctr = -1, exist = 0, checkName = 0, checkPin = 0, charPin = 0;

void makenull();
void retrieve();
int insertCard();
int Fd();
void pinEnter();
int transaction();
int isEmpty();
int isFull();
void insert();
int locate();
void save();
void saving();
void checkingPin();
int confirm(int x, int y, int len);

void delay(int ms);
void transactionUi();
void pinUi();
void gotoxy(int x, int y);
void printToxy(int x, int y, char* label);
COORD coord = {0,0};
void UI(int x, int y, int len, char *label);
void clear(char *label);
void removeFd();

int main()
{
    int i;
    system("color E0");
    makenull();
    retrieve();
    if(insertCard()){
        pinUi();
        pinEnter();
        accInput = locate(1);
        do{
            _getch();
            system("cls");
            system("color E0");
            transactionUi();
        }while(transaction());
    }
    i = remove(flashDrive[ctr]);
    gotoxy(0,25);
    _getch();
    return 0;
}

void makenull(){
    l.last = -1;
}

void retrieve(){
    FILE *fp;
    int i = 0;
    fp = fopen("AtmBank.dbf","r+");

    if (fp==NULL){
        UI(30, 10, 90, " ");
        printf("D A T A B A S E   I S   E M P T Y");
        delay(600);
        printToxy(32,12,"                                                       ");
        printToxy(42,12," R E G I S T E R  F I R S T . . .");
        _getch();
        gotoxy(1,30);
        exit(0);
    }
    else{
        while(!feof(fp)){
            l.last++;
            fscanf(fp,"%d %[^\t]%d %[^\n]",&acc[l.last].accNum,acc[l.last].accName,&acc[l.last].balance,acc[l.last].accPin);
        }
    }
    fclose(fp);
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
        return 0;
    }
    else{
        while(!feof(fp))
            fscanf(fp,"%d %[^\n]",&accountNoFromCard,cardPin);

        for(int i = 0; i < l.last + 1; i++){
            if(strcmp(acc[i].accPin,cardPin) == 0 && acc[i].accNum == accountNoFromCard){
                printToxy(46,9,"R E G I S T E R E D");
                return 1;
            }
        }
        printToxy(42,9,"N O T   R E G I S T E R E D .");
        return 0;
    }
    fclose(fp);
}

int Fd(){
    FILE *fp;
    fp = fopen(flashDrive[ctr],"r");
    if(fp == NULL){
        clear("result2");
        printToxy(43,22,"F L A S H   D R I V E   E R R O R");
        removeFd();
        gotoxy(0,25);
        return 1;
    }
    return 0;
    fclose(fp);
}

void pinEnter(){
    int i = 0;
    do{
        if(i == 3){
            printToxy(46,17," E X I T I N G . . .");
            delay(2000);
            gotoxy(0,20);
            exit(0);
        }
        gotoxy(52, 13);
        for(int i = 0; i < 6; i++){
            inputPin[i] = _getch() + 9;
            putchar('*');
            putchar(' ');
        }
        inputPin[6] = '\0';

        if(strcmp(inputPin,cardPin) != 0){
            printToxy(44,17,"I N V A L I D   P I N");
            _getch();
            clear("result");
            clear("pinBoxes");
        }
        i++;
    }while(strcmp(inputPin,cardPin) != 0);
    printToxy(46,17,"W E L C O M E ! ! !");
}

int locate(int x){

    if(x == 0){
        for(int i = 0; i < l.last + 1; i++){
            if(strcmp(acc[i].accPin,regPin) == 0){
                checkPin = 1;
                return 1;
            }
        }
        return 0;
    }
    else if(x == 1){
        for(int i = 0; i < l.last + 1; i++){
            if(acc[i].accNum == accountNoFromCard)
                return i;
        }
        return 0;
   }
   else if(x == 2){
        for(int i = 0; i < l.last + 1; i++){
            if(acc[i].accNum == regAccountNo){
                otherAccIndex = i;
                return 0;
            }
        }
        return 1;
   }
}

int transaction(){
    char choice;
    int end;
    gotoxy(41,12);
    scanf(" %c",&choice);
    if(choice == '1'){
        if(acc[accInput].balance != 0){
            do{
                clear("result2");
                clear("enter");
                printToxy(36,22,"E N T E R   A M O U N T   T O   W I T H D R A W");
                printToxy(31,18,"Enter Amount: Php  ");
                scanf("%d",&regAccBal);
                if(regAccBal > acc[accInput].balance){
                    printToxy(33,22,"A M O U N T   G R E A T E R   T H A N   B A L A N C E");
                    _getch();
                }
                else if(regAccBal < 0){
                    clear("result2");
                    printToxy(45,22,"N E G A T I V E   E N T R Y");
                    _getch();
                }
                else if(regAccBal == 0){
                    clear("result2");
                    printToxy(45,22,"  I N V A L I D   E N T R Y");
                    _getch();
                }
            }while(regAccBal > acc[accInput].balance || regAccBal < 0 || regAccBal == 0);
            if(confirm(44,11,76)){
                if(Fd()){
                    exit(0);
                }
                acc[accInput].balance -= regAccBal;
                system("color 02");
                printToxy(50,22,"S U C C E S S ! ! !");
            }
            else
                printToxy(50,22,"C A N C E L L E D");
        }
        else
            printToxy(50,22,"N O   B A L A N C E");
    }
    else if(choice == '2'){
        do{
            clear("result2");
            clear("enter");
            printToxy(37,22,"E N T E R   A M O U N T   T O   D E P O S I T");
            printToxy(31,18,"Enter Amount: Php  ");
            scanf("%d",&regAccBal);
            if(regAccBal < 0){
                clear("result2");
                printToxy(45,22,"N E G A T I V E   E N T R Y");
                _getch();
            }
            else if(regAccBal == 0){
                clear("result2");
                printToxy(45,22,"  I N V A L I D   E N T R Y");
                _getch();
            }
        }while(regAccBal < 0 || regAccBal == 0);
        if(confirm(44,11,76)){
                if(Fd()){
                    exit(0);
                }
                system("color 02");
                acc[accInput].balance += regAccBal;
                printToxy(50,22,"S U C C E S S ! ! !");
            }
        else
            printToxy(50,22,"C A N C E L L E D");
    }
    else if(choice == '3'){
        if(Fd()){
            exit(0);
        }
        printToxy(48,22,"Y O U R   B A L A N C E");
        gotoxy(31,18);
        printf("Balance: Php  %d", acc[accInput].balance);
    }

    else if(choice == '4'){
        if(l.last > 0){
            if(acc[accInput].balance != 0){
                do{
                    otherAccIndex = -1;
                    clear("result2");
                    clear("enter");
                    printToxy(47,22,"F U N D   T R A N S F E R");
                    printToxy(31,18,"Enter Account No. you want to Transfer:  ");
                    scanf("%d",&regAccountNo);
                    if(locate(2)){
                        clear("result2");
                        printToxy(34,22,"A C C O U N T   N U M B E R   D O N \' T   E X I S T");
                        _getch();
                    }
                    else if(acc[accInput].accNum == acc[otherAccIndex].accNum){
                        otherAccIndex = -1;
                        clear("result2");
                        printToxy(37,22,"I N V A L I D   A C C O U N T   N U M B E R");
                        _getch();
                    }
                    else{
                        do{
                            clear("result2");
                            clear("enter");
                            printToxy(36,22,"E N T E R   A M O U N T   T O   T R A N S F E R");
                            printToxy(31,18,"Enter Amount: Php  ");
                            scanf("%d",&regAccBal);
                            if(regAccBal > acc[accInput].balance){
                                clear("result2");
                                printToxy(33,22,"A M O U N T   G R E A T E R   T H A N   B A L A N C E");
                                _getch();
                            }
                            else if(regAccBal < 0){
                                clear("result2");
                                printToxy(45,22,"N E G A T I V E   E N T R Y");
                                _getch();
                            }
                            else if(regAccBal == 0){
                                clear("result2");
                                printToxy(45,22,"  I N V A L I D   E N T R Y");
                                _getch();
                            }
                        }while(regAccBal < 0 || regAccBal > acc[accInput].balance || regAccBal == 0);
                    }
                }while(otherAccIndex == -1);
                if(confirm(44,11,76)){
                    if(Fd()){
                        exit(0);
                    }
                    system("color 02");
                    acc[accInput].balance -= regAccBal;
                    acc[otherAccIndex].balance += regAccBal;
                    printToxy(50,22,"S U C C E S S ! ! !");
                }
                else
                    printToxy(50,22,"C A N C E L L E D");
            }
            else
                printToxy(50,22,"N O   B A L A N C E");
        }
        else if(l.last == 0)
            printToxy(32,22,"N O   O T H E R   A C C O U N T   T O   T R A N S F E R");
    }

    else if(choice == '5'){
        do{
            if(Fd()){
                exit(0);
            }
            checkingPin();
            do{
                clear("result2");
                clear("enter");
                printToxy(50,22,"C H A N G E   P I N");
                printToxy(31,18,"Enter Current PIN:  ");

                for(int i = 0; i < 6; i++){
                    inputPin[i] = _getch() + 9;
                    putchar('*');
                    putchar(' ');
                }
                if(strcmp(inputPin,cardPin) != 0){
                    clear("result2");
                    printToxy(48,22,"I N V A L I D   P I N");
                    _getch();
                }
            }while(strcmp(inputPin,cardPin) != 0);

            clear("result2");
            clear("enter");
            printToxy(31,18,"Enter new PIN:  ");
            for(int i = 0; i < 6; i++){
                regPin[i] = _getch() + 9;
                putchar('*');
                putchar(' ');
            }
            regPin[6] = '\0';

            clear("result2");
            clear("enter");
            printToxy(31,18,"Confirm new PIN:  ");
            for(int i = 0; i < 6; i++){
                confirmPin[i] = _getch() + 9;
                putchar('*');
                putchar(' ');
            }
            confirmPin[6] = '\0';

            end = strcmp(confirmPin,regPin);

            checkPin = 0;
            charPin = 0;

            for(int i = 0; i < 6; i++){
                if(isdigit(confirmPin[i] - 9)  == 0){
                    charPin = 1;
                    break;
                }
            }

            if(end != 0){
                clear("result2");
                printToxy(42,22,"P I N   D I D   N O T   M A T C H !");
                _getch();
            }

            else if(locate(0) && checkPin == 1){
                clear("result2");
                printToxy(46,22,"P I N   N O T   U N I Q U E");
                _getch();
            }

            else if(charPin == 1){
                clear("result2");
                printToxy(36,22,"P I N   C O N T A I N S   A   C H A R A C T E R");
                _getch();
            }

        }while(end != 0 || checkPin || charPin);
        if(Fd()){
            exit(0);
        }
        system("color 02");
        strcpy(acc[accInput].accPin,regPin);
        clear("result2");
        printToxy(50,22,"S U C C E S S ! ! !");
        saving();
    }

    else if(choice == '6'){
        printToxy(39,22,"T H A N K   Y O U   F O R   U S I N G ! ! !");
        return 0;
    }
    else
        printToxy(45,22,"I N V A L I D   C H O I C E");
    save();
    return 1;
}

void checkingPin(){
    FILE* fp;
    fp = fopen(drives[ctr],"r");
    while(!feof(fp))
            fscanf(fp,"%d %[^\n]",&accountNoFromCard,cardPin);
}

int confirm(int x, int y, int len){
    char input;
    for(int i = y; i < y + 8; i++)
        printToxy(x,i,"                                ");
    for(int i = x; i < len; i++){
        printToxy(i,y - 1,"_");
        printToxy(i,y + 2,"_");
        printToxy(i, y + 7,"_");
    }
    for(int i = y; i < y + 8; i++){
        printToxy(x - 1,i,"|");
        printToxy(len,i,"|");
    }
    printToxy(x + 6,y + 1,"CONFIRM TRANSACTION?");
    printToxy(x + 5,y + 5,"[Y] YES       [N] NO");
    do{
        input = _getch();
        if(input == 'Y' || input == 'y' || input == 'N' || input == 'n'){
            system("cls");
            transactionUi();
            if(input == 'Y' || input == 'y')
                return 1;
            else if(input == 'N' || input == 'n')
                return 0;
        }
    }while(input != 'Y' || input != 'y' || input != 'N' || input != 'n');
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
    fprintf(fp,"%d\t%s",acc[accInput].accNum,regPin);
    fclose(fp);
}

void delay(int ms){
    clock_t timeDelay = ms + clock();
    while(timeDelay > clock());
}

void pinUi(){
    UI(25, 11, 39, "PIN:");
    UI(41, 11, 86, "");
    UI(25, 15, 86, "");
    printToxy(53,17,"- - -");
}

void transactionUi(){
    printToxy(48,2," T R A N S A C T I O N");
    UI(30, 4, 75, "Full Name:");
    UI(77, 4, 90, "No.:");
    gotoxy(42, 6);
    printf("%s",acc[accInput].accName);
    gotoxy(83, 6);
    printf("%d",acc[accInput].accNum);
    printToxy(31, 12,"Option: [   ]");
    for(int i = 30; i < 90; i++){
        printToxy(i,8,"_");
        printToxy(i,15,"_");
    }
    for(int i = 9; i < 16; i++){
        printToxy(29,i,"|");
        printToxy(46,i,"|");
        printToxy(90,i,"|");
    }
    printToxy(48,10,"[1] Withdraw          [4] Fund Transfer");
    printToxy(48,12,"[2] Deposit           [5] Change PIN");
    printToxy(48,14,"[3] Balance Inquiry   [6] Exit");
    UI(30, 16, 90, "");
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
        printToxy(25,17,"                                                       ");
        printToxy(53,17,"- - -");
    }
    else if(strcmp(label,"result2") == 0){
        printToxy(32,22,"                                                       ");
        printToxy(57,22,"- - -");
    }
    else if(strcmp(label,"enter") == 0)
        printToxy(31,18,"                                                       ");
    else if(strcmp(label,"pinBoxes") == 0)
        printToxy(52, 13,"                 ");
}

void removeFd(){
    int i = 0;
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
}
