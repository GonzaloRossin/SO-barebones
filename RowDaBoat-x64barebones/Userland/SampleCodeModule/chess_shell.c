#include<shell.h>
#include<libasm.h>
#include<Lib.h>
#include<chesslib.h>

//Buffer to store the input from the keyboard.
#define BUFFER 50
#define MAX_COMMANDS 10
#define MAX_DESC 50
#define TIMER_X 850
#define TIMER_Y 40
#define ERROR_X 5
#define ERROR_Y 130
#define GAME_DURATION_IN_SECONDS 5
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 1
#define BACKGROUND_COLOR1 0x0000FF

#define ISCOL(c) (( (c <= 'h'&& c>='a') || (c<='H' && c>='A'))  ? 1 : 0)
#define ISROW(c) ( (c>='0' && c<='8') ? 1 : 0)

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
    int color;
}player;
typedef struct log{
    char from[2];
    char to[2];
    char* playername;
    int order;
}log;

static log loghistory[100];
static int count=0;

static int FLAG_START=0;
static int FLAG_END=0;
static int LOG_X=5;
static int LOG_Y=170;
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
    players[0].name="player 1 : ";
    players[1].name="player 2 : ";
    players[0].color = 0xFFFFFF;
    players[1].color = 0x000000;
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
void restartgame(){
    FLAG_START=0;
    FLAG_END=0;
    command_size=0;
    initialize_chess();
}
void fillChessCommand(char* name,char *desc, void (*cmdptr)(void))
{
    chesscommand aux;
    strncpy(name,aux.command_name,0,strlen(name));
    strncpy(desc, aux.desc,0, strlen(desc));
    aux.cmdptr = cmdptr;
    chess_commands[command_size++] = aux;
}
void fillChessList()
{
    fillChessCommand("start",": inicia el tiempo y el juego",&start);
    fillChessCommand("restart",": reinicia el juego",&restartgame);
}
int CommandHandlerChess(){
    char potentialCommand[BUFFER] = {0};
    strncpy(buffer, potentialCommand,0, buffer_size);
    for (int i = 0; i < command_size; i++)
    {
        if (strcmp(potentialCommand, chess_commands[i].command_name))
        {
            (chess_commands[i].cmdptr)();
            putActioncall(3);
            return 0;
        }
        else if(ISCOL(potentialCommand[0]) && ISROW(potentialCommand[1]) && potentialCommand[2]=='-' && ISCOL(potentialCommand[3]) && ISROW(potentialCommand[4])){
            chess_square * origin = get_board_tile(potentialCommand[1]-'0', potentialCommand[0]);
            chess_square * destiny = get_board_tile(potentialCommand[4]-'0', potentialCommand[3]);
            if (validate_player(origin, players[currentplayer].color)==0)
            {
                sys_cursor(ERROR_X,ERROR_Y);
                print("Invalid Piece");
                sys_cursor(-1,-1);
                return 0;
            }
            int validate = validate_move(origin, destiny);
            if(validate>=0){
                char movefrom[3]={0};
                char moveto[3]={0};
                strncpy(potentialCommand,movefrom,0,2);
                strncpy(potentialCommand,moveto,3,5);
                sys_cursor(150,ERROR_Y); //PARA BORRAR EL MENSAJE DE ERROR CUANDO EL MOVIMIENTO ES VALIDO
                putActioncall(3);
                sys_cursor(-1,-1);
                printlog(movefrom,moveto);
            }
            if (validate < 0){
                sys_cursor(ERROR_X,ERROR_Y);
                print("Invalid Move");
                sys_cursor(-1,-1);
                return 0;
            }
            if (validate == 0)
            {
                move(origin->row, origin->column, destiny->row, destiny->column);
                return 1;
            }
            if (validate == 1)
            {
                castling_move(origin, destiny);
                return 1;
            }
            if (validate == 2)
            {
                //al_paso_move(origin, destiny);
                return 1;
            }
        } 
    }
    sys_cursor(ERROR_X,ERROR_Y);
    print("Invalid command");
    sys_cursor(-1,-1);
    return 0;
}

void printlog(char* source,char* destiny){

    if(LOG_Y>=600){
        LOG_Y=150;
    }
    sys_cursor(LOG_X,LOG_Y);
    print(players[currentplayer].name);
    print(source);
    print("--->");
    print(destiny);
    sys_cursor(-1, -1);
    LOG_Y+=16;
}
void mini_shell(){
    print("WELCOME TO CHESS, press help to view commands");
    newline();
    print("Los comandos a disposicion del usuario son los siguientes:");
    newLine();
    newLine();
    for(int i=0;i<command_size;i++){
        print(chess_commands[i].command_name);
        print(chess_commands[i].desc);
        newLine();
    }
    print("Para mover un pieza debe escribir la casilla a desplazar+\"-\"+casilla de destino");
    newLine();
    put_char('>');
    sys_cursor(5,150);
    print("Log:");
    sys_cursor(-1,-1);
    while(1 && !FLAG_END){
        if(FLAG_START){
            decrecetime();
        }
        if(readChessInput()){
            if(CommandHandlerChess()){
                playerswap();
            }
            put_char('>');
            cleanChessBuffer();
        }
    }
    sys_cursor(ERROR_X,ERROR_Y);
    print("JUEGO FINALIZADO, ");
    print("gana el jugador: ");
    print(players[!currentplayer].name);
    for(int i=0;i<2;i++){
        putActioncall(1);
    }
    sys_cursor(-1,-1);
    while(1){
        if(readChessInput()){
            CommandHandlerChess();
            put_char('>');
            cleanChessBuffer();
        }
    }
}