#include<iostream>
#include<vector>
#include<queue>
#include<conio.h>
#include<unordered_map>
#include<map>
#include<unordered_set>
#include"opencv2/highgui.hpp"
#include<opencv2/imgproc.hpp>
#include<stdint.h>
#include<string>

using namespace std;
using namespace cv;

#define N 3
#define TILESIZE 200
#define PLAYSPEED 20 //in ms
#define SIMULATIONSPEED 4

Mat DisplayText(vector<string> lists){
    Mat Display(Size(3*TILESIZE,3*TILESIZE),CV_8UC3,Scalar(250,250,250));
    int pad=TILESIZE/5;
    double fontScale=TILESIZE/150.0;
    Size TextSize= getTextSize(lists[0],FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),NULL);
    int x1,y1= (3*TILESIZE - lists.size()*(TextSize.height+pad))/2 ;
    y1-=pad/2;
    for(int i=0;i<lists.size();i++){
        TextSize= getTextSize(lists[i],FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),NULL);
        y1+=(TextSize.height+pad);
        x1=(3*TILESIZE-TextSize.width)/2;
        putText(
            Display,
            lists[i],
            Point(x1,y1),
            FONT_HERSHEY_DUPLEX,
            fontScale,
            Scalar(0,0,0),
            (int)ceil(fontScale),
            LINE_8);
    }
    return Display;
}

int row[] = {
    1, 0, -1, 0
};
int col[] = {
    0, -1, 0, 1
};

int isSafe(int x, int y) {
	return (x >= 0 && x < N && y >= 0 && y < N);
}

pair<int,int> findTile(vector<vector<int> >& mat ){
    for (int i=0;i<mat.size();i++)
        for (int j=0;j<mat[0].size();j++)
            if (mat[i][j]==0)
                return make_pair(i,j);
    return make_pair(-1,-1);
}

struct Board{
    vector<vector<int> > mat;
	pair<int,int> pos;//x and y
    Board(vector<vector<int> >& board){
        mat=board;
        pos=findTile(mat);
    }
    Board (Board* board){
        mat=board->mat;
        pos=board->pos;
    }
};

void drawLine( Mat img, Point start, Point end ){
    int thickness = (int)TILESIZE/75.0;
    int lineType = LINE_8;
    line( img,
       start,
       end,
       Scalar( 0, 0, 0 ),
       thickness,
       lineType );
}

void drawTile( Mat img, Point start, Point end, int a){
    int thickness = 2;
    int lineType = LINE_8;
    double fontScale = TILESIZE/50.0;
    Scalar color = (a!=0? Scalar(250,250,250): Scalar(200,200,200) );
    rectangle(img,
        start,
        end,
        color,
        FILLED,
        LINE_8 );
    if(a!=0&&a!=-1){
        Size textSize = getTextSize(to_string(a),FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),NULL);
        int textX = ((int)end.x - (int)textSize.width+start.x)/2;
        int textY = ((int)end.y + (int)textSize.height+start.y)/2;
        putText(img,
            to_string(a),
            Point(textX,textY),
            FONT_HERSHEY_DUPLEX,
            fontScale,
            Scalar(0,0,0),
            (int)ceil(fontScale),
            lineType);
    }
}

void putLine(Mat img){
    drawLine(img,Point(TILESIZE,0),Point(TILESIZE,3*TILESIZE));
    drawLine(img,Point(2*TILESIZE,0),Point(2*TILESIZE,3*TILESIZE));
    drawLine(img,Point(0,TILESIZE),Point(3*TILESIZE,TILESIZE));
    drawLine(img,Point(0,2*TILESIZE),Point(3*TILESIZE,2*TILESIZE));

}

Mat createBoard(vector<vector<int> >& mat){
    Mat Board(Size(N*TILESIZE,N*TILESIZE),CV_8UC3,Scalar(255,255,255));
    for(int i=0;i<mat.size();i++){
        for(int j=0;j<mat[0].size();j++){
            int x1=j*TILESIZE;
            int y1=i*TILESIZE;
            int x2=x1+TILESIZE;
            int y2=y1+TILESIZE;
            drawTile(Board,Point(x1,y1),Point(x2,y2),mat[i][j]);
        }
    }
    return Board;
}

void drawBoard(vector<vector<int> >& mat){
    Mat Board= createBoard(mat);
    putLine(Board);
    imshow("Board",Board);
}

void animateBoard(Board* prev,Board* curr,int speed){
    drawBoard(prev->mat);
    Mat Board(Size(N*TILESIZE,N*TILESIZE),CV_8UC3,Scalar(255,255,255));
    for(int i=0;i<prev->mat.size();i++){
        for(int j=0;j<prev->mat[0].size();j++){
            int x1=i*TILESIZE;
            int y1=j*TILESIZE;
            int x2=x1+TILESIZE;
            int y2=y1+TILESIZE;
            if(i==curr->pos.second &&j==curr->pos.first)
                    drawTile(Board,Point(x1,y1),Point(x2,y2),curr->mat[j][i]);
            else
            drawTile(Board,Point(x1,y1),Point(x2,y2),prev->mat[j][i]);
        }
    }
    int dely=(prev->pos.first-curr->pos.first);
    int delx=(prev->pos.second-curr->pos.second);
    int currx1=curr->pos.second *TILESIZE;
    int curry1=curr->pos.first *TILESIZE;
    int currx2=currx1+TILESIZE;
    int curry2=curry1+TILESIZE;
    for(int i=0;i<TILESIZE;i=i+1*speed){
        Mat Transition=Board.clone();
        drawTile(Transition,Point(currx1+delx*i,curry1+dely*i),Point(currx2+delx*i,curry2+dely*i),prev->mat[curr->pos.first][curr->pos.second]);
        putLine(Transition);
        imshow("Board",Transition);
        waitKey(1);
    }
    drawBoard(curr->mat);
}

Board* getBoard(string info,string gameType){
    vector<string> Instructions{
        "",
        "Instructions:",
        "Fill tiles 1-8",
        "Use \'0\' for blank",
    };
    if(info.size()!=0)
        Instructions.insert(Instructions.begin(),"Input \'"+info+"\' Board");
    if(info.size()!=0)
        Instructions.insert(Instructions.begin(),"Mode: "+gameType);
    Instructions.push_back("Press key to continue!");
    imshow("Board",DisplayText(Instructions));
    waitKey();
    vector<vector<int> > mat(N,vector<int>(N,-1));
    int itr=0;
    unordered_set<int> st;
    while(itr<N*N){
        drawBoard(mat);
        if(st.size()==0){
            Mat toast=DisplayText({"Enter",info});
            putLine(toast);
            imshow("Board",toast);
        }
        int choice= waitKey()-'0';
        if(choice==-40){
            if(itr==0)
                continue;
            itr--;
            st.erase(st.find(mat[itr/N][itr%N]));
            mat[itr/N][itr%N]=-1;
            continue;
        }
        if(choice<0||choice>8)
            continue;
        if(st.find(choice)==st.end()){
            st.insert(choice);
            mat[itr/N][itr%N]=choice;
            itr++;
        }
    }
    drawBoard(mat);
    waitKey();
    return new Board(mat);
}

int calculateCost(vector<vector<int> >& initial, vector<vector<int> >& Final) {
	int count=0;
	for (int i=0;i<initial.size();i++)
	    for (int j=0;j<initial[0].size();j++)
		    if (initial[i][j] && initial[i][j] != Final[i][j])
		        count++;
	return count;
}

struct TileNode{
	TileNode* parent;
	Board* B;
    int cost;
	int level;
	int moveTaken=-1;
	unordered_map<int,pair<int,int>> moves;//moves and their cost,level

	TileNode(Board* Prev, pair<int,int> newPos, int levell, vector<vector<int> >& Final){
        parent= NULL;
        B= new Board(Prev);
        swap(B->mat[B->pos.first][B->pos.second], B->mat[newPos.first][newPos.second]);
        cost = calculateCost(B->mat,Final);
        level = levell;
        B->pos=newPos;
        moveTaken=-1;
    }
    ~TileNode(){
        delete B;
    }
};

void putWeights(Mat Board,TileNode* curr){
    int i=curr->B->pos.first;
    int j=curr->B->pos.second;
    int xo1=j*TILESIZE;
    int yo1=i*TILESIZE;
    int xo2=xo1+TILESIZE;
    int yo2=yo1+TILESIZE;
    for(auto muv:curr->moves){
        int i=curr->B->pos.first+row[muv.first];
        int j=curr->B->pos.second+col[muv.first];
        int x1=j*TILESIZE;
        int y1=i*TILESIZE;
        int x2=x1+TILESIZE;
        int y2=y1+TILESIZE;
        //PUT weigths
        double fontScale= TILESIZE/250.0;
        int lineType = LINE_8;
        string text =""+ to_string(muv.second.first)+ "-"+to_string(muv.second.second)+"";
        int xmin= min(xo1,x1);
        int ymin= min(yo1,y1);
        int xmax= max(xo2,x2);
        int ymax= max(yo2,y2);
        Size textSize = getTextSize(text,FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),NULL);
        int textX = ((int)xmax - (int)textSize.width+xmin)/2;
        int textY = ((int)ymax + (int)textSize.height+ymin)/2;
        x2=textX+textSize.width;
        y2=textY-textSize.height;
        int pad=(int)ceil(TILESIZE/30);
        rectangle(Board,
            Point(textX-pad,y2-pad),
            Point(x2+pad,textY+pad),
            Scalar(250,250,250),
            FILLED,
            LINE_8 );
        putText(Board,
            text,
            Point(textX,textY),
            FONT_HERSHEY_DUPLEX,
            fontScale,
            Scalar(0,0,0),
            (int)ceil(fontScale),
            lineType);
    }


}

void showMoves(TileNode* curr,int moveTaken){
    Mat Board = createBoard(curr->B->mat);
    for(auto muv:curr->moves){
        int i=curr->B->pos.first+row[muv.first];
        int j=curr->B->pos.second+col[muv.first];
        int x1=j*TILESIZE;
        int y1=i*TILESIZE;
        int x2=x1+TILESIZE;
        int y2=y1+TILESIZE;
        //RECANGLE
        int thickness = 2;
        int lineType = LINE_8;
        Scalar color = (muv.first==moveTaken ? Scalar(150,250,150): Scalar(150,150,250) );
        rectangle(Board,
            Point(x1,y1),
            Point(x2,y2),
            color,
            FILLED,
            LINE_8 );
        //TILE NUMBER
        double fonscale = TILESIZE/50.0;
        Size textSize = getTextSize(to_string(curr->B->mat[i][j]),FONT_HERSHEY_DUPLEX,fonscale,(int)fonscale,NULL);
        int textX = ((int)x2 - (int)textSize.width+x1)/2;
        int textY = ((int)y2 + (int)textSize.height+y1)/2;
        putText(Board,
            to_string(curr->B->mat[i][j]),
            Point(textX,textY),
            FONT_HERSHEY_DUPLEX,
            fonscale,
            Scalar(0,0,0),
            (int)fonscale,
            lineType);
    }
    putLine(Board);
    putWeights(Board,curr);
    imshow("Board",Board);
    waitKey();

}

struct comp {
	bool operator()(const TileNode* lhs, const TileNode* rhs) const{
		return (lhs->cost + lhs->level) > (rhs->cost + rhs->level);
	}
};

void simulatePath(TileNode* root){
    vector<TileNode*> path;
    while(root!=NULL){
        path.push_back(root);
        root=root->parent;
    }
    for(int i=path.size()-1;i>0;i--){
        showMoves(path[i],path[i-1]->moveTaken);
        animateBoard(path[i]->B,path[i-1]->B,SIMULATIONSPEED);
        waitKey(100);
    }
    drawBoard(path[0]->B->mat);
    waitKey(1000);
    imshow("Board",DisplayText({"Finished","","Press any key!!"}));
    waitKey();
}

void solve(Board* C, Board* F){
	priority_queue<TileNode*, std::vector<TileNode*>, comp> pq;
	TileNode* root = new TileNode(C, C->pos, 0,F->mat);
	pq.push(root);
	while (!pq.empty()) {
		TileNode* min = pq.top();
		pq.pop();
		if (min->cost == 0){
			simulatePath(min);
			break;
		}
		for (int i=0; i<4; i++){
			if (isSafe(min->B->pos.first + row[i], min->B->pos.second + col[i])){
				TileNode* child = new TileNode(min->B, make_pair(min->B->pos.first + row[i],min->B->pos.second + col[i]), min->level + 1,F->mat);
				child->parent=min;
				child->moveTaken=i;
				pq.push(child);
				min->moves[i]= make_pair(child->cost,child->level);
			}
		}
	}
	while(!pq.empty()){
        TileNode* temp = pq.top();
        pq.pop();
        delete temp;
	}
	delete C;
	delete F;
}

void SolvePuzzle(){
    Board *F= getBoard("Target","Solve");
    Board *I= getBoard("Initial","Solve");
	solve(I,F);

}

bool isfinished(Board* C, Board* F){
    for(int i=0;i<C->mat.size();i++)
        for(int j=0;j<C->mat[i].size();j++)
            if(C->mat[i][j]!=F->mat[i][j])
                return false;
    return true;
}

unordered_map<char,int > moves {
    {'w',2},
    {'a',1},
    {'s',0},
    {'d',3},
};

void moveTile(Board *C,Board *F, char ch){
    unordered_map<char,int > ::iterator move = moves.find(ch);
    if(move==moves.end())
        return;
    if(isSafe(C->pos.first + row[move->second],C->pos.second + col[move->second])){
        swap(C->mat[C->pos.first][C->pos.second],C->mat[C->pos.first + row[move->second]][C->pos.second + col[move->second]]);
        C->pos.first += row[move->second];
        C->pos.second += col[move->second];
    }
}

void play(Board* game,Board* goal){
    while(!isfinished(game,goal)){
        drawBoard(game->mat);
        char choice=waitKey();
        Board* prev= new Board(game);
        moveTile(game, goal, choice);
        animateBoard(prev,game,PLAYSPEED);
        if(choice=='3')
            break;
    }
    drawBoard(game->mat);
    waitKey(1000);
    imshow("Board",DisplayText({"Finished","","Press any key!!"}));
    waitKey();
    delete game;
    delete goal;
}

void PlayPuzzle(){
    Board* goal=getBoard("Target","Play");
    Board* game=getBoard("Initial","Play");
    imshow("Board",DisplayText({"Use W A S D ","to move blank"," "," "," Press key to continue!!"}));
    waitKey();
    play(game,goal);
}

int main() {
    vector<string> options{
        "8 TilePuzzle!!",
        " ",
        "Press \'1\' to Play",
        "Press \'2\' to Solve",
        "Press \'3\' to Exit"
    };
    char choice;
    bool flag=1;
    while(flag){
        imshow("Board",DisplayText(options));
        choice = waitKey();
        //cout<<(int)choice;
        switch (choice)
        {
        case '1':
            PlayPuzzle();
            break;
        case '2':
            SolvePuzzle();
            break;
        case '3':
            flag=0;
            break;
        default:
            cout<<"Enter valid input"<<endl;
        }
    }/*
    vector<vector<int> >  Initial{
		{2, 8, 3},
		{1, 6, 4},
		{7, 0, 5}
	};
	vector<vector<int> >  Final{
		{1, 2, 3},
		{8, 0, 4},
		{7, 6, 5}
	};
	play(new Board(Initial),new Board(Final));*/
	return 0;
}

/*
vector<vector<int> >  Initial{
		{2, 8, 3},
		{1, 6, 4},
		{7, 0, 5}
	};
	vector<vector<int> >  Final{
		{1, 2, 3},
		{8, 0, 4},
		{7, 6, 5}
	};
	*/
