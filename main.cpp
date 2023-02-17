#include<iostream>
#include<vector>
#include<queue>
#include<unordered_map>
#include<unordered_set>
#include"opencv2/highgui.hpp"
#include<opencv2/imgproc.hpp>
#include<string>

using namespace std;
using namespace cv;

#define N 3
#define TILE_SIZE 200
#define PLAY_SPEED 20 //in ms
#define SIMULATION_SPEED 4

Mat DisplayText(vector<string> lists){
    Mat Display(Size(3 * TILE_SIZE, 3 * TILE_SIZE), CV_8UC3, Scalar(250, 250, 250));
    int pad= TILE_SIZE / 5;
    double fontScale= TILE_SIZE / 150.0;
    Size TextSize= getTextSize(lists[0],FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),nullptr);
    int x1,y1= (3 * TILE_SIZE - lists.size() * (TextSize.height + pad)) / 2 ;
    y1-=pad/2;
    for(const auto & list : lists){
        TextSize= getTextSize(list,FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),nullptr);
        y1+=(TextSize.height+pad);
        x1= (3 * TILE_SIZE - TextSize.width) / 2;
        putText(
            Display,
            list,
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
    explicit Board(vector<vector<int> >& board){
        mat = board;
        pos = findTile(mat);
    }
    explicit Board (Board* board){
        mat = board->mat;
        pos = board->pos;
    }
};

void drawLine( Mat img, Point start, Point end ){
    int thickness = (int) TILE_SIZE / 75.0;
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
    double fontScale = TILE_SIZE / 50.0;
    Scalar color = (a!=0? Scalar(250,250,250): Scalar(200,200,200) );
    rectangle(img,
        start,
        end,
        color,
        FILLED,
        LINE_8 );
    if(a!=0&&a!=-1){
        Size textSize = getTextSize(to_string(a),FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),nullptr);
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

void putLine(const Mat& img){
    drawLine(img, Point(TILE_SIZE, 0), Point(TILE_SIZE, 3 * TILE_SIZE));
    drawLine(img, Point(2 * TILE_SIZE, 0), Point(2 * TILE_SIZE, 3 * TILE_SIZE));
    drawLine(img, Point(0, TILE_SIZE), Point(3 * TILE_SIZE, TILE_SIZE));
    drawLine(img, Point(0, 2 * TILE_SIZE), Point(3 * TILE_SIZE, 2 * TILE_SIZE));

}

Mat createBoard(vector<vector<int> >& mat){
    Mat Board(Size(N * TILE_SIZE, N * TILE_SIZE), CV_8UC3, Scalar(255, 255, 255));
    for(int i=0;i<mat.size();i++){
        for(int j=0;j<mat[0].size();j++){
            int x1= j * TILE_SIZE;
            int y1= i * TILE_SIZE;
            int x2= x1 + TILE_SIZE;
            int y2= y1 + TILE_SIZE;
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
    Mat Board(Size(N * TILE_SIZE, N * TILE_SIZE), CV_8UC3, Scalar(255, 255, 255));
    for(int i=0;i<prev->mat.size();i++){
        for(int j=0;j<prev->mat[0].size();j++){
            int x1= i * TILE_SIZE;
            int y1= j * TILE_SIZE;
            int x2= x1 + TILE_SIZE;
            int y2= y1 + TILE_SIZE;
            if(i==curr->pos.second &&j==curr->pos.first)
                    drawTile(Board,Point(x1,y1),Point(x2,y2),curr->mat[j][i]);
            else
            drawTile(Board,Point(x1,y1),Point(x2,y2),prev->mat[j][i]);
        }
    }
    int del_y=(prev->pos.first - curr->pos.first);
    int del_x=(prev->pos.second - curr->pos.second);
    int curr_x1= curr->pos.second * TILE_SIZE;
    int curr_y1= curr->pos.first * TILE_SIZE;
    int curr_x2= curr_x1 + TILE_SIZE;
    int curr_y2= curr_y1 + TILE_SIZE;
    for(int i=0; i < TILE_SIZE; i= i + 1 * speed){
        Mat Transition=Board.clone();
        drawTile(Transition, Point(curr_x1 + del_x * i, curr_y1 + del_y * i), Point(curr_x2 + del_x * i, curr_y2 + del_y * i), prev->mat[curr->pos.first][curr->pos.second]);
        putLine(Transition);
        imshow("Board",Transition);
        waitKey(1);
    }
    drawBoard(curr->mat);
}

Board* getBoard(const string& info,const string& gameType){
    vector<string> Instructions{
        "",
        "Instructions:",
        "Fill tiles 1-8",
        "Use \'0\' for blank",
    };
    if(!info.empty())
        Instructions.insert(Instructions.begin(),"Input \'"+info+"\' Board");
    if(!info.empty())
        Instructions.insert(Instructions.begin(),"Mode: "+gameType);
    Instructions.emplace_back("Press key to continue!");
    imshow("Board",DisplayText(Instructions));
    waitKey();
    vector<vector<int> > mat(N,vector<int>(N,-1));
    int itr=0;
    unordered_set<int> st;
    while(itr<N*N){
        drawBoard(mat);
        if(st.empty()){
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

	TileNode(Board* Prev, pair<int,int> newPos, int level, vector<vector<int> >& Final){
        parent= nullptr;
        B= new Board(Prev);
        swap(B->mat[B->pos.first][B->pos.second], B->mat[newPos.first][newPos.second]);
        cost = calculateCost(B->mat,Final);
        level = level;
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
    int xo1= j * TILE_SIZE;
    int yo1= i * TILE_SIZE;
    int xo2= xo1 + TILE_SIZE;
    int yo2= yo1 + TILE_SIZE;
    for(auto muv:curr->moves){
        int i=curr->B->pos.first+row[muv.first];
        int j=curr->B->pos.second+col[muv.first];
        int x1= j * TILE_SIZE;
        int y1= i * TILE_SIZE;
        int x2= x1 + TILE_SIZE;
        int y2= y1 + TILE_SIZE;
        double fontScale= TILE_SIZE / 250.0;
        int lineType = LINE_8;
        string text =""+ to_string(muv.second.first)+ "-"+to_string(muv.second.second)+"";
        int x_min= min(xo1, x1);
        int y_min= min(yo1, y1);
        int x_max= max(xo2, x2);
        int y_max= max(yo2, y2);
        Size textSize = getTextSize(text,FONT_HERSHEY_DUPLEX,fontScale,(int)ceil(fontScale),nullptr);
        int textX = ((int)x_max - (int)textSize.width + x_min) / 2;
        int textY = ((int)y_max + (int)textSize.height + y_min) / 2;
        x2=textX+textSize.width;
        y2=textY-textSize.height;
        int pad=(int)ceil(TILE_SIZE / 30);
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
        int x1= j * TILE_SIZE;
        int y1= i * TILE_SIZE;
        int x2= x1 + TILE_SIZE;
        int y2= y1 + TILE_SIZE;
        //RECTANGLE
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
        double fontScale = TILE_SIZE / 50.0;
        Size textSize = getTextSize(to_string(curr->B->mat[i][j]), FONT_HERSHEY_DUPLEX, fontScale, (int)fontScale, NULL);
        int textX = ((int)x2 - (int)textSize.width+x1)/2;
        int textY = ((int)y2 + (int)textSize.height+y1)/2;
        putText(Board,
                to_string(curr->B->mat[i][j]),
                Point(textX,textY),
                FONT_HERSHEY_DUPLEX,
                fontScale,
                Scalar(0,0,0),
                (int)fontScale,
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
    while(root!=nullptr){
        path.push_back(root);
        root=root->parent;
    }
    for(int i= path.size() -1;i>0;i--){
        showMoves(path[i],path[i-1]->moveTaken);
        animateBoard(path[i]->B, path[i-1]->B, SIMULATION_SPEED);
        waitKey(100);
    }
    drawBoard(path[0]->B->mat);
    waitKey(1000);
    imshow("Board",DisplayText({"Finished","","Press any key!!"}));
    waitKey();
}

void solve(Board* C, Board* F){
	priority_queue<TileNode*, std::vector<TileNode*>, comp> pq;
	auto* root = new TileNode(C, C->pos, 0,F->mat);
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
				auto* child = new TileNode(min->B, make_pair(min->B->pos.first + row[i],min->B->pos.second + col[i]), min->level + 1,F->mat);
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

bool isFinished(Board* C, Board* F){
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
    auto move = moves.find(ch);
    if(move==moves.end())
        return;
    if(isSafe(C->pos.first + row[move->second],C->pos.second + col[move->second])){
        swap(C->mat[C->pos.first][C->pos.second],C->mat[C->pos.first + row[move->second]][C->pos.second + col[move->second]]);
        C->pos.first += row[move->second];
        C->pos.second += col[move->second];
    }
}

void play(Board* game,Board* goal){
    while(!isFinished(game, goal)){
        drawBoard(game->mat);
        char choice=waitKey();
        auto* prev= new Board(game);
        moveTile(game, goal, choice);
        animateBoard(prev, game, PLAY_SPEED);
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
    bool flag=true;
    while(flag){
        imshow("Board",DisplayText(options));
        choice = waitKey();
        switch (choice)
        {
        case '1':
            PlayPuzzle();
            break;
        case '2':
            SolvePuzzle();
            break;
        case '3':
            flag=false;
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
