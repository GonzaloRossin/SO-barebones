#include<shell.h>
#include<libasm.h>
#include<Lib.h>
#include<chesslib.h>

//Buffer to store the input from the keyboard.
#define BUFFER 50
#define MAX_COMMANDS 10
#define MAX_DESC 50
#define TIMER_X 25
#define TIMER_Y 100
#define GAME_DURATION_IN_SECONDS 10
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 1
#define BACKGROUND_COLOR1 0x0000FF

matrix_struct * timermatrix;

typedef struct chesscommand
{
    char command_name[MAX_DESC];
    char desc[MAX_DESC];
    void (*cmdptr)(void);
} chesscommand;

typedef struct playertime{
    unsigned long time_reference;
    int total_seconds;
}playertime;

typedef struct player{
    char* name;
    playertime timer;
}player;

static int FLAG_START=0;
static int FLAG_END=0;
static player players[2];
static int currentplayer;

static chesscommand chess_commands[MAX_COMMANDS];
static int command_size = 0;

static char buffer[BUFFER+ 1] = {0}; //Non cyclic buffer
static int buffer_size = 0;

void print_time(){
    int timedata[2][2]={{players[0].timer.total_seconds/60,players[0].timer.total_seconds%60},
                        {players[1].timer.total_seconds/60,players[1].timer.total_seconds%60}};//minutos[0],segundos[1]
    sys_cursor(TIMER_X,TIMER_Y);
    for(int j=0;j<2;j++){
        print(players[j].name);
        for(int i=0;i<2;i++){//IMPRIMO MINUTOS SI I=0 Y SEGUNDOS SI I=1
            int dec=timedata[j][i]/10;
            int res=timedata[j][i]%10;
            print_num(dec,0);
            print_num(res,0);
            if(i==0)
                put_char(':');
        }
        sys_cursor(-1,-1);//VUELVO A LA POSICION DEL SHELL
        sys_cursor(TIMER_X,TIMER_Y-25);
    }
    sys_cursor(-1,-1);
}

void decrecetime(){
    int remaining_time=players[currentplayer].timer.total_seconds-(get_seconds()-players[currentplayer].timer.time_reference);
    if(remaining_time==0){
        FLAG_END=1;
        return;
    }
    players[currentplayer].timer.total_seconds=remaining_time;
    players[currentplayer].timer.time_reference=get_seconds();
    print_time();
}

void start(){
    player aux;
    for(int i=0;i<2;i++){
        aux.timer.total_seconds=GAME_DURATION_IN_SECONDS;
        aux.timer.time_reference=get_seconds();
        players[i]=aux;
    }
    players[0].name="player 2:";
    players[1].name="player 1:";
    FLAG_START=1;
    currentplayer=0;
}
void playerswap(){
    if(currentplayer==1)
        currentplayer=0;
    else
        currentplayer=1;
    players[currentplayer].timer.time_reference=get_seconds();
    
}
static void chess_help(){
    print("Los comandos a disposicion del usuario son los siguientes:");
    newLine();
    newLine();
    for(int i=0;i<command_size;i++){
        print(chess_commands[i].command_name);
        print(chess_commands[i].desc);
        newLine();
    }
}
static int readChessInput()
{
    
    char chartoadd=read_input();

    //If there is nothing new or its not a valid character...
    if (chartoadd == 0)
    {
        return 0;
    }
    else if(chartoadd==ENTER){
        putActioncall(3);
        return 1;
    }
    else if(chartoadd==BACKSPACE){
        if (buffer_size > 0)
        {
            buffer[--buffer_size] = 0;
            putActioncall(1);
        }
        return 0;
    }
    //If its a regular letter.
    else
    {
        if (buffer_size <= 100)
        {
            buffer[buffer_size++] = chartoadd;
            put_char(chartoadd);
            return 0;
        }
    }

    //Just in case
    return 0;
}
static void cleanChessBuffer(){
    for (int i = 0; i < BUFFER; i++)
    {
       buffer[i] = 0;
    }
    buffer_size = 0;
}
void fillChessCommand(char* name,char *desc, void (*cmdptr)(void))
{
    chesscommand aux;
    strncpy(name,aux.command_name,0,strlen(name));
    strncpy(desc, aux.desc,0, strlen(desc));
    aux.cmdptr = cmdptr;
    chess_commands[command_size++] = aux;
}
void fillCommandsChess()
{
    fillChessCommand("start",": inicia el tiempo y el juego",&start);
    fillChessCommand("help",": muestra comandos disponibles",&chess_help);

}
void CommandHandlerChess(){
    char potentialCommand[BUFFER] = {0};
    strncpy(buffer, potentialCommand,0, buffer_size);
    for (int i = 0; i < command_size; i++)
    {
        if (strcmp(potentialCommand, chess_commands[i].command_name))
        {
            (chess_commands[i].cmdptr)();
            putActioncall(3);
            return;
        }
        if(potentialCommand[i]==' '){
            char* source;
            char* finalposition;
            strncpy(potentialCommand,source,0,i);
            strncpy(potentialCommand,finalposition,i+1,strlen(potentialCommand));
            return;
        }
    }
}

void mini_shell(){
    put_char('>');
    while(1 && !FLAG_END){
        if(FLAG_START){
            decrecetime();
        }
        if(readChessInput()){
            CommandHandlerChess();
            playerswap();
            put_char('>');
            cleanChessBuffer();
        }
    }
    sys_cursor(0,0);
    print("JUEGO FINALIZADO");
    newLine();
    playerswap();
    print("gana el jugador: ");
    print(players[currentplayer].name);
}