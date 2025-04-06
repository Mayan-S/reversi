#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Old Functions
void printBoard(char board[][26], int n);
bool positionInBounds(int n, int row, int col);
bool checkValidMove(char board[][26], int n, int row, int col, char colour);
bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol);
void applyMove(char board[][26], int n, int row, int col, char colour);

// New Functions
int potentialFlips(char board[][26], int n, int row, int col, char colour);
bool hasValidMove(char board[][26], int n, char colour);
bool boardIsFull(char board[][26], int n);
void countPieces(char board[][26], int n, int *bCount, int *wCount);

// Newer Functions
int makeMove(const char board[][26], int n, char turn, int *row, int *col);
void copyBoard(const char source[][26], char dest[][26], int n);
int evaluateBoard(const char board[][26], int n, char player);
int minimax(char board[][26], int n, char turn, char player, int depth, bool maximizing);

#define INF 1000000
#define SEARCH_DEPTH 3

#include "lab8part2.h"
#include "liblab8part2.h"

int main(void) {
  int n;
  char board[26][26];

  // Read board dimension
  printf("Enter the board dimension: ");
  scanf("%d", &n);

  // Initialize board to 'U'
  for (int x=0; x<n; x++){
      for (int y=0; y<n; y++){
          board[x][y] = 'U';
      }
  }

  // Set initial positions
  board[(n/2) - 1][(n/2) - 1] = 'W';
  board[n/2][n/2] = 'W';
  board[(n/2) - 1][n/2] = 'B';
  board[n/2][(n/2) - 1] = 'B';

  //Ask which colour the computer will play
  char computerColour;
  do{
    printf("Computer plays (B/W) : ");
    scanf(" %c", &computerColour);
  }while(computerColour!='W' && computerColour!='B');

  // Print Initial Board
  printBoard(board, n);

  //Black always goes first
  char currentTurn = 'B';
  bool loop=true;

  //Main Game Loop
  while(loop==true){
    if(boardIsFull(board, n) || (!hasValidMove(board, n, 'B') && !hasValidMove(board, n, 'W'))){
      loop=false;
    }
    else if(!hasValidMove(board, n, currentTurn)){
      printf("%c player has no valid move.\n", currentTurn);
      if(currentTurn=='W'){
        currentTurn='B';
      }
      else{
        currentTurn='W';
      }
    }
    else{
      if(currentTurn == computerColour){
        int chosenRow, chosenCol, dummy=makeMove(board, n, computerColour, &chosenRow, &chosenCol);
        printf("Computer places %c at %c%c.\n", computerColour, 'a' + chosenRow, 'a' + chosenCol);
        applyMove(board, n, chosenRow, chosenCol, computerColour);
        printBoard(board, n);
      }
      else{
        char move[3];
        printf("Enter move for colour %c (RowCol): ", currentTurn);
        scanf("%s", move);
        int moveRow = move[0] - 'a';
        int moveCol = move[1] - 'a';

        if(board[moveRow][moveCol] != 'U' || !checkValidMove(board, n, moveRow, moveCol, currentTurn)){
          printf("Invalid move.\n");
          if(computerColour=='B'){
            printf("B player wins.\n");
          }
          else{
            printf("W player wins.\n");
          }
          return 0;
        }

        applyMove(board, n, moveRow, moveCol, currentTurn);
        printBoard(board, n);
      }

      if(currentTurn=='W'){
        currentTurn='B';
      }
      else{
        currentTurn='W';
      }
    }
  }

  int bCount=0, wCount=0;
  countPieces(board, n, &bCount, &wCount);
  if(bCount>wCount){
    printf("B player wins.\n");
  }
  else if(wCount>bCount){
    printf("W player wins.\n");
  }
  else{
    printf("Draw!\n");
  }

  return 0;
}

//New Functions
int makeMove(const char board[][26], int n, char turn, int *row, int *col){
  int bestScore=-INF;
  int bestRow=-1, bestCol=-1;
  char currentTurn = turn;

  char tempBoard[26][26];
  copyBoard(board, tempBoard, n);

  for(int x=0; x<n; x++){
    for(int y=0; y<n; y++){
      if(tempBoard[x][y]=='U' && checkValidMove(tempBoard, n, x, y, currentTurn)){
        char boardCopy[26][26];
        copyBoard(tempBoard, boardCopy, n);
        applyMove(boardCopy, n, x, y, currentTurn);

        char opponentTurn;
        if(currentTurn=='B'){
          opponentTurn='W';
        }
        else{
          opponentTurn='B';
        }

        int score=minimax(boardCopy, n, opponentTurn, turn, SEARCH_DEPTH-1, false);

        if(score>bestScore){
          bestScore=score;
          bestRow=x;
          bestCol=y;
        }
      }
    }
  }

  *row=bestRow;
  *col=bestCol;
  return bestScore;
}

void copyBoard(const char source[][26], char dest[][26], int n){
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      dest[i][j]=source[i][j];
    }
  }
}

int evaluateBoard(const char board[][26], int n, char player){
  int bCount=0, wCount=0, score=0, bonus=0;
  countPieces((char (*)[26])board, n, &bCount, &wCount);

  if(player=='B'){
    score=(bCount-wCount)*10;
  }
  else{
    score=(wCount-bCount)*10;
  }

  int corners[4][2]={{0,0}, {0, n-1}, {n-1, 0}, {n-1, n-1}};

  for(int i=0; i<4; i++){
    int x=corners[i][0], y=corners[i][1];
    if(board[x][y]==player){
      bonus += 20;
    }
    else if (board[x][y]!='U' && board[x][y]!=player) {
      bonus -= 20;
    }
  }

  return score+bonus;
}

int minimax(char board[][26], int n, char turn, char player, int depth, bool maximizing){
  if(depth==0 || boardIsFull(board, n) || (!hasValidMove(board, n, 'B') && !hasValidMove(board, n, 'W'))){
    return evaluateBoard(board, n, player);
  }

  char opponent;
  if(turn=='B'){
    opponent='W';
  }
  else{
    opponent='B';
  }

  if(maximizing==true){
    int best=-INF;
    bool foundMove=false;

    for(int x=0; x<n; x++){
      for(int y=0; y<n; y++){
        if(board[x][y]=='U' && checkValidMove(board, n, x, y, turn)){
          foundMove=true;
          char boardCopy[26][26];
          copyBoard(board, boardCopy, n);
          applyMove(boardCopy, n, x, y, turn);
          int score=minimax(boardCopy, n, opponent, player, depth-1, false);          
          if(score>best){
            best=score;
          }
        }
      }
    }

    if(!foundMove){
      best = minimax(board, n, opponent, player, depth-1, false);
    }

    return best;
  }
  else{
    int best=INF;
    bool foundMove=false;

    for(int x=0; x<n; x++){
      for(int y=0; y<n; y++){
        if(board[x][y]=='U' && checkValidMove(board, n, x, y, turn)){
          foundMove=true;
          char boardCopy[26][26];
          copyBoard(board, boardCopy, n);
          applyMove(boardCopy, n, x, y, turn);
          int score=minimax(boardCopy, n, opponent, player, depth-1, true);          
          if(score<best){
            best=score;
          }
        }
      }
    }

    if(!foundMove){
      best = minimax(board, n, opponent, player, depth-1, true);
    }

    return best;
  }
}

//Part 1 Functions
int potentialFlips(char board[][26], int n, int row, int col, char colour){
  int totalFlips=0;
  char opponentColour;

  if(colour=='W'){
    opponentColour='B';
  }
  else{
    opponentColour='W';
  }

  for (int deltaRow = -1; deltaRow <= 1; deltaRow++){
    for (int deltaCol = -1; deltaCol <= 1; deltaCol++){
      if(deltaRow!=0 || deltaCol!=0){
        if(checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol)){
          int flips=0;
          int newRow = row + deltaRow;
          int newCol = col + deltaCol;
          while(board[newRow][newCol]==opponentColour && positionInBounds(n, newRow, newCol)){
            flips++;
            newRow+=deltaRow;
            newCol+=deltaCol;
          }
          totalFlips+=flips;
        }
      }
    }
  }

  return totalFlips;
}

bool hasValidMove(char board[][26], int n, char colour){
  for(int row=0; row<n; row++){
    for(int col=0; col<n; col++){
      if(board[row][col]=='U' && checkValidMove(board, n, row, col, colour)){
        return true;
      }
    }
  }

  return false;
}

bool boardIsFull(char board[][26], int n){
  for(int row=0; row<n; row++){
    for(int col=0; col<n; col++){
      if(board[row][col]=='U'){
        return false;
      }
    }
  }

  return true;
}

void countPieces(char board[][26], int n, int *bCount, int *wCount){
  *bCount=0;
  *wCount=0;
  for(int row=0; row<n; row++){
    for(int col=0; col<n; col++){
      if(board[row][col]=='B'){
        (*bCount)++;
      }
      else if(board[row][col]=='W'){
        (*wCount)++;
      }
    }
  }
}

//Old Functions
void printBoard(char board[][26], int n){
  printf("  ");
  for(int x=0; x<n; x++){
      printf("%c", 'a' + x);
  }
  printf("\n");
  for (int x=0; x<n; x++){
      printf("%c ", 'a' + x);
      for (int y=0; y<n; y++){
          printf("%c", board[x][y]);
      }
      printf("\n");
  }
}

bool positionInBounds(int n, int row, int col){
  return (row >= 0 && row < n && col >= 0 && col < n);
}

bool checkValidMove(char board[][26], int n, int row, int col, char colour){
  if (board[row][col] != 'U'){
    return false;
  }

  for (int deltaRow = -1; deltaRow <= 1; deltaRow++){
    for (int deltaCol = -1; deltaCol <= 1; deltaCol++){
      if(deltaRow != 0 || deltaCol != 0){
        if (checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol)){
          return true;
        }
      }
    }
  }

  return false;
}

bool checkLegalInDirection(char board[][26], int n, int row, int col, char colour, int deltaRow, int deltaCol){
  int newRow = row + deltaRow;
  int newCol = col + deltaCol;

  char opponentColour;
  if(colour=='W'){
    opponentColour='B';
  }
  else{
    opponentColour='W';
  }

  if(!positionInBounds(n, newRow, newCol) || board[newRow][newCol] != opponentColour){
    return false;
  }

  newRow += deltaRow;
  newCol += deltaCol;

  while(positionInBounds(n, newRow, newCol)){
    if(board[newRow][newCol]=='U'){
      return false;
    }
    if(board[newRow][newCol]==colour){
      return true;
    }
    newRow += deltaRow;
    newCol += deltaCol;
  }
  return false;
}

void applyMove(char board[][26], int n, int row, int col, char colour){
  board[row][col]=colour;

  for (int deltaRow = -1; deltaRow <= 1; deltaRow++){
    for (int deltaCol = -1; deltaCol <= 1; deltaCol++){
      if(deltaRow != 0 || deltaCol != 0){
        if (checkLegalInDirection(board, n, row, col, colour, deltaRow, deltaCol)){
          int newRow = row + deltaRow;
          int newCol = col + deltaCol;

          char opponentColour;
          if(colour=='W'){
            opponentColour='B';
          }
          else{
            opponentColour='W';
          }

          while(positionInBounds(n, newRow, newCol) && board[newRow][newCol] == opponentColour){
            board[newRow][newCol]=colour;
            newRow += deltaRow;
            newCol += deltaCol;
          }
        }
      }
    }
  }
}
