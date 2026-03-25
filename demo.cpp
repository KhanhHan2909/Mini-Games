#include<winbgim.h>
#include<windows.h>
#include<string.h>
#include<stdio.h>
#include<time.h>

struct Cell {
	int x, y;
};

int turn = 0, currentindex = -1;
Cell cell[256];
bool canUndo, endGame;

void drawX(Cell *cell);
void drawO(Cell *cell);
void drawtable();
void initgame();
void checking(int mx,int my);
void undo(int c);
bool Xwin(Cell cll[]);
bool Owin(Cell cll[]);
bool gamedraw();
void resetvalues();
void timer(int minute,int second);
void showAbout();
int menu();
void run();

void drawX(Cell *cell) { //PHAI DUNG CON TRO DE TRANH CHECK 2 LAN VAO 1 O
	setcolor(4);
	setlinestyle(SOLID_LINE,0,THICK_WIDTH);
	line(cell->x+3,cell->y+3,cell->x+37,cell->y+37);
	line(cell->x+37,cell->y+3,cell->x+3,cell->y+37);
	turn++;
	cell->x = -1;
	cell->y = -1;
}

void drawO(Cell *cell) {
	setcolor(2);
	setlinestyle(SOLID_LINE,0,THICK_WIDTH);
	circle(cell->x+20,cell->y+20,17);
	turn++;
	cell->x = -2;
	cell->y = -2;
}

void drawtable() {
	int i, j, count = 0;
	setcolor(15);
	setlinestyle(SOLID_LINE,0,NORM_WIDTH);
	for(i=155;i<795;i=i+40)
		for(j=80;j<720;j=j+40)
		{
			rectangle(i,j,i+40,j+40);
			cell[count].x = i; cell[count].y = j;
			count++;
		}
}

void initgame() {
	setbkcolor(8);
	cleardevice();
	drawtable();
	settextstyle(7,0,5); setcolor(3); outtextxy(399,20,"HAVE FUN!!");
	setfillstyle(SOLID_FILL,12); bar(10,10,105,30); settextstyle(1,0,1); setbkcolor(12); setcolor(0); outtextxy(19,10,"QUIT");
	setfillstyle(SOLID_FILL,9); bar(10,50,105,70); settextstyle(1,0,1); setbkcolor(9); outtextxy(10,50,"UNDO");
}

void checking(int mx,int my) {
	if(mx>=155&&mx<=795&&my>=80&&my<=720) {
		for(int i=0;i<256;i++) {
			if(cell[i].x==-1) continue;
			if(mx>=cell[i].x&&mx<(cell[i].x+40)&&my>=cell[i].y&&my<(cell[i].y+40)) {
				if(!(turn%2)) drawX(&cell[i]);
				else drawO(&cell[i]);
				currentindex = i;
				canUndo = true;
				break;
			}
		}
	}
}

void undo(int c) {
	if(!canUndo) return;
	int x0 = 155 + c/16*40;
	int y0 = 80 + c%16*40;
	cell[c].x = x0;
	cell[c].y = y0;
	setcolor(8);
	setlinestyle(SOLID_LINE,0,THICK_WIDTH);
	if(turn%2) {
		line(x0+3,y0+3,x0+37,y0+37);
		line(x0+37,y0+3,x0+3,y0+37);
		turn--;
	}
	else {
		circle(x0+20,y0+20,17);
		turn--;
	}
	canUndo = false;
}

bool Xwin(Cell cll[]) {
	int c = currentindex, i, string;
	string = 1;
	for(i=c+1;i<c+5;i++) {
		if(!(i%16)) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	for(i=c-1;i>c-5;i--) {
		if(i%16==15) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	if(string>4) return true;
	
	string = 1;
	for(i=c+16;i<c+80;i=i+16) {
		if(i>255) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	for(i=c-16;i>c-80;i=i-16) {
		if(i<0) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	if(string>4) return true;
	
	string = 1;
	for(i=c+17;i<c+85;i=i+17) {
		if(!(i%16)||i>255) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	for(i=c-17;i>c-85;i=i-17) {
		if(i%16==15||i<0) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	if(string>4) return true;
	
	string = 1;
	for(i=c+15;i<c+75;i=i+15) {
		if((i%16)==15||i>255) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	for(i=c-15;i>c-75;i=i-15) {
		if(!(i%16)||i<0) break;
		if(cll[i].x==-1) string++;
		else break;
	}
	if(string>4) return true;
	return false;
}

bool Owin(Cell cll[]) {
	int c = currentindex, i, string;
	string = 1;
	for(i=c+1;i<c+5;i++) {
		if(!(i%16)) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	for(i=c-1;i>c-5;i--) {
		if(i%16==15) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	if(string>4) return true;
	
	string = 1;
	for(i=c+16;i<c+80;i=i+16) {
		if(i>255) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	for(i=c-16;i>c-80;i=i-16) {
		if(i<0) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	if(string>4) return true;
	
	string = 1;
	for(i=c+17;i<c+85;i=i+17) {
		if(!(i%16)||i>255) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	for(i=c-17;i>c-85;i=i-17) {
		if(i%16==15||i<0) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	if(string>4) return true;
	
	string = 1;
	for(i=c+15;i<c+75;i=i+15) {
		if((i%16)==15||i>255) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	for(i=c-15;i>c-75;i=i-15) {
		if(!(i%16)||i<0) break;
		if(cll[i].x==-2) string++;
		else break;
	}
	if(string>4) return true;
	
	return false;
}

bool gamedraw() {
	if(turn<256) return false;
	else {
		if(!Owin(cell)) return true;
	}
	return false;
}

void resetvalues() {
	currentindex = -1;
    turn = 0;
}

void timer(int minute,int second) {
    char str[20];
    sprintf(str,"%d:%02d",minute,second);
    settextstyle(DEFAULT_FONT,HORIZ_DIR,3);
    setbkcolor(8);
    setcolor(15);
    outtextxy(830,20,str);

}
void showAbout() {
    cleardevice();

    settextstyle(7,0,3);
    setcolor(0);

    outtextxy(180,216,"VE NHA PHAT TRIEN...");
    outtextxy(180,296,"- APTECH HANOI.");
    outtextxy(180,376,"- NGUYEN KHANH HAN.");

    while(!ismouseclick(WM_LBUTTONDOWN)) {
        delay(90);
    }

    clearmouseclick(WM_LBUTTONDOWN);
}

int menu() {
	setbkcolor(15);
	
    while(true) {
        int mx = mousex();
        int my = mousey();

        settextstyle(1,0,6);
        setcolor(5);
        outtextxy(180,70,"CHECKERS 2025");
        settextstyle(10,0,5);
        if(mx>=300&&mx<=520&&my>=200&&my<=250)
            setcolor(2);
        else
            setcolor(5);
        outtextxy(300,200,"NEW GAME");

        if(mx>=300&&mx<=475&&my>=270&&my<=320)
            setcolor(2);
        else
            setcolor(5);
        outtextxy(300,270,"ABOUT");

        if(mx>=300&&mx<=525&&my>=340&&my<=390)
            setcolor(2);
        else
            setcolor(5);
        outtextxy(300,340,"EXIT GAME");

        if(ismouseclick(WM_LBUTTONDOWN)) {
            int x0 = mousex();
            int y0 = mousey();
            clearmouseclick(WM_LBUTTONDOWN);

            if(x0>=300 && x0<=520 && y0>=200 && y0<=250) return 1;
            if(x0>=300 && x0<=520 && y0>=270 && y0<=320) return 2;
            if(x0>=300 && x0<=520 && y0>=340 && y0<=390) return 3;
        }

        delay(100);
    }
}

void run() {
    initwindow(1000,750);
    SetWindowText(GetForegroundWindow(),"Vietnamese Checkers");
    setbkcolor(15);
    cleardevice();

    while(true) {
    	bool outtomenu = false;
        int choice = menu();

        if(choice==1) {
        	newgame:
            initgame();
            time_t start = time(NULL);
            canUndo = false;
			endGame = false;
            char s[100];
            while(true) {
            	time_t now = time(NULL);
            	int minute = (now - start)/60;
    			int second = (now - start)%60;
            	timer(minute,second);
                if(ismouseclick(WM_LBUTTONDOWN)) {
                    int gx = mousex();
                    int gy = mousey();
                    clearmouseclick(WM_LBUTTONDOWN);
                    if(gx>=10&&gx<=105&&gy>=10&&gy<=30) {
                    	setbkcolor(15);
                    	outtomenu = true;
                    	break;
					}
					if(gx>=10&&gx<=105&&gy>=50&&gy<=70&&canUndo) undo(currentindex);
                    checking(gx,gy);
                    settextstyle(10,0,4);
                    setcolor(12);
                    setbkcolor(9);
                    if(turn%2) {
						if(Xwin(cell)) { outtextxy(308,230,"GAME OVER: X WINS!"); endGame = true; }
					}
                    else {
						if(Owin(cell)) { outtextxy(308,230,"GAME OVER: O WINS!"); endGame = true; }
                    	if(gamedraw()) { outtextxy(360,230,"GAME DRAW!"); endGame = true; }
                    }
                    if(endGame) {
                    	gx = mousex();
                    	gy = mousey();
                    	while(gx<340||gx>390&&gx<550||gx>590||gy<310||gy>350) {
                    		setbkcolor(9);
                    		setcolor(12);
	                    	outtextxy(365,270,"PLAY AGAIN?");
	                    	setcolor(15);
	                    	setbkcolor(2);
	                    	outtextxy(340,310,"YES");
	                    	setbkcolor(12);
	                    	outtextxy(550,310,"NO");
	                    	if(ismouseclick(WM_LBUTTONDOWN)) {
	                    		gx = mousex();
	                    		gy = mousey();
	                    	}
	                    	clearmouseclick(WM_LBUTTONDOWN);
                    		delay(50);
                    	}
                    	if(gx>=340&&gx<=390&&gy>=310&&gy<=350) {
                    		resetvalues();
                    		goto newgame;
                    	}
                        setbkcolor(15);
                        resetvalues();
                        break;
                    }
                }
                delay(36);
            }
            cleardevice();
            if(outtomenu) continue;
		}
		
		else if(choice==2) {
            showAbout();
            cleardevice();
        }
        
		else if(choice==3) {
            closegraph();
            exit(0);
        }

        delay(100);
    }
}

int main() {
	run();
	getch();
	closegraph();
    return 0;
}