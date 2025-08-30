#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char peek(FILE *input_file);
int hash(const char *str);
int lexicalAnalyze();
char* char_to_str(char);


int main()
{
    lexicalAnalyze();
    int loopCount=0;
    long loopStartPositionInFile=0;
    int codeBlock = 0;
    FILE *input_file;
    char character;
    char* str=NULL;
    char* temp=NULL;
    int seperatordetectstr=0;
    int seperatordetectint=0;

    input_file = fopen("code.sta.txt","r");
    char stringmap[256][256]; // her bir indexinde string tutan liste, burada tanımlanan değişkenleri hashleyip saklayacağım
    // değişkenin değeri integer olsa bile string olarak saklayıp string olarak yazacağım, gerekirse inte çevireceğim.

    char integermap[256][10];

    if (input_file==NULL){
        printf("Dosya açılamadı veya bulunamadı");
        exit(1);
    }
    character = fgetc(input_file);
    while (character!=EOF)
    {
        if (loopCount>0) // loop
        {
            if (codeBlock)
            {
                if (character == '}')
                {
                    loopCount--;
                    fseek(input_file, loopStartPositionInFile-1,SEEK_SET);
                    character= fgetc(input_file);
                }
                if (loopCount==1)
                {
                    loopCount=0;
                    codeBlock=0;
                }
            }
            else {
                loopCount--;
                fseek(input_file, loopStartPositionInFile-1,SEEK_SET);
                character= fgetc(input_file);
            }
        }
        else if (character == '}') {
            character= fgetc(input_file);
        }
        if (character == '.')
            character = fgetc(input_file);
        else if (character==',') {
            character= fgetc(input_file);
        }
        else if (character == ' ' || character == '\n') {
            character = fgetc(input_file); // eğer boşluk okumuşsam veya yeni satır karakteriyse sonrakine geçebilirim
        }
        else if (character =='/' && peek(input_file)=='*') {
            while (character!='*' && peek(input_file)!='/')
            {
                character = fgetc(input_file);
            }
            character= fgetc(input_file);
        }
        else if (isalpha(character) || character == '_')
        {
            str = (char *) malloc(11); // maximum identifier size 11 olmalı
            int i = 0;
            while (i < 11 || character != EOF) {
                str[i] = character;
                i++;
                character = fgetc(input_file);
                if ((!isalnum(character) && character != '_') || isspace(character)) {
                    str[i] = '\0';
                    break;
                }
            }
            if (strcmp(str, "text") == 0 || seperatordetectstr || stringmap[hash(str)][0]!='\000' ) // text
            {
                seperatordetectstr=0;
                int index = hash(str);
                free(str);
                while (isspace(character)) // eğer ki identifier name _ ile başlıyorsa diye
                {
                    character = fgetc(input_file);
                }
                str = (char *) malloc(11);
                int x = 0;
                while (x < 11 || character != EOF)
                {
                    if (character=='.') // ,identifier. şeklinde bitmişse
                    {
                        strcpy(stringmap[index],"empty");
                        free(str);
                        break;
                    }
                    else if (character==',') {
                        strcpy(stringmap[index],"empty");
                        free(str);
                        seperatordetectstr=1;
                        break;
                    }
                    str[x] = character;
                    x++;
                    character = fgetc(input_file);
                    if (character == '.') // idenditifer tanımlanmış ama değer atanmamış
                    {
                        str[x] = '\0';
                        index = hash(str);
                        strcpy(stringmap[index], "empty"); // değer atanmadığı için empty
                        break;
                    }
                    if (isspace(character)) // eğer endofline gelmemişse değeri de atayacak demektir, o yüzden
                        // string atayacağından " gelene dek okuma yapıyorum
                    {
                        char* string = (char*) malloc(256); // - den sonra gelen stringi tutacak
                        int removeFrom=0; // - den önce gelen değişkenin indexini tutacak
                        while (character != '"') {
                            character = fgetc(input_file);
                            if (isalpha(character))
                            {
                                temp=(char*) malloc(256);
                                int b=0;
                                while (isalpha(character))
                                {
                                    temp[b]=character;
                                    b++;
                                    character = fgetc(input_file);
                                }
                                temp[b]='\0';
                                if (strcmp(temp,"is")==0) {
                                    free(temp);
                                }
                                else if (character=='-')
                                {
                                    removeFrom=hash(temp);
                                    free(temp);
                                }
                            }
                        }
                        str[x] = '\0';
                        if (strcmp(str,"is")==0) {
                            free(str);
                        }
                        else {
                            index = hash(str);
                            free(str);
                        }

                        str = (char *) malloc(256);
                        int k = 0;
                        character = fgetc(input_file);
                        while (k < 256 || character != EOF)
                        {
                            str[k] = character;
                            k++;
                            character = fgetc(input_file);
                            if (character == '"')
                            {
                                if (peek(input_file)!='.' && peek(input_file)!=',') // string concet
                                {
                                    while (character!='+') {
                                        character= fgetc(input_file);
                                    }
                                    while (!isalpha(character)) {
                                        character= fgetc(input_file);
                                    }
                                    temp = (char*) malloc(11);
                                    int p=0;
                                    while (isalpha(character) || character=='_')
                                    {
                                        temp[p]=character;
                                        p++;
                                        character= fgetc(input_file);
                                    }
                                    temp[p]='\0';
                                    int index2= hash(temp);
                                    str[k]='\0';
                                    char result[256];
                                    strcpy(result,str);
                                    strcat(result,stringmap[index2]);
                                    free(temp);
                                    free(str);
                                    strcpy(stringmap[index], result);
                                    character = fgetc(input_file);
                                    if (character==',') {
                                        seperatordetectstr=1;
                                    }
                                    break;
                                }
                                str[k] = '\0';
                                if (removeFrom)
                                {
                                    string = stringmap[removeFrom]; // değişkenden str değerini çıkartacağız.
                                    char* pos = strstr(string,str);
                                    size_t lenstr = strlen(str);
                                    memmove(pos,pos+lenstr,strlen(pos+lenstr)+1);
                                    free(str);
                                    strcpy(stringmap[index],pos);
                                    character= fgetc(input_file);
                                    break;
                                }
                                strcpy(stringmap[index], str);
                                free(str);
                                character = fgetc(input_file);
                                if (character==',') {
                                    seperatordetectstr=1;
                                }
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            else if (strcmp(str, "int") == 0 || atoi(integermap[hash(str)])>0 || seperatordetectint ||
            strcmp(integermap[hash(str)],"0")==0)
            {
                int index = hash(str);
                seperatordetectint=0;
                free(str);
                while (isspace(character) || character == '_') {
                    character = fgetc(input_file);
                }
                str = (char *) malloc(11);
                int x = 0;
                while (x < 11 || character != EOF)
                {
                    if (character=='.') {
                        strcpy(integermap[index],"0");
                        free(str);
                        break;
                    }
                    else if (character==',') {
                        strcpy(stringmap[index],"0");
                        free(str);
                        seperatordetectint=1;
                        break;
                    }
                    str[x] = character;
                    x++;
                    character = fgetc(input_file);
                    if (character == '.') // idenditifer tanımlanmış ama değer atanmamış
                    {
                        str[x] = '\0';
                        int index = hash(str);
                        strcpy(integermap[index], "0"); // değer atanmadığı için 0
                        break;
                    }
                    if (isspace(character)) {
                        str[x] = '\0';
                        if (strcmp(str,"is")==0)
                        {
                            free(str);
                        }
                        else if (strcmp(str,"/*")==0) {
                            while (character!='*' && peek(input_file)!='/') {
                                character= fgetc(input_file);
                            }
                            character= fgetc(input_file);
                            free(str);
                        }
                        else {
                            index = hash(str);
                        }
                        while (!isdigit(character)) {
                            character = fgetc(input_file);
                            if (character == '/' && peek(input_file) == '*')
                            {
                                character= fgetc(input_file);
                                character= fgetc(input_file);
                                while (character != '*' && peek(input_file) != '/') {
                                    character = fgetc(input_file);
                                }
                                character = fgetc(input_file);
                            }
                            else if (isalpha(character) || character == '_' || isdigit(character))
                            {
                                if (character=='_' || isalpha(character)) {
                                    str = malloc(11);
                                    int j = 0;
                                    while (isalpha(character) || character == '_') {
                                        str[j] = character;
                                        j++;
                                        character = fgetc(input_file);
                                    }
                                    str[j] = '\0';
                                    if (strcmp(str, "is") == 0) {
                                        free(str);
                                        continue;
                                    }
                                }
                                else if (isdigit(character))
                                {
                                    str= malloc(8);
                                    int j=0;
                                    while (isdigit(character)) {
                                        str[j]=character;
                                        j++;
                                        character= fgetc(input_file);
                                    }
                                    str[j]='\0';
                                }
                                int value;
                                if (isdigit(str[0]))
                                {
                                    value = atoi(str);
                                }
                                else {
                                    value = atoi(integermap[hash(str)]);
                                }
                                while (isspace(character)) {
                                    character = fgetc(input_file); // boşlukları atla
                                }
                                if (character=='.')
                                {
                                    strcpy(integermap[index],str);
                                    free(str);
                                    break;
                                }
                                if (character == '+') {
                                    temp = (char *) malloc(8);
                                    character = fgetc(input_file);
                                    int y = 0;
                                    while (character != '.') {
                                        temp[y] = character;
                                        character = fgetc(input_file);
                                        y++;
                                    }
                                    int value_2;
                                    temp[y] = '\0';
                                    if (isdigit(temp[0])) // 2.operand da sayı
                                    {
                                        value_2 = atoi(temp);
                                    } else {              //2. operand da identifier
                                        value_2 = atoi(integermap[hash(temp)]);
                                    }
                                    char strvalue[8];
                                    strcpy(integermap[index], itoa(value + value_2, strvalue, 10));
                                    free(temp);
                                } else if (character == '/')
                                {
                                    character= fgetc(input_file);
                                    temp = (char *) malloc(8);
                                    int y = 0;
                                    while (character != '.') {
                                        temp[y] = character;
                                        character = fgetc(input_file);
                                        y++;
                                    }
                                    int value_2;
                                    temp[y] = '\0';
                                    if (isdigit(temp[0])) // 2.operand da sayı
                                    {
                                        value_2 = atoi(temp);
                                    } else {              //2. operand da identifier
                                        value_2 = atoi(integermap[hash(temp)]);
                                    }
                                    char strvalue[8];
                                    value = value/value_2;
                                    strcpy(integermap[index], itoa(value, strvalue, 10));
                                    free(temp);
                                } else if (character == '*')
                                {
                                    character = fgetc(input_file);
                                    temp = (char *) malloc(8);
                                    int y = 0;
                                    while (character != '.') {
                                        temp[y] = character;
                                        character = fgetc(input_file);
                                        y++;
                                    }
                                    int value_2;
                                    temp[y] = '\0';
                                    if (isdigit(temp[0])) // 2.operand da sayı
                                    {
                                        value_2 = atoi(temp);
                                    } else {              //2. operand da identifier
                                        value_2 = atoi(integermap[hash(temp)]);
                                    }
                                    char strvalue[8];
                                    strcpy(integermap[index], itoa(value * value_2, strvalue, 10));
                                    free(temp);
                                } else {
                                    character = fgetc(input_file);
                                    temp = (char *) malloc(8);
                                    int y = 0;
                                    while (character != '.') {
                                        temp[y] = character;
                                        character = fgetc(input_file);
                                        y++;
                                    }
                                    int value_2;
                                    temp[y] = '\0';
                                    if (isdigit(temp[0])) // 2.operand da sayı
                                    {
                                        value_2 = atoi(temp);
                                    } else {              //2. operand da identifier
                                        value_2 = atoi(integermap[hash(temp)]);
                                    }
                                    char strvalue[8];
                                    if (value-value_2<0) {
                                        strcpy(integermap[index], itoa(0, strvalue, 10));
                                    }
                                    else {
                                        strcpy(integermap[index], itoa(value - value_2, strvalue, 10));
                                    }
                                    free(temp);
                                }
                                break;
                            }
                        }
                        if (isdigit(character)) {
                            str = (char *) malloc(9);
                            int k = 0;
                            while (k < 9 || character != EOF) {
                                str[k] = character;
                                k++;
                                character = fgetc(input_file);
                                if (character == '.' || character==',') // sayıdan
                                {
                                    str[k] = '\0';
                                    strcpy(integermap[index], str);
                                    free(str);
                                    if (character==',') {
                                        seperatordetectint=1;
                                    }
                                    break;
                                }
                            }
                        }
                        break;
                    }
                }
            }
            else if (strcmp(str, "loop") == 0)
            {
                free(str);
                while (!isdigit(character) && character != '_' && !isalpha(character)) {
                    character= fgetc(input_file);
                }
                int x =0;
                str = (char*)malloc(8);
                while (!isspace(character)){
                    str[x]=character;
                    x++;
                    character= fgetc(input_file);
                }
                str[x]='\0';
                if (isdigit(str[0])) {
                    loopCount= atoi(str);
                }
                else {
                    loopCount= atoi(integermap[hash(str)]);
                }
                free(str);
                str = (char*) malloc(256);
                while (!isalpha(character) && character!='_') // loopun içinin başladığı yere gel
                {
                    character = fgetc(input_file);
                    if (character=='t') // loopun başladığı yere gitmek için times kelimesinden geçmem lazım
                    {
                        for (int p=0; p<5; p++) {
                            character= fgetc(input_file);
                        }
                    }
                    if (character=='{')
                    {
                        codeBlock=1;
                        character = fgetc(input_file);
                    }
                }
                loopStartPositionInFile = ftell(input_file);

            }
            else if (strcmp(str,"read")==0)
            {
                free(str);
                while (character!='"')
                {
                    character= fgetc(input_file);
                    if (isalpha(character)) {
                        break;
                    }
                }
                if (isalpha(character)) // prompt yok demek
                {
                    int k=0;
                    str = (char*) malloc(10);
                    while (character!='.')
                    {
                        str[k]=character;
                        k++;
                        character= fgetc(input_file);
                    }
                    str[k]='\0';
                    int index = hash(str);
                    free(str);
                    str = (char*) malloc(256);
                    fgets(str, 256, stdin);
                    if (isdigit(str[0]))
                    {
                        if (strlen(str)>8)
                        {
                            printf("%s\n","Integer size error"); // 8 basamaktan büyükse integer size error
                            exit(1);
                        }
                        strcpy(integermap[index],str);
                    }
                    else if (str[0]=='-') {
                        strcpy(integermap[index],str);
                        strcpy(str,"0\0"); // 0 dan küçük input almışsak 0 a eşitliyorum
                    }
                    else
                    {
                        strcpy(stringmap[index],str);
                    }
                    free(str);
                }
                else if (character=='"')
                {
                    str = (char*) malloc(256);
                    character= fgetc(input_file);
                    int k =0;
                    while (character!='"')
                    {
                        str[k]=character;
                        k++;
                        character= fgetc(input_file);
                    }
                    str[k]='\0';
                    printf("%s\t:",str);
                    free(str);
                    while (!isalpha(character) && character!='_') {
                        character= fgetc(input_file);
                    }
                    k=0;
                    str = (char*) malloc(10);
                    while (character!='.')
                    {
                        str[k]=character;
                        k++;
                        character= fgetc(input_file);
                    }
                    str[k]='\0';
                    int index = hash(str);
                    free(str);
                    str = (char*) malloc(256);
                    fgets(str, 256, stdin);
                    if (isdigit(str[0]))
                    {
                        if (strlen(str)>8)
                        {
                            printf("%s\n","Integer size error"); // 8 basamaktan büyükse integer size error
                            exit(1);
                        }
                        strcpy(integermap[index],str);
                    }
                    else if (str[0]=='-')
                    {
                        strcpy(str,"0\0"); // 0 dan küçük input almışsak 0 a eşitliyorum
                        strcpy(integermap[index],str);
                    }
                    else {
                        strcpy(stringmap[index],str);
                    }
                    free(str);
                }
            }
            else if (strcmp(str,"write")==0)
            {
                free(str);
                while (character!='.')
                {
                    character= fgetc(input_file);
                    if (character=='"')
                    {
                        int k=0;
                        character = fgetc(input_file);
                        str = (char*) malloc(256);
                        while (character!='"')
                        {
                            str[k]=character;
                            k++;
                            character= fgetc(input_file);
                        }
                        str[k]='\0';
                        printf("%s",str);
                        free(str);
                    }
                    else if (isalpha(character) || character=='_')
                    {
                        int k=0;
                        str=(char*) malloc(10);
                        while (!isspace(character)  && character!=',' && character!='.')
                        {
                            str[k]=character;
                            k++;
                            character = fgetc(input_file);
                        }
                        str[k]='\0';
                        int index = hash(str);
                        free(str);
                        printf("%s",stringmap[index]);
                        printf("%s",integermap[index]);
                    }
                }
            }
            else if (strcmp(str,"newLine")==0) {
                printf("%s","\n");
            }
        }
    }
    return 0;
}

int lexicalAnalyze() {
    char character;
    char* str = NULL;
    int const INT_SIZE = 8;
    int const STRING_SIZE = 256+1;
    int const IDENTIFIER_SIZE=10+1;

    FILE *input_file ,*output_file;
    input_file = fopen("code.sta.txt","r");
    output_file = fopen("code.lex.txt","w");

    if (input_file==NULL || output_file==NULL) {
        printf("Dosya açılamadı veya bulunamadı");
        exit(1);
    }

    character = fgetc(input_file);


    // döngüde bir sonraki karaktere geçmek, hangi if'in içine girdiyse oranın sorumluluğunda
    while (character!= EOF) {
        if (character==' ' ||character=='\n') {
            character= fgetc(input_file); // eğer boşluk okumuşsam veya yeni satır karakteriyse sonrakine geçebilirim
        }
            // sayı kontrol
        else if (isdigit(character) && (isspace(peek(input_file)) || isdigit(peek(input_file)) || peek(input_file)=='.'))
            // eğer sayı + boşluk veya sayı + sayı.... varsa elimdekinin bir integer olduğunu anlıyorum
            // star dilinde maximum sayı 8 basamaklı olabileceğinden 9luk bir yer ayırıyorum
        {
            str = (char*) malloc(INT_SIZE);
            int i =0;
            str[i]=character;
            character= fgetc(input_file);
            while (i<INT_SIZE || character!=EOF)
            {
                i++;
                if (character=='.') {  // ende ulaştık, sonu ekle ve bitir
                    str[i]='\0';
                    break;
                }
                if (!isdigit(character) && !isspace(character))  // sonraki karakter digit, değil, boşluk da değil
                {                                                      // sonu ekle ve bitir
                    str[i]='\0';
                    break;
                }
                if (isspace(character)) {
                    str[i]='\0';
                    character= fgetc(input_file); // karakter boşluk olduğundan sonrakine geçebilirim
                    break;
                }
                if (isdigit(character)){
                    str[i]=character;
                    character= fgetc(input_file);
                }
            }
            if (i>=INT_SIZE ||character==EOF){
                printf("INTEGER SIZE ERROR");
                exit(1);
            }
            fprintf(output_file,"IntConst()-> ");
            fprintf(output_file,"%s\n",str);
            free(str);


        }
            // - sayı kontrol
        else if (character=='-')
        {
            if (!isdigit(peek(input_file)))
            {
                fprintf(output_file,"Operator(-)\n");
                character = fgetc(input_file);
            }
            else if (isdigit(peek(input_file))) {
                while (isdigit(peek(input_file))) { // sayının sonuna dek oku, sonuna geldiğinde dur
                    fgetc(input_file);
                }
                character = fgetc(input_file); // döngü için yeni karaktere geçtim
                fprintf(output_file,"IntConst(0)\n");
            }
        }
        else if (isdigit(character) && isalpha(peek(input_file))) { // sayıyla başlayan bir identifier, error case
            printf("Identifier type error");
            exit(1);
        }
        else if (character=='.')
        {
            fprintf(output_file,"EndOfLine\n");
            character= fgetc(input_file);   // döngü için yeni karaktere geçtim
        }
        else if (character==',') {
            fprintf(output_file,"Seperator()-> ,\n");
            character= fgetc(input_file); // döngü için yeni karaktere geçtim
        }
            // yorum kontrol
        else if (character=='/' && peek(input_file)=='*')
        {
            fgetc(input_file);
            character= fgetc(input_file); //* dan sonraki ilk kelimeyi okuyorum
            while (!(character=='*' && peek(input_file)=='/'))
            {
                character= fgetc(input_file);
                if (character==EOF) {
                    printf("Comment error");
                    exit(1);
                }
            }
            fgetc(input_file); // /'ı okudum
            character= fgetc(input_file); // döngü için yeni karaktere geçtim
        }
        else if (character=='/' || character=='+' || character=='*')
        {
            char* temp = char_to_str(character);
            fprintf(output_file,"Operator()->");
            fprintf(output_file,"%s\n",temp);
            character= fgetc(input_file);
        }
        else if (character=='{' || character == '}') {
            if (character == '{') {
                fprintf(output_file,"LeftCurlyBracket\n");

            }
            else {
                fprintf(output_file,"RightCurlyBracket\n");

            }
            character = fgetc(input_file); // döngü için sonraki karaktere geçtim
        }
        else if (character=='"')
        {
            str = (char*)malloc(STRING_SIZE);
            int a = 0;
            character= fgetc(input_file);
            while (character!=EOF)
            {
                str[a]=character;
                a++;
                if (a>=STRING_SIZE) {
                    printf("String size error");
                    exit(1);
                }
                if (peek(input_file)=='"')
                {
                    str[a]='\0';
                    fgetc(input_file);
                    // " 'ı okudum
                    if (isalnum(peek(input_file)) || peek(input_file)=='"')// eğerki " kapandıktan sonra alfabetik veya numerik bir şey gelmişse
                        // hemen ardından, bir sorun var demektir
                    {
                        printf("String format error");
                        exit(1);
                    }
                    char* temp = str;
                    fprintf(output_file,"StringConstant()-> ");
                    fprintf(output_file,"%s\n",temp);
                    free(str);
                    character = fgetc(input_file);// döngü için sonraki karaktere geçtik.
                    break;
                }
                character = fgetc(input_file);
            }
        }
        else if (isalpha(character) || character=='_') // eğer karakter alfabetik bir şeyle başlıyorsa ya identifier ya da
            // keyword, bu durumda bir sonraki okuduğum kelime alfabetik numerik veya '_' olmayıncaya dek okuyacağım
            // okuduğum kelime eğer keywordlerden birisine eşitse keyword olarak yazdıracağım, eğer ki keywordlerden birine
            // eşit değil ve uzunluğu da 10'dan küçük eşitse identifier olarak yazdıracağım
        {
            str = (char*)malloc(IDENTIFIER_SIZE); //keyword listesini bildiğimden en uzun keywordun bile 10'dan kısa olduğunu biliyorum
            int i = 0;
            while (i<IDENTIFIER_SIZE || character != EOF)
            {
                str[i]=character;
                i++;
                character = fgetc(input_file);

                if (!isalnum(character) && character!='_' && i<=IDENTIFIER_SIZE)
                {
                    if (strcmp(str,"int") ==1 || strcmp(str,"text") == 1 || strcmp(str,"is")==1 || strcmp(str,"loop") ==1 ||
                        strcmp(str,"times") == 1 || strcmp(str,"read") == 1 || strcmp(str,"write") == 1 || strcmp(str,"newLine") == 1)
                    {
                        str[i]='\0';
                        fprintf(output_file,"Keyword()-> ");
                        char* temp = str;
                        fprintf(output_file,"%s\n",temp);
                        free(str);
                        break;
                    }
                    else {
                        str[i]='\0';
                        fprintf(output_file,"Identifier()-> ");
                        char* temp = str;
                        fprintf(output_file,"%s\n",temp);
                        free(str);
                        break;
                    }
                }
            }
            if (i>IDENTIFIER_SIZE) {
                printf("Identifier size error");
                exit(1);
            }
        }

    }
    fclose(input_file);
    fclose(output_file);
    return 0;
}


int hash(const char *str) {
    int hash_value = 0;
    while (*str != '\0') {
        hash_value += *str;
        str++;
    }
    return hash_value % 256;
}

char peek(FILE *input_file) {
    char c;
    c = fgetc(input_file);
    ungetc(c, input_file);
    return c;
}
char* char_to_str(char chr){
    char* temp = (char*) malloc(2);
    temp[0] = chr;
    temp[1] = '\0';
    return temp;
}