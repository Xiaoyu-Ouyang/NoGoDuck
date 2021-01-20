/*****
这是终端交互的不围棋程序，通过键盘输入。全部代码由欧阳霄宇完成。
*****/
#include <iostream>
#include<time.h>
#include <unistd.h>
#include<cmath>
#include <iomanip>
using namespace std;
#define random(a,b) (rand()%(b-a+1)+a)

int N=3;
int board[9][9]={0};
int saver[81][9][9]={0};
int singleAirBoard[9][9]={0};
int multiAirBoard[9][9]={0};
int tmpboard[9][9]={0};
int valueBoard[9][9]={0};
int tmpX=0,tmpY=0;
int tmpX2=0,tmpY2=0;
double totaltime=0;

void reload(int x,int y,int chess);
void tag(int x,int y);
void clearBoard(int sampleboard[][9]);
int singleAir(int x,int y);
int multiAir(int x,int y);
struct step{int x;int y;int chess;};
step AI(int chess);
step youngAI(int chess);
int chessAround(int x,int y,int chess);

int chessAround(int x,int y,int chess)
{
	int result=0;
	if(x>1 && board[x-1][y]==chess) result++;
	if(x<8 && board[x+1][y]==chess) result++;
	if(y>1 && board[x][y-1]==chess) result++;
	if(y<8 && board[x][y+1]==chess) result++;
	return result;
}
void clearBoard(int sampleboard[][9])
{
	int t1=0,t2=0;
	for(t1=0;t1<9;t1++){
		for(t2=0;t2<9;t2++){
			sampleboard[t1][t2]=0;
		}
	}
}
void reload(int x,int y,int chess)
{
	board[x][y]=chess;
	multiAirBoard[x][y]=multiAir(x,y);
	if(x>0 && board[x-1][y]!=0)
		multiAirBoard[x-1][y]=multiAir(x-1,y);
	if(x<8 && board[x+1][y]!=0)
		multiAirBoard[x+1][y]=multiAir(x+1,y);
	if(y>0 && board[x][y-1]!=0)
		multiAirBoard[x][y-1]=multiAir(x,y-1);
	if(y<8 && board[x][y+1]!=0)
		multiAirBoard[x][y+1]=multiAir(x,y+1);
}
int singleAir(int x,int y)//计算单个有棋位置棋子的气（注意连通性）
{
	int airNum=0;
	if(x>0 && board[x-1][y]==0) airNum++;
	if(x<8 && board[x+1][y]==0) airNum++;
	if(y>0 && board[x][y-1]==0) airNum++;
	if(y<8 && board[x][y+1]==0) airNum++;
	singleAirBoard[x][y]=airNum;
	return airNum;
}
void tag(int x,int y)
{
	tmpboard[x][y]=1;
	if(x>0 && board[x-1][y]==board[x][y] && tmpboard[x-1][y]==0)
		tag(x-1,y);
	if(x<8 && board[x+1][y]==board[x][y] && tmpboard[x+1][y]==0)
		tag(x+1,y);
	if(y>0 && board[x][y-1]==board[x][y] && tmpboard[x][y-1]==0)
		tag(x,y-1);
	if(y<8 && board[x][y+1]==board[x][y] && tmpboard[x][y+1]==0)
		tag(x,y+1);
}

int multiAir(int x,int y)//计算一片棋子总气数
{
	int result=0;
	clearBoard(tmpboard);
	tag(x,y);
	for(int t1=0;t1<9;t1++){
		for(int t2=0;t2<9;t2++){
			if(tmpboard[t1][t2]==1){
				result+=singleAir(t1,t2);
			}
		}
	}
	for(int t1=0;t1<9;t1++){
		for(int t2=0;t2<9;t2++){
			if(tmpboard[t1][t2]==1)
				multiAirBoard[t1][t2]=result;
		}
	}
	return result;
}

bool judgeForbidPoint(int chess, int x,int y)//判断下在这个点会不会导致自己被提掉（即不允许下的点）
{
	bool flag=0;
	if ((x==0 || board[x-1][y]==-chess || multiAirBoard[x-1][y]==1)&&
		(x==8 || board[x+1][y]==-chess || multiAirBoard[x+1][y]==1)&&
		(y==0 || board[x][y-1]==-chess || multiAirBoard[x][y-1]==1)&&
		(y==8 || board[x][y+1]==-chess || multiAirBoard[x][y+1]==1)){
		flag=1;
	}
	return flag;
}

bool judgeDeadPoint(int chess, int x, int y)//判断chess下在这个点会不会把别的子提掉（即自杀输棋）
{
	bool flag=0;
	if ((x>0 && board[x-1][y]==-chess && multiAirBoard[x-1][y]==1)||
		(x<8 && board[x+1][y]==-chess && multiAirBoard[x+1][y]==1)||
		(y>0 && board[x][y-1]==-chess && multiAirBoard[x][y-1]==1)||
		(y<8 && board[x][y+1]==-chess && multiAirBoard[x][y+1]==1)){
		flag=1;
	}
	return flag;
}
int value(int x,int y,int chess,int tmp){
	tmp++;
	int result=0;
	board[x][y]=chess;
	reload(x,y,chess);
	for(int t1=0;t1<9;t1++){
		for(int t2=0;t2<9;t2++){
			if(board[t1][t2]==0){
				result+=2*judgeDeadPoint(-chess,t1,t2)+
				2*judgeForbidPoint(-chess,t1,t2)-
				judgeDeadPoint(chess,t1,t2)-
				judgeForbidPoint(chess,t1,t2);
			}
		}
	}
	if(tmp<N){
		int max=0;
		for(int t1=0;t1<9;t1++){
			for(int t2=0;t2<9;t2++){
				if(board[t1][t2]==0 && 
					judgeForbidPoint(-chess,t1,t2)==0 && 
					judgeDeadPoint(-chess,t1,t2)==0){
					int tmp_value=value(t1,t2,-chess,tmp);
					if (max<tmp_value) max=tmp_value;
				}
			}
		}
		result-=max;
	}
	board[x][y]=0;
	reload(x,y,0);
	valueBoard[x][y]=result;
	return result;
}
int valueEarlyUse(int x,int y,int chess,int tmp){
	tmp++;
	int result=0;
	board[x][y]=chess;
	reload(x,y,chess);
	result=-3*multiAirBoard[x][y]-3*chessAround(x,y,chess);
	if(x>0 && board[x-1][y]==0){
		if((x<=1 || judgeDeadPoint(chess,x-2,y)==0)&&
			(y<=0 || judgeDeadPoint(chess,x-1,y-1)==0)&&
			(y>=8 || judgeDeadPoint(chess,x-1,y+1)==0))
			result+=2*chessAround(x-1,y,chess)-chessAround(x-1,y,-chess);
	}
	if(x<8 && board[x+1][y]==0) {
		if((x>=7 || judgeDeadPoint(chess,x+2,y)==0)&&
			(y<=0 || judgeDeadPoint(chess,x+1,y-1)==0)&&
			(y>=8 || judgeDeadPoint(chess,x+1,y+1)==0))
			result+=2*chessAround(x+1,y,chess)-chessAround(x+1,y,-chess);
	}
	if(y>0 && board[x][y-1]==0) {
		if((y<=1 || judgeDeadPoint(chess,x,y-2)==0)&&
			(x<=0 || judgeDeadPoint(chess,x-1,y-1)==0)&&
			(x>=8 || judgeDeadPoint(chess,x+1,y-1)==0))
			result+=2*chessAround(x,y-1,chess)-chessAround(x,y-1,-chess);
	}
	if(y<8 && board[x][y+1]==0){
		if((y>=7 || judgeDeadPoint(chess,x,y+2)==0)&&
			(x<=0 || judgeDeadPoint(chess,x-1,y+1)==0)&&
			(x>=8 || judgeDeadPoint(chess,x+1,y+1)==0))
			result+=2*chessAround(x,y+1,chess)-chessAround(x,y+1,-chess);
	}
	board[x][y]=0;
	reload(x,y,0);
	return result;
}

step youngAI(int chess)//youngAI，年轻不讲武德
{
	step result;
	result.chess=chess;
	int max=-1000;
	for(int t1=0;t1<9;t1++){
		for(int t2=0;t2<9;t2++){
			if(board[t1][t2]==0 &&
				judgeForbidPoint(chess,t1,t2)==0 && 
					judgeDeadPoint(chess,t1,t2)==0){
				int tmp_value=valueEarlyUse(t1,t2,chess,0);
				//cout << "("<<t1<<","<<t2<<")点的value值为"<<tmp_value<<endl;
				if (max<tmp_value){
					max=tmp_value;
					result.x=t1;
					result.y=t2;
					//cout << "("<<t1<<","<<t2<<")"<<endl;
				}
			}
		}
	}
	return result;
}

step AI(int chess)//AI，采用负极大值算法
{
	step result;
	result.chess=chess;
	int max=-1000;
	for(int t1=0;t1<9;t1++){
		for(int t2=0;t2<9;t2++){
			if(board[t1][t2]==0 &&
				judgeForbidPoint(chess,t1,t2)==0 && 
					judgeDeadPoint(chess,t1,t2)==0){
				int tmp_value=value(t1,t2,chess,0);
				//cout << "("<<t1<<","<<t2<<")点的value值为"<<tmp_value<<endl;
				if (max<tmp_value){
					max=tmp_value;
					result.x=t1;
					result.y=t2;
					//cout << "("<<t1<<","<<t2<<")"<<endl;
				}
			}
		}
	}
	return result;
}

int main(){
	int chess=1;//判断黑or白
	int roundNum=0;
	int shutdown=0;
	int modeNum=0;
	int AINum=0;
	int AINumBlack=0;
	int AINumWhite=0;
	int chess0=1;
	int tips=0;
	cout << "和小黄鸭🦆的不围棋对战"<<endl;
	label0:
	chess=1;roundNum=0;modeNum=0;tmpX=0;tmpX2=0;tmpY=0;tmpY2=0;
	clearBoard(board);
	clearBoard(multiAirBoard);
	clearBoard(tmpboard);
	cout <<"输入数字选择模式："<<endl<<"1 人机对战  2 人人对战  3 观看AI对局"<<endl;
	cin >> modeNum;
	if(modeNum==1){
		cout << "输入选择执黑/执白："<<endl<<"1 黑    2 白"<<endl;
		cin >> chess0;
		chess0=-2*chess0+3;
		cout << "是否开启智能提示功能？"<<endl<<"1 开    2 关"<<endl;
		cin >> tips;
		tips=-tips+2;
		cout << "输入选择AI版本："<<endl<<"1 大黄鸭  2 小黄鸭  3 弹性黄鸭"<<endl;
		cin >> AINum;
	}
	else if(modeNum==3){
		cout << "输入选择黑方AI版本："<<endl<<"1 大黄鸭  2 小黄鸭  3 弹性黄鸭"<<endl;
		cin >> AINumBlack;
		cout << "输入选择白方AI版本："<<endl<<"1 大黄鸭  2 小黄鸭  3 弹性黄鸭"<<endl;
		cin >> AINumWhite;
	}
	else {
		cout << "是否开启智能提示功能？"<<endl<<"1 开    2 关"<<endl;
		cin >> tips;
		tips=-tips+2;
	}
	while(shutdown==0){
		int x=0,y=0; 
		if(modeNum==2 || (modeNum==1 && chess==chess0)){
			label1:
			cout << "请输入坐标（范围1～9，输入0暂停棋局）："<<endl;
			cin>>x;
			if(x==0){
				cout << "棋局暂停..." <<endl;
				char stopNum;
				cout << "1 悔棋  2 继续棋局  3 新游戏  4 结束棋局"<<endl;
				cin>>stopNum;
				if(stopNum=='1'){
					if(modeNum==1){
						board[tmpX][tmpY]=0;
						board[tmpX2][tmpY2]=0;
						reload(tmpX,tmpY,0);
						reload(tmpX2,tmpY2,0);
						int t1=0,t2=0;
						cout <<endl<<"  1 2 3 4 5 6 7 8 9"<<endl;
						for(t1=0;t1<9;t1++){
							cout<<" "<<t1+1;
							for(t2=0;t2<9;t2++){
								if(board[t1][t2]==1) cout<<"⚫️";
								else if(board[t1][t2]==-1) cout<<"⚪️";
								else cout<<"十"; 
							}
							cout << endl;
						}//打印棋盘
						cout << endl;
						roundNum-=2;
						continue;
					}
					else{
						cout << "把("<<tmpX+1<<","<<tmpY+1<<")变为0"<<endl;
						board[tmpX][tmpY]=0;
						reload(tmpX,tmpY,0);
						int t1=0,t2=0;
						cout <<endl<<"  1 2 3 4 5 6 7 8 9"<<endl;
						for(t1=0;t1<9;t1++){
							cout<<" "<<t1+1;
							for(t2=0;t2<9;t2++){
								if(board[t1][t2]==1) cout<<"⚫️";
								else if(board[t1][t2]==-1) cout<<"⚪️";
								else cout<<"十"; 
							}
							cout << endl;
						}//打印棋盘
						cout << endl;
						chess=-chess;
						roundNum--;
						continue;
					}
				}
				else if(stopNum=='2') goto label1;
				else if(stopNum=='3'){
					goto label0;
				}
				else goto stop;
			}
			cin >> y;
			x--;
			y--;
		}
		else if((modeNum==1 && (AINum==2 ||(AINum==3 && roundNum<30)))||
			(modeNum==3 &&((chess==1 && (AINumBlack==2 || (AINumBlack==3 && roundNum<30)))||
				(chess==-1 && (AINumWhite==2||(AINumWhite==3 && roundNum<30)))))){
			clock_t start,finish;
			start=clock();
			step tempAI=youngAI(chess);
			x=tempAI.x;
			y=tempAI.y;
			cout << "("<<x+1<<","<<y+1<<")"<<endl;
			finish=clock();
    		totaltime+=(double)(finish-start)/CLOCKS_PER_SEC;
    		//cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;
    		totaltime=0;
    	}
    	else{
    		clock_t start,finish;
			start=clock();
			step tempAI=AI(chess);
			x=tempAI.x;
			y=tempAI.y;
			cout << "("<<x+1<<","<<y+1<<")"<<endl;
			finish=clock();
    		totaltime+=(double)(finish-start)/CLOCKS_PER_SEC;
    		//cout<<"\n此程序的运行时间为"<<totaltime<<"秒！"<<endl;
    		totaltime=0;
    	}
    	if(x<0 || x>8 || y<0 || y>8 || board[x][y]!=0 ){ 
    		if(modeNum==2 || (modeNum==1 && chess==chess0)){
    			cout << "无效落子!"<<endl;
    			goto label1;
    		}
    		else goto label3;
    	}
		if(judgeDeadPoint(chess,x,y)==1 || judgeForbidPoint(chess,x,y)==1){
			label3:
			if(chess==-1)
				cout << "黑棋胜！"<<endl;
			else
				cout <<"白棋胜！"<<endl;
			goto stop;
		}//非法落子
		board[x][y]=chess;
		tmpX2=tmpX;tmpY2=tmpY;
		tmpX=x;tmpY=y;
		//cout << "tmpX:"<<tmpX<<endl<<"tmpY:"<<tmpY<<endl;
		reload(x,y,chess);
		int t1=0,t2=0;
		int chess1=-chess;
		if(modeNum==1) chess1=chess0;
		system("clear");
		cout <<endl<<"  1 2 3 4 5 6 7 8 9"<<endl;
		for(t1=0;t1<9;t1++){
			cout<<" "<<t1+1;
			for(t2=0;t2<9;t2++){
				
				if(board[t1][t2]==1) cout<<"⚫️";
				else if(board[t1][t2]==-1) cout<<"⚪️";
				
				else if((modeNum==1 || modeNum==2)&&tips==1&& judgeDeadPoint(chess1,t1,t2)==1) {
					if (judgeForbidPoint(chess1,t1,t2)==0)
						cout << "🚫";
					else cout <<"😵";
				}
				else if((modeNum==1 || modeNum==2)&&tips==1&& judgeForbidPoint(chess1,t1,t2)==1)cout << "⛔️";
				
				else cout<<"十"; 
			}
			cout << endl;
		}//打印棋盘
		cout << endl;
		/*
				cout <<endl<<"   1 2 3 4 5 6 7 8 9"<<endl;
		for(t1=0;t1<9;t1++){
			cout<<" "<<t1+1;
			for(t2=0;t2<9;t2++){
				if(board[t1][t2]!=0)
					cout << setw(2)<<right<<multiAirBoard[t1][t2];
				else
					cout << "十";
			}
			cout << endl;
		}

		cout <<endl<<"   1 2 3 4 5 6 7 8 9"<<endl;
		for(t1=0;t1<9;t1++){
			cout<<" "<<t1+1;
			for(t2=0;t2<9;t2++){
				if(board[t1][t2]==0){
					cout << setw(2)<<right<<valueBoard[t1][t2];
				}
				else cout << "十";
			}
			cout << endl;
		}
		*/
		chess=-chess;
		for(t1=0;t1<9;t1++){
			for(t2=0;t2<9;t2++){
				saver[roundNum][t1][t2]=board[t1][t2];
			}
		}
		roundNum++;
	}
	stop:
	cout << "本局结束"<<endl<<"1 复盘  2 再来一局  3 退出"<<endl;
	int finalNum=0;cin >> finalNum;
	if(finalNum==1){
		for(int t3=0;t3<roundNum;t3++){
			sleep(1);
			cout << "第"<<t3+1<<"手："<<endl;
			for(int t4=0;t4<9;t4++){
				for(int t5=0;t5<9;t5++){
					if(saver[t3][t4][t5]==1) cout<<"⚫️";
					else if(saver[t3][t4][t5]==-1) cout<<"⚪️";
					else cout<<"十"; 
				}
				cout << endl;
			}
			cout << endl;
		}
		goto stop;
	}
	else if(finalNum==2) goto label0;
	else return 0;
}