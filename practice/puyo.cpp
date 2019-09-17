//original sorce
//https://www.youtube.com/watch?v=SalSGpiBdvo
#include<conio.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>

#define FIELD_WIDTH    8
#define FIELD_HEIGHT  14
#define PUYO_START_X   3
#define PUYO_START_Y   1
#define PUYO_COLOR_MAX 4

enum {
    CELL_NONE,
    CELL_WALL,
    CELL_PUYO_0,
    CELL_PUYO_1,
    CELL_PUYO_2,
    CELL_PUYO_3,
    CELL_MAX
};
enum {
    PUYO_ANGLE_0,
    PUYO_ANGLE_90,
    PUYO_ANGLE_180,
    PUYO_ANGLE_270,
    PUYO_ANGLE_MAX
};
int puyoSubPotions[][2] = {
    {0,-1}, //PUYO_ANGLE_0,
    {-1,0}, //PUYO_ANGLE_90,
    {0,1}, //PUYO_ANGLE_180,
    {1,0}  //PUYO_ANGLE_270,
};

int cells[FIELD_HEIGHT][FIELD_WIDTH];
int displayBuffer[FIELD_HEIGHT][FIELD_WIDTH];
int checked[FIELD_HEIGHT][FIELD_WIDTH];
char cellNames[][2 + 1] = {
    "ÅE", //CELL_NONE
    "Å°", //CELL_WALL
    "ÅZ", //CELL_PUYO_0
    "Å¢", //CELL_PUYO_1
    "Å†", //CELL_PUYO_2
    "Åô"  //CELL_PUYO_3
};

int puyoX = PUYO_START_X;
int puyoY = PUYO_START_Y;
int puyoColor;
int puyoAngle;
bool lock = false;
int score = 0;

void display() {
    system("cls");
    memcpy(displayBuffer,cells, sizeof cells);

    if (!lock) {
        int subX = puyoX + puyoSubPotions[puyoAngle][0];
        int subY = puyoY + puyoSubPotions[puyoAngle][1];
        displayBuffer[puyoY][puyoX] = displayBuffer[subY][subX] = CELL_PUYO_0 + puyoColor;
    }
    for(int y=1; y<FIELD_HEIGHT; y++){
        for(int x=0; x<FIELD_WIDTH; x++)
            printf("%s", cellNames[displayBuffer[y][x]]);
        printf("\n");
    }
    printf("Chain:%d", score);
}

bool intersectPuyoToField(int _puyoX, int _puyoY, int _puyoAngle){
    if(cells[_puyoY][_puyoX] != CELL_NONE)
        return true;
    
    int subX = _puyoX + puyoSubPotions[_puyoAngle][0];
    int subY = _puyoY + puyoSubPotions[_puyoAngle][1];
    if(cells[subY][subX] != CELL_NONE)
        return true;


    return false;
}

int getPuyoConnectedCount(int _x, int _y, int _cell, int _count) {
    if(
            checked[_y][_x]
        || (cells[_y][_x] != _cell)
    )
        return _count;
    
    _count++;
    checked[_y][_x] = true;

    for(int i = 0; i < PUYO_ANGLE_MAX; i++) {
        int x = _x + puyoSubPotions[i][0];
        int y = _y + puyoSubPotions[i][1];
        _count = getPuyoConnectedCount(x, y, _cell, _count);
    }

    return _count;
}

void erasePuyo(int _x, int _y, int _cell){
    if(cells[_y][_x] != _cell)
        return;
    
    cells[_y][_x] = CELL_NONE;

    for(int i = 0; i < PUYO_ANGLE_MAX; i++) {
        int x = _x + puyoSubPotions[i][0];
        int y = _y + puyoSubPotions[i][1];
        erasePuyo(x, y, _cell);
    }
    
}


int main() {
    srand((unsigned int)time(NULL));
    for(int y=0; y<FIELD_HEIGHT; y++)
        cells[y][0] = cells[y][FIELD_WIDTH-1]  = CELL_WALL;
    for(int x=0; x<FIELD_WIDTH; x++)
        cells[FIELD_HEIGHT-1][x] = CELL_WALL;

    puyoColor = rand() % PUYO_COLOR_MAX;
    time_t t = 0;
    
    while(1) {
        if(t < time(NULL)){
            t = time(NULL);

            if(!lock){
                if(!intersectPuyoToField(puyoX,puyoY+1,puyoAngle)) {
                    puyoY++;
                } else {
                    int subX = puyoX + puyoSubPotions[puyoAngle][0];
                    int subY = puyoY + puyoSubPotions[puyoAngle][1];
                    cells[puyoY][puyoX] = cells[subY][subX] = CELL_PUYO_0 + puyoColor;
                    puyoX = PUYO_START_X;
                    puyoY = PUYO_START_Y;
                    puyoAngle = PUYO_ANGLE_0;
                    puyoColor = rand() % PUYO_COLOR_MAX;
                    lock = true;
                }
            }
            if (lock) {
                lock = false;
                for(int y = FIELD_HEIGHT - 3; y>=0; y--)
                    for(int x = 1; x < FIELD_WIDTH-1; x++)
                        if(cells[y][x] != CELL_NONE
                        && (cells[y+1][x] == CELL_NONE)){
                            cells[y + 1][x] = cells[y][x];
                            cells[y][x] = CELL_NONE;
                            lock = true;
                        }
                if(!lock) {
                    memset(checked,0,sizeof checked);
                    for(int y=0; y<FIELD_HEIGHT-1; y++)
                        for (int x = 1; x<FIELD_WIDTH-1; x++){
                            if(cells[y][x] != CELL_NONE)
                                if(getPuyoConnectedCount(x,y,cells[y][x],0) >= 4){
                                    erasePuyo(x,y,cells[y][x]);
                                    score++;
                                    lock = true;
                                }
                        }
                }
            }


            display();
        }

        if(_kbhit()) {
            if(lock)
                _getch();
            else {
                int x = puyoX;
                int y = puyoY;
                int angle = puyoAngle;
                switch(_getch()) {
                //case 'w':y--; break;
                case 's':y++; break;
                case 'a':x--; break;
                case 'd':x++; break;
                case ' ': angle = (++angle) % PUYO_ANGLE_MAX;
                }
                if (!intersectPuyoToField(x,y,angle)){
                    puyoX = x;
                    puyoY = y;
                    puyoAngle = angle;
                }
                display();
            }

        }
    }

}