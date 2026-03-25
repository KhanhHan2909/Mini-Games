#include<winbgim.h>
#include<windows.h>
#include<string.h>
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

#define M 28

struct Block {
	int x, y;
};

struct Tetromino {
	Block B[4];
	Block B0[4];
};

struct HighScore {
	int score;
	char name[10];
};

Tetromino pieces[7]; //I-piece(0), O-piece(1), T-piece(2), S-piece(3), Z-piece(4), J-piece(5), L-piece(6)
Block ground[240];
int current_situation, color[240], sc;
bool outtomenu;
time_t start, now;
HighScore highscore[3][5];

void drawWell();
void drawblock(int x,int y,int c);
void timer(int minute,int second);
void showscore();
void initgame();
void initground();
void initIpiece(int p1,int p2);
void initOpiece(int p1,int p2);
void initTpiece(int p1,int p2);
void initSpiece(int p1,int p2);
void initZpiece(int p1,int p2);
void initJpiece(int p1,int p2);
void initLpiece(int p1, int p2);
void drawpiece(int r,int ch);
void drawnext(int r1,int r2);
void rotatepiece(int r);
int block_index(int r,int i);
void updateground(int r);
bool hitground(int r);
void redrawboard();
void popground();
void handlepiece(int r,int ch);
void movepiece(int r,int level);
bool gameover();
bool isempty(int level);
void inithighscore();
void showhighscore();
void gethighscore(int level);
void checkhighscore(int level,int _score);
void showAbout();
int menu();
void run();

void drawWell() {
	setcolor(0);
	setlinestyle(SOLID_LINE,0,THICK_WIDTH);
	rectangle(34,34+4*M,43+10*M,43+24*M);
}

void drawblock(int x,int y,int c) {
	setfillstyle(SOLID_FILL,c);
	setcolor(c);
	setlinestyle(SOLID_LINE,0,THICK_WIDTH);
	bar(x,y+3,x+M-5,y+M-3);
	rectangle(x,y,x+M-3,y+M-3);
}

void timer(int minute,int second) {
    char str[20];
    sprintf(str,"%d:%02d",minute,second);
    settextstyle(DEFAULT_FONT,HORIZ_DIR,3);
    setbkcolor(15);
    setcolor(0);
    outtextxy(449,40+4*M,str);

}

void showscore() {
	char str[30];
	sprintf(str,"%d",sc);
	settextstyle(DEFAULT_FONT,HORIZ_DIR,3);
    setbkcolor(15);
    setcolor(0);
    outtextxy(42+10*M+82,40+7*M,str);
}

void initgame() {
	setbkcolor(15);
	cleardevice();
	drawWell();
	settextstyle(10,0,7);
	outtextxy(42+4*M,25,"TETRIS 2024");
	settextstyle(8,0,1);
	outtextxy(42+10*M+13,43+4*M,"GAME TIME:");
	outtextxy(42+10*M+13,43+7*M,"SCORE:");
	settextstyle(10,0,1); setbkcolor(9); outtextxy(42+10*M+13,40+24*M-60,"PAUSE - ENTER");
	settextstyle(10,0,1); setbkcolor(12); outtextxy(42+10*M+13,40+24*M-20,"QUIT - ESC");
	rectangle(40+10*M+20,40+12*M-5,40+13*M+29,40+16*M+5);
	settextstyle(8,0,1); setbkcolor(15); outtextxy(40+10*M+13,40+16*M+10,"NEXT PIECE");
}

void initground() {
	int i;
	for(i=0;i<240;i++) { ground[i].x = -1; ground[i].y = -1; color[i] = -1; }
}

void initIpiece(int p1,int p2) {
	pieces[0].B[0].x = p1 + 4*M; pieces[0].B[0].y = p2 - 4*M; pieces[0].B[1].x = p1 + 4*M; pieces[0].B[1].y = p2 - 3*M;
	pieces[0].B[2].x = p1 + 4*M; pieces[0].B[2].y = p2 - 2*M; pieces[0].B[3].x = p1 + 4*M; pieces[0].B[3].y = p2 - M;
	for(int i=0;i<4;i++) { pieces[0].B0[i].x = pieces[0].B[i].x; pieces[0].B0[i].y = pieces[0].B[i].y; }
}

void initOpiece(int p1,int p2) {
	pieces[1].B[0].x = p1 + 4*M; pieces[1].B[0].y = p2 - 2*M; pieces[1].B[1].x = p1 + 5*M; pieces[1].B[1].y = p2 - 2*M;
	pieces[1].B[2].x = p1 + 4*M; pieces[1].B[2].y = p2 - M; pieces[1].B[3].x = p1 + 5*M; pieces[1].B[3].y = p2 - M;
	for(int i=0;i<4;i++) { pieces[1].B0[i].x = pieces[1].B[i].x; pieces[1].B0[i].y = pieces[1].B[i].y; }
}

void initTpiece(int p1,int p2) {
	pieces[2].B[0].x = p1 + 3*M; pieces[2].B[0].y = p2 - 2*M; pieces[2].B[1].x = p1 + 4*M; pieces[2].B[1].y = p2 - 2*M;
	pieces[2].B[2].x = p1 + 5*M; pieces[2].B[2].y = p2 - 2*M; pieces[2].B[3].x = p1 + 4*M; pieces[2].B[3].y = p2 - M;
	for(int i=0;i<4;i++) { pieces[2].B0[i].x = pieces[2].B[i].x; pieces[2].B0[i].y = pieces[2].B[i].y; }
}

void initSpiece(int p1,int p2) {
	pieces[3].B[0].x = p1 + 4*M; pieces[3].B[0].y = p2 - 3*M; pieces[3].B[1].x = p1 + 4*M; pieces[3].B[1].y = p2 - 2*M;
	pieces[3].B[2].x = p1 + 5*M; pieces[3].B[2].y = p2 - 2*M; pieces[3].B[3].x = p1 + 5*M; pieces[3].B[3].y = p2 - M;
	for(int i=0;i<4;i++) { pieces[3].B0[i].x = pieces[3].B[i].x; pieces[3].B0[i].y = pieces[3].B[i].y; }
}

void initZpiece(int p1,int p2) {
	pieces[4].B[0].x = p1 + 5*M; pieces[4].B[0].y = p2 - 3*M; pieces[4].B[1].x = p1 + 4*M; pieces[4].B[1].y = p2 - 2*M;
	pieces[4].B[2].x = p1 + 5*M; pieces[4].B[2].y = p2 - 2*M; pieces[4].B[3].x = p1 + 4*M; pieces[4].B[3].y = p2 - M;
	for(int i=0;i<4;i++) { pieces[4].B0[i].x = pieces[4].B[i].x; pieces[4].B0[i].y = pieces[4].B[i].y; }
}

void initJpiece(int p1,int p2) {
	pieces[5].B[0].x = p1 + 5*M; pieces[5].B[0].y = p2 - 3*M; pieces[5].B[1].x = p1 + 5*M; pieces[5].B[1].y = p2 - 2*M;
	pieces[5].B[2].x = p1 + 4*M; pieces[5].B[2].y = p2 - M; pieces[5].B[3].x = p1 + 5*M; pieces[5].B[3].y = p2 - M;
	for(int i=0;i<4;i++) { pieces[5].B0[i].x = pieces[5].B[i].x; pieces[5].B0[i].y = pieces[5].B[i].y; }
}

void initLpiece(int p1, int p2) {
	pieces[6].B[0].x = p1 + 4*M; pieces[6].B[0].y = p2 - 3*M; pieces[6].B[1].x = p1 + 4*M; pieces[6].B[1].y = p2 - 2*M;
	pieces[6].B[2].x = p1 + 4*M; pieces[6].B[2].y = p2 - M; pieces[6].B[3].x = p1 + 5*M; pieces[6].B[3].y = p2 - M;
	for(int i=0;i<4;i++) { pieces[6].B0[i].x = pieces[6].B[i].x; pieces[6].B0[i].y = pieces[6].B[i].y; }
}

void drawpiece(int r,int ch) {
	if(ch!=32)
		for(int i=3;i>=0;i--) {
			if(pieces[r].B[i].y<40+4*M) continue;
			if(ch==77) {
				if(!i) {
					if(block_index(r,i)!=block_index(r,i-1)+24) drawblock(pieces[r].B0[i].x,pieces[r].B0[i].y,15);
				}
				else drawblock(pieces[r].B0[i].x,pieces[r].B0[i].y,15);
			}
			else if(ch==75) {
				if(i!=3) {
					if(block_index(r,i)!=block_index(r,i+1)-24) drawblock(pieces[r].B0[i].x,pieces[r].B0[i].y,15);
				}
				else drawblock(pieces[r].B0[i].x,pieces[r].B0[i].y,15);
			}
			else if(ch==-1) if(pieces[r].B0[i].y>=40+4*M) {
				drawblock(pieces[r].B0[i].x,pieces[r].B0[i].y,15);
			}
			drawblock(pieces[r].B[i].x,pieces[r].B[i].y,r+1);
		}
	else {
		for(int i=3;i>=0;i--) if(pieces[r].B0[i].y>=40+4*M) drawblock(pieces[r].B0[i].x,pieces[r].B0[i].y,15);
		for(int i=3;i>=0;i--) if(pieces[r].B[i].y>=40+4*M) drawblock(pieces[r].B[i].x,pieces[r].B[i].y,r+1);
	}
}

void drawnext(int r1,int r2) {
	int i, j;
	switch(r1) {
		case 0: initIpiece(40+7*M+26,40+16*M+1); break;
		case 1: initOpiece(40+7*M+26,40+16*M+1); break;
		case 2: initTpiece(40+7*M+26,40+16*M+1); break;
		case 3: initSpiece(40+7*M+26,40+16*M+1); break;
		case 4: initZpiece(40+7*M+26,40+16*M+1); break;
		case 5: initJpiece(40+7*M+26,40+16*M+1); break;
		case 6: initLpiece(40+7*M+26,40+16*M+1);
	}
	for(i=0;i<3;i++) for(j=0;j<4;j++) drawblock(40+10*M+26+i*M,40+12*M+1+j*M,15);
	for(i=0;i<4;i++) drawblock(pieces[r1].B[i].x,pieces[r1].B[i].y,r1+1);
}

void rotatepiece(int r) {
	int i, x0, y0;
	if(r!=1) for(i=0;i<4;i++) { pieces[r].B0[i].x = pieces[r].B[i].x; pieces[r].B0[i].y = pieces[r].B[i].y; }

	switch(r) {
		case 0:
			{
				if(current_situation==1) {
					for(i=-2;i<2;i++) pieces[r].B[i+2].x = pieces[r].B[i+2].x + i*M;
					y0 = pieces[r].B[2].y;
					for(i=0;i<4;i++) pieces[r].B[i].y = y0;
					current_situation = 2;
				}
				else {
					for(i=-2;i<2;i++) pieces[r].B[i+2].y = pieces[r].B[i+2].y + i*M;
					x0 = pieces[r].B[2].x;
					for(i=0;i<4;i++) pieces[r].B[i].x = x0;
					current_situation = 1;
				}
			}
			break;
		case 2:
			{
				if(current_situation==1) {
					pieces[r].B[0].x += M; pieces[r].B[0].y -= M;
					current_situation = 2;
				}
				else if(current_situation==2) {
					for(i=1;i<3;i++) pieces[r].B[i].x -= M;
					pieces[r].B[3].x += M; pieces[r].B[3].y -= M;
					current_situation = 3;
				}
				else if(current_situation==3) {
					pieces[r].B[3].x -= M; pieces[r].B[3].y += M;
					current_situation = 4;
				}
				else {
					for(i=1;i<3;i++) pieces[r].B[i].x += M;
					pieces[r].B[0].x -= M; pieces[r].B[0].y += M;
					current_situation = 1;
				}
			}
			break;
		case 3:
			{
				if(current_situation==1) {
					pieces[r].B[1].x += M; pieces[r].B[1].y -= M;
					pieces[r].B[2].x -= 2*M;
					pieces[r].B[3].x -= M; pieces[r].B[3].y -= M;
					current_situation = 2;
				}
				else {
					pieces[r].B[1].x -= M; pieces[r].B[1].y += M;
					pieces[r].B[2].x += 2*M;
					pieces[r].B[3].x += M; pieces[r].B[3].y += M;
					current_situation = 1;
				}
			}
			break;
		case 4:
			{
				if(current_situation==1) {
					pieces[r].B[0].x -= M;
					pieces[r].B[1].x += M; pieces[r].B[1].y -= M;
					pieces[r].B[3].x += 2*M; pieces[r].B[3].y -= M;
					current_situation = 2;
				}
				else {
					pieces[r].B[0].x += M;
					pieces[r].B[1].x -= M; pieces[r].B[1].y += M;
					pieces[r].B[3].x -= 2*M; pieces[r].B[3].y += M;
					current_situation = 1;
				}
			}
			break;
		case 5:
			{
				if(current_situation==1) {
					pieces[r].B[0].x -= M; pieces[r].B[0].y += M;
					pieces[r].B[2].x += 2*M; pieces[r].B[2].y -= M;
					pieces[r].B[3].x += M;
					current_situation = 2;
				}
				else if(current_situation==2) {
					pieces[r].B[0].x += M; pieces[r].B[0].y -= M;
					pieces[r].B[1].x += M; pieces[r].B[1].y -= M;
					pieces[r].B[2].x -= M;
					pieces[r].B[3].x -= M;
					current_situation = 3;
				}
				else if(current_situation==3) {
					pieces[r].B[0].x -= M;
					pieces[r].B[1].x -= 2*M; pieces[r].B[1].y += M;
					pieces[r].B[3].x += M; pieces[r].B[3].y -= M;
					current_situation = 4;
				}
				else {
					pieces[r].B[0].x += M;
					pieces[r].B[1].x += M;
					pieces[r].B[2].x -= M; pieces[r].B[2].y += M;
					pieces[r].B[3].x -= M; pieces[r].B[3].y += M;
					current_situation = 1;
				}
			}
			break;
		case 6:
			{
				if(current_situation==1) {
					pieces[r].B[0].x += M;
					pieces[r].B[1].x -= M;
					pieces[r].B[2].y -= M;
					pieces[r].B[3].y -= M;
					current_situation = 4;
				}
				else if(current_situation==4) {
					pieces[r].B[0].x -= 2*M;
					pieces[r].B[1].x += M; pieces[r].B[1].y -= M;
					pieces[r].B[3].x -= M; pieces[r].B[3].y += M;
					current_situation = 3;
				}
				else if(current_situation==3) {
					pieces[r].B[0].y += M;
					pieces[r].B[1].y += M;
					pieces[r].B[2].x += M;
					pieces[r].B[3].x -= M;
					current_situation = 2;
				}
				else {
					pieces[r].B[0].x += M; pieces[r].B[0].y -= M;
					pieces[r].B[2].x -= M; pieces[r].B[2].y += M;
					pieces[r].B[3].x += 2*M;
					current_situation = 1;
				}
			}
	}
}

int block_index(int r,int i) {
	return (pieces[r].B[i].x - 40)*24/M + (pieces[r].B[i].y - 40)/M;
}

void updateground(int r) {
	for(int i=0;i<4;i++) {
		ground[block_index(r,i)].x = pieces[r].B[i].x;
		ground[block_index(r,i)].y = pieces[r].B[i].y;
		color[block_index(r,i)] = r + 1;
		if(block_index(r,i)%24>3) drawblock(ground[block_index(r,i)].x,ground[block_index(r,i)].y,r+1);
	}
}

bool hitground(int r) {
	int i;
	if(block_index(r,3)%24==23) return true;

	switch(r) {
		case 0:
			{
				if(current_situation==1) {
					if(ground[block_index(r,3)+1].x>-1)
						return true;
				}
				else {
					for(i=0;i<4;i++)
						if(ground[block_index(r,i)+1].x>-1)
							return true;
				}
			}
			break;
		case 1:
			{
				if(ground[block_index(r,2)+1].x>-1||ground[block_index(r,3)+1].x>-1) return true;
			}
			break;
		case 2:
			{
				if(current_situation==3) {
					for(i=1;i<4;i++) if(ground[block_index(r,i)+1].x>-1) return true;
				}
				else if(current_situation==1) {
					if(ground[block_index(r,0)+1].x>-1) return true;
					if(ground[block_index(r,2)+1].x>-1) return true;
					if(ground[block_index(r,3)+1].x>-1) return true;
				}
				else if(current_situation==2) {
					if(ground[block_index(r,2)+1].x>-1) return true;
					if(ground[block_index(r,3)+1].x>-1) return true;
				}
				else {
					if(ground[block_index(r,1)+1].x>-1) return true;
					if(ground[block_index(r,3)+1].x>-1) return true;
				}
			}
			break;
		case 3:
			{
				if(current_situation==1) {
						if(ground[block_index(r,1)+1].x>-1) return true;
						if(ground[block_index(r,3)+1].x>-1) return true;
				}
				else {
					if(ground[block_index(r,1)+1].x>-1||ground[block_index(r,2)+1].x>-1||ground[block_index(r,3)+1].x>-1) return true;
				}
			}
		case 4:
			{
				if(current_situation==1) {
						if(ground[block_index(r,2)+1].x>-1) return true;
						if(ground[block_index(r,3)+1].x>-1) return true;
				}
				else {
					if(ground[block_index(r,0)+1].x>-1||ground[block_index(r,2)+1].x>-1||ground[block_index(r,3)+1].x>-1) return true;
				}
			}
			break;
		case 5:
			{
				if(current_situation==1) {
					if(ground[block_index(r,2)+1].x>-1||ground[block_index(r,3)+1].x>-1) return true;
				}
				else if(current_situation==2) {
					if(ground[block_index(r,3)+1].x>-1) return true;
					if(ground[block_index(r,1)+1].x>-1) return true;
					if(ground[block_index(r,0)+1].x>-1) return true;
				}
				else if(current_situation==3) {
					if(ground[block_index(r,3)+1].x>-1) return true;
					if(ground[block_index(r,1)+1].x>-1) return true;
				}
				else {
					for(i=1;i<4;i++) if(ground[block_index(r,i)+1].x>-1) return true;
				}
			}
		case 6:
			{
				if(current_situation==1) {
					if(ground[block_index(r,2)+1].x>-1||ground[block_index(r,3)+1].x>-1) return true;
				}
				else if(current_situation==2) {
					if(ground[block_index(r,3)+1].x>-1) return true;
					if(ground[block_index(r,2)+1].x>-1) return true;
					if(ground[block_index(r,1)+1].x>-1) return true;
				}
				else if(current_situation==3) {
					if(ground[block_index(r,3)+1].x>-1) return true;
					if(ground[block_index(r,0)+1].x>-1) return true;
				}
				else {
					for(i=1;i<4;i++) if(ground[block_index(r,i)+1].x>-1) return true;
				}
			}
	}
	return false;
}

void redrawboard() {
	int row, col, idx;
	for(row =4;row<24;row++) {
		for(col=0;col<10;col++) {
			idx = 24*col + row;
			if(color[idx]==-1) drawblock(40+col*M,40+row*M,15);
			else drawblock(40+col*M,40+row*M,color[idx]);
		}
	}
}

void popground() {
	int row, col, rr, count;
	bool change = false;

	for(row=23;row>3;row--) {
		bool full = true;
		count = 0;
		
		for(col=0;col<10;col++) {
			if(ground[24*col+row].x==-1) {
				full = false;
				break;
			}
			else count++;
		}
		if(count==10) sc += 10;

		if(full) {
			change = true;

			for(rr=row;rr>0;rr--) {
				for(col=0;col<10;col++) {
					ground[24*col+rr] = ground[24*col+rr-1];
					color[24*col+rr] = color[24*col+rr-1];
				}
			}

			for(col=0;col<10;col++) {
				ground[24*col+0].x = -1;
				ground[24*col+0].y = -1;
				color[24*col+0] = -1;
			}
			
			row++;
		}
	}

	if(change) {
		redrawboard();
	}
}

//void popground(int r) {
//	int i, j, k, t;
//	bool change = false, notpopped;
//	for(i=0;i<4;i++) {
//		t = block_index(r,i)%24;
//		notpopped = false;
//		for(j=0;j<10;j++) if(ground[24*j+t].x==-1) { notpopped = true; break; }
//		if(!notpopped) {
//			for(j=t;j>4;j--)
//				for(k=0;k<10;k++) {
//					if(ground[24*k+j-1].x==-1) { ground[24*k+j].x = -1; ground[24*k+j].y = -1; }
//					else { ground[24*k+j].x = 40 + k*M; ground[24*k+j].y = 40 + j*M; }
//					color[20*k+j-4] = color[20*k+j-5];
//				}
//			for(j=0;j<10;j++) {
//				ground[24*j+4].x = -1; ground[24*j+4].y = -1;
//				color[20*j] = -1;
//			}
//			sc += 10;
//			change = true;
//		}
//	}
//	if(change) {
//		t = block_index(r,3)%24;
//		printf("%d ",t);
//		for(i=t;i>3;i--)
//            for(j=0;j<10;j++) {
//                if(color[20*j+i-4]>-1) {
//					drawblock(ground[24*j+i].x,ground[24*j+i].y,color[20*j+i-4]); }
//                else drawblock(40+j*M,40+4*M+(i-4)*M,15);
//            }
//	}
//}

void handlepiece(int r,int ch) {
	bool hitleftwall = false, hitrightwall = false;
	int i;
	for(i=0;i<4;i++)
		if(ground[block_index(r,i)-24].x>-1||block_index(r,i)<24) {
			hitleftwall = true;
			break;
		}
	for(i=3;i>-1;i--)
		if(ground[block_index(r,i)+24].x>-1||block_index(r,i)>215) {
			hitrightwall = true;
			break;
		}
	if(ch==32) {
		if(r==0&&(current_situation==1&&!hitleftwall&&!hitrightwall&&block_index(0,2)>47&&ground[block_index(0,2)-48].x==-1||current_situation==2)) rotatepiece(r);
		else if(r==2&&(current_situation==1||current_situation==3||current_situation==2&&!hitleftwall||current_situation==4&&!hitrightwall)) rotatepiece(r);
		else if(r==3&&(current_situation==1&&!hitleftwall||current_situation==2)) rotatepiece(r);
		else if(r==4&&(current_situation==1&&!hitrightwall||current_situation==2)) rotatepiece(r);
		else if(r==5&&(current_situation==1&&!hitrightwall||current_situation==2||current_situation==3&&!hitleftwall||current_situation==4)) rotatepiece(r);
		else if(r==6&&(current_situation==1&&!hitleftwall||current_situation==2||current_situation==3&&!hitrightwall||current_situation==4)) rotatepiece(r);
	}
	else if(ch==75&&!hitleftwall) for(int i=0;i<4;i++) { pieces[r].B0[i].x = pieces[r].B[i].x; pieces[r].B0[i].y = pieces[r].B[i].y; pieces[r].B[i].x -= M; }
	else if(ch==77&&!hitrightwall) for(int i=0;i<4;i++) { pieces[r].B0[i].x = pieces[r].B[i].x; pieces[r].B0[i].y = pieces[r].B[i].y; pieces[r].B[i].x += M; }
	drawpiece(r,ch);
}

void movepiece(int r,int level) {
	int i, minute, second, lockDelay, d_fast = 50, d = 600 - (level - 1)*200, gx, gy;
	char ch;
	bool softdrop = false, touchingground = false;
	DWORD lastFall = GetTickCount(), lockStart = 0;

	while(true) {
		while(kbhit()) {
			now = time(NULL);
			minute = (now - start)/60;
			second = (now - start)%60;
			timer(minute,second);
			ch = getch();
			if(ch==27) {
				clearmouseclick(WM_LBUTTONDOWN);
				gx = mousex();
				gy = mousey();
				while(!ismouseclick(WM_LBUTTONDOWN)||gx<280||gx>320&&gx<400||gx>428||gy<270||gy>290) {
					setfillstyle(SOLID_FILL,14); bar(150,200,530,350);
					setcolor(3); rectangle(150,200,530,350);
					settextstyle(10,0,3);
					setbkcolor(14);
					setcolor(0);
					outtextxy(280,230,"QUIT GAME?");
					setcolor(2);
					outtextxy(280,270,"YES");
					setcolor(4);
					outtextxy(400,270,"NO");
					if(ismouseclick(WM_LBUTTONDOWN)) {
						gx = mousex();
						gy = mousey();
					}
					delay(100);
				}
				if(gx>=280&&gx<=320&&gy>=270&&gy<=290) {
					outtomenu = true;
					return;
				}
				else {
					setfillstyle(SOLID_FILL,15); bar(150,200,530,350);
					setcolor(15); rectangle(150,200,530,350);
					drawWell();
					drawpiece(r,-1);
					setcolor(0);
					setbkcolor(15);
					settextstyle(8,0,1);
					outtextxy(42+10*M+13,43+7*M,"SCORE:");
					showscore();
				}
			}
			else if(ch==13) {
				setcolor(0);
				settextstyle(10,0,3);
				outtextxy(42+10*M+13,43+9*M,"Game Paused");
				ch = getch();
				while(ch!=13) {
					ch = getch();
					delay(0);
				}
				setcolor(15);
				outtextxy(42+10*M+13,43+9*M,"Game Paused");
				delay(100);
			}
            else if (ch==0||ch==224) ch = getch();
            if (ch==75||ch==77||ch==32){
				handlepiece(r, ch);
				if(!hitground(r)) {
					touchingground = false;
					lockStart = 0;
				}
			}
            else if (ch==80) {
                softdrop = true;
            }
        }
        if(!kbhit()) {
        	now = time(NULL);
			minute = (now - start)/60;
			second = (now - start)%60;
			timer(minute,second);
		}
		if (!(GetAsyncKeyState(VK_DOWN)&0x8000)) {
            softdrop = false;
        }
        
        if(hitground(r)) {
        	if(!touchingground) {
	        	touchingground = true;
	        	lockStart = GetTickCount();
			}
			lockDelay = 340 - (level - 1)*140;
			if(GetTickCount()-lockStart>=(DWORD)lockDelay) break;
		}
		
		else {
	        int currentdelay = softdrop ? d_fast : d;
	        
			if (GetTickCount()-lastFall>=(DWORD)currentdelay) {
	            for (i=0;i<4;i++) {
	                pieces[r].B0[i].x = pieces[r].B[i].x;
	                pieces[r].B0[i].y = pieces[r].B[i].y;
	                pieces[r].B[i].y += M;
	            }
	            drawpiece(r,-1);
	            lastFall = GetTickCount();
		    }
		}
		
        delay(1);
	}
}

bool gameover() {
	for(int i=3;i<=219;i+=24) if(ground[i].x>-1) return true;
	return false;
}

bool isempty(int level) {
    FILE *fp;
	if(level==1) fp = fopen("highscore1.txt","r");
	else if(level==2) fp = fopen("highscore2.txt","r");
	else fp = fopen("highscore3.txt","r");
    if (!fp) return true;
    fseek(fp,0,SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return (size<=0);
}

void inithighscore() {
	int i, j;
    for(i=1;i<4;i++) {
		if(isempty(i)) {
	        for(j=0;j<5;j++) {
	            strcpy(highscore[i-1][j].name,"unknown");
	            highscore[i-1][j].score = 0;
	        }
	        gethighscore(i);
	    }
		else {
	        FILE *f;
			if(i==1) f = fopen("highscore1.txt","r");
			else if(i==2) f = fopen("highscore2.txt","r");
			else f = fopen("highscore3.txt","r");
	        for (j=0;j<5;j++) {
	            if (fscanf(f,"%s %d",highscore[i-1][j].name,&highscore[i-1][j].score)==EOF) break;
	        }
	        fclose(f);
	    }
	}
}

void showhighscore() {
	settextstyle(10,0,1);
	FILE *f[3];
	f[0] = fopen("highscore1.txt","r");
	f[1] = fopen("highscore2.txt","r");
	f[2] = fopen("highscore3.txt","r");
	int y, count;
	char ch[20];
	for(int i=0;i<3;i++) {
		y = 200;
		count = 0;
		while(!feof(f[i])) {
			if(count == 10) break;
			count++;
			fscanf(f[i],"%s",ch);
	   		if(count%2==1) {
	   			outtextxy(50+i*240,y,ch);
	   			y += 50;
			} else outtextxy(170+i*240,y-50,ch);
		}
		fclose(f[i]);
	}
}

void gethighscore(int level) {
	FILE *f;
	if(level==1) f = fopen("highscore1.txt","w");
	else if(level==2) f = fopen("highscore2.txt","w");
	else f = fopen("highscore3.txt","w");
	for (int i=0;i<5;i++) {
		fputs(highscore[level-1][i].name,f);
		fputs("\n",f);
		fprintf(f,"%d",highscore[level-1][i].score);
		fputs("\n",f);
	}
	fclose(f);
	
}

void checkhighscore(int level,int _score) {
	char _name[10] = "";
	int count;
	char ch;
	for(int i=0;i<5;i++) {
		if(_score>highscore[level-1][i].score) {
			setcolor(5);
			settextstyle(10,0,3);
			outtextxy(280,230,"NEW HIGH SCORE!");
			setcolor(0);
			settextstyle(5,0,1);
			outtextxy(133,270,"Enter name (less than 11 characters):");
			count = 0;
			ch = 0;
			char strtemp[2] = {0,0};
			while(kbhit()) getch();
			while(count<10) {
				ch = getch();
				if(ch==13) break;
				if(ch == 0 || ch == 224) {
					getch();
					continue;
				}
				if(ch>=32&&ch<=126) {
					strtemp[0] = ch;
					strcat(_name,strtemp);
					outtextxy(290,300,_name);
					count++;
				}
			}
			for (int j=4;j>i;j--) {
                strcpy(highscore[level-1][j].name,highscore[level-1][j-1].name);
                highscore[level-1][j].score = highscore[level-1][j-1].score;
            }
            strcpy(highscore[level-1][i].name,(strlen(_name)>0) ? _name : "unknown");
            highscore[level-1][i].score = _score;
            gethighscore(level);
            break;
		}
	}
}

void showAbout() {

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

        settextstyle(9,0,5);
        setcolor(5);
        outtextxy(180,70,"TETRIS 2024");
        settextstyle(10,0,5);
        if(mx>=210&&mx<=430&&my>=200&&my<=250)
            setcolor(2);
        else
            setcolor(5);
        outtextxy(210,200,"NEW GAME");

		if(mx>=210&&mx<=465&&my>=270&&my<=320)
            setcolor(2);
        else
            setcolor(5);
        outtextxy(210,270,"HIGH SCORE");

        if(mx>=210&&mx<=385&&my>=340&&my<=390)
            setcolor(2);
        else
            setcolor(5);
        outtextxy(210,340,"ABOUT");

		if(mx>=210&&mx<=435&&my>=410&&my<=460)
            setcolor(2);
        else
            setcolor(5);
        outtextxy(210,410,"EXIT GAME");

        if(ismouseclick(WM_LBUTTONDOWN)) {
            int x0 = mousex();
            int y0 = mousey();
            clearmouseclick(WM_LBUTTONDOWN);

            if(x0>=210 && x0<=430 && y0>=200 && y0<=250) return 1;
            if(x0>=210 && x0<=435 && y0>=270 && y0<=320) return 2;
            if(x0>=210 && x0<=465 && y0>=340 && y0<=390) return 3;
            if(x0>=210 && x0<=385 && y0>=410 && y0<=460) return 4;
        }

        delay(100);
    }
}

void run() {
    initwindow(730,40+24*M+40);
    SetWindowText(GetForegroundWindow(),"TETRIS 2024");
    inithighscore();
	int choice;
	setbkcolor(15);
	cleardevice();

    while(true) {
		clearmouseclick(WM_LBUTTONDOWN);
        choice = menu();

        if(choice==1) {
        	cleardevice();
        	outtomenu = false;
        	int level = 0, gx, gy;
        	setcolor(5);
        	settextstyle(9,0,4);
        	outtextxy(164,120,"CHOOSE A LEVEL");
        	while(true) {
        		int mx = mousex(), my = mousey();
        		settextstyle(4,0,6);
        		if(mx>=170&&mx<=200&&my>=190&&my<=240) setcolor(2);
        		else setcolor(5);
        		outtextxy(170,190,"1");
        		if(mx>=250&&mx<=280&&my>=190&&my<=240) setcolor(2);
        		else setcolor(5);
        		outtextxy(250,190,"2");
        		if(mx>=330&&mx<=360&&my>=190&&my<=240) setcolor(2);
        		else setcolor(5);
        		outtextxy(330,190,"3");
        		if(ismouseclick(WM_LBUTTONDOWN)) {
        			int x0 = mousex(), y0 = mousey();
        			clearmouseclick(WM_LBUTTONDOWN);
        			if(x0>=170&&x0<=200&&y0>=190&&y0<=240) level = 1;
        			if(x0>=250&&x0<=280&&y0>=190&&y0<=240) level = 2;
        			if(x0>=330&&x0<=360&&y0>=190&&y0<=240) level = 3;
				}
				if(level) break;
				delay(100);
			}
        	newgame:
            initgame();
            initground();
            sc = 0;
            showscore();
            srand(time(NULL));
            start = time(NULL);
			int current, next = rand()%7;
            while(true) {
            	current_situation = 1;
				current = next;
            	next = rand()%7;
            	drawnext(next,current);
            	switch(current) {
            		case 0:
            			initIpiece(40,40+4*M); break;
            		case 1:
            			initOpiece(40,40+4*M); break;
            		case 2:
            			initTpiece(40,40+4*M); break;
            		case 3:
            			initSpiece(40,40+4*M); break;
            		case 4:
            			initZpiece(40,40+4*M); break;
            		case 5:
            			initJpiece(40,40+4*M); break;
            		case 6:
            			initLpiece(40,40+4*M);
				}
				movepiece(current,level);
				if(outtomenu) break;
				updateground(current);
				popground();
				showscore();
				if(gameover()) {
					setfillstyle(SOLID_FILL,14); bar(150,200,530,350);
					setcolor(3); rectangle(150,200,530,350);
					settextstyle(8,0,6);
					setcolor(12);
					setbkcolor(14);
					outtextxy(180,240,"GAME OVER");
					delay(3000);
					bar(150,200,530,350);
					checkhighscore(level,sc);
					bar(150,200,530,350);
					clearmouseclick(WM_LBUTTONDOWN);
					gx = mousex();
					gy = mousey();
					while(!ismouseclick(WM_LBUTTONDOWN)||gx<280||gx>320&&gx<400||gx>428||gy<270||gy>290) {
						settextstyle(10,0,3);
						setbkcolor(14);
						setcolor(0);
						outtextxy(280,230,"PLAY AGAIN?");
						setcolor(2);
						outtextxy(280,270,"YES");
						setcolor(4);
						outtextxy(400,270,"NO");
						if(ismouseclick(WM_LBUTTONDOWN)) {
							gx = mousex();
							gy = mousey();
						}
						delay(100);
					}
					if(gx>=280&&gx<=320&&gy>=270&&gy<=290) goto newgame;
					else break;
				}
            }
            setbkcolor(15);
            cleardevice();
		}
		
		else if(choice==2) {
			cleardevice();
			setcolor(5);
			settextstyle(9,0,5);
			outtextxy(180,70,"HIGH SCORE");
			settextstyle(9,0,3);
			outtextxy(50,150,"LEVEL 1");
			outtextxy(290,150,"LEVEL 2");
			outtextxy(530,150,"LEVEL 3");
			showhighscore();
			while(!ismouseclick(WM_LBUTTONDOWN)) {
        	delay(90);
        	}
        	cleardevice();
		}

        else if(choice==3) {
        	cleardevice();
            showAbout();
            cleardevice();
        }

        else if(choice==4) {
            closegraph();
            exit(0);
        }

        delay(120);
    }
}

int main() {
	run();
	getch();
	closegraph();
}
