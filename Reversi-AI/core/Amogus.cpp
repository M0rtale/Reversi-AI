
// MyOthelloAI.cpp
//
// some minimax search stuff

#include <ics46/factory/DynamicFactory.hpp>
#include "Amogus.hpp"
#include <cmath>
//#include <iostream>
//#include <assert.h>

//since this isn't redily avaliable, 
//set this a constant and we work on this in the 
//training
const int ENEMY_MOVE_COUNT = 10;
//modify this so it doesn't take too long to make a move
const int MAX_DEPTH = 4;
//using such cutoff, we can cut the games into 2 stages,
//the initial stage where we are binding everything as tight as possible
//the second where we start to make a move for the edges
//60 - 41 = 19
//when it is 40/60 score
const float SCORE_RUSHB = 19;
//60 - 56 = 4
//when it is 55/60 score
const float FINAL_PUSH = 4;
//60 - 61 = 0
//when it is 60/60 score
const float FINAL_FINAL_PUSH = -1;
//was 12
//const int DEPTH_CUTOFF = 12;

ICS46_DYNAMIC_FACTORY_REGISTER(OthelloAI, fengkaiq::Amogus, "(head, eyes)");

//returns a vector of possible moves
std::vector<std::pair<int, int>> fengkaiq::Amogus::getPossibleMoves(const OthelloGameState& state, bool enemy)
{
    std::vector<std::pair<int, int>> moves{};
    for(int i = 0; i < state.board().width(); i++)
    {
        for(int j = 0; j < state.board().height(); j++)
        {
            // if(!enemy)
            // {
            //     assert(state.isValidMove(i, j) == isValidMove(state, i,j, enemy));
            // }
            if(isValidMove(state, i,j, enemy))moves.push_back(std::pair<int, int>(i,j));
        }
    }
    return moves;
}

//returns how much such move evaluates.
//1-(x^(2)+y^(2))
float fengkaiq::Amogus::CenterEval(const OthelloGameState& state, int x, int y)
{
    //first, create a 2d Vector from the center of the board, normalize the vector
    //float xDir = x - float(4) + 0.5;
    //float yDir = y - float(4) + 0.5;
     float xDir = x - float(state.board().width() / 2) + 0.5;
     float yDir = y - float(state.board().height() / 2) + 0.5;
    float length = sqrt(float((state.board().width() / 2) * (state.board().width() / 2) + (state.board().height() / 2) * (state.board().height() / 2)));
    float xDirNormal = (float)xDir / length;
    float yDirNormal = (float)yDir / length;
    //std::cout << xDirNormal << " " << yDirNormal << std::endl;
    //then, find f(x,y) = 1-(x^(2)+y^(2))
    return 1.f - (xDirNormal * xDirNormal + yDirNormal * yDirNormal);
}

//evaluates current state's winning-ness, larger the number the more desired this state is
int fengkaiq::Amogus::evaluate_winning(const OthelloGameState& state)
{
    std::vector<std::pair<int, int>> myMoves = getPossibleMoves(state, false);
    std::vector<std::pair<int, int>> enemyMoves = getPossibleMoves(state, true);
    int myscore = state.isBlackTurn() ? state.blackScore() : state.whiteScore();
    int enemyscore = state.isBlackTurn() ? state.whiteScore() : state.blackScore();

    //get the number of corner moves
    int myCornetMoves = 0;
    for(std::pair<int, int> move : myMoves)
    {
        //top right
        if(move.first == state.board().width()-1 && move.second == 0) myCornetMoves++;
        //bottom left
        if(move.first == 0 && move.second == state.board().height()-1) myCornetMoves++;
        //bottom right
        if(move.first == state.board().width()-1 && move.second == state.board().height()-1) myCornetMoves++;
        //top left
        if(move.first == 0 && move.second == 0) myCornetMoves++;
    }

    //get the number of corner moves
    int enemyCornetMoves = 0;
    for(std::pair<int, int> move : enemyMoves)
    {
        //top right
        if(move.first == state.board().width()-1 && move.second == 0) enemyCornetMoves++;
        //bottom left
        if(move.first == 0 && move.second == state.board().height()-1) enemyCornetMoves++;
        //bottom right
        if(move.first == state.board().width()-1 && move.second == state.board().height()-1) enemyCornetMoves++;
        //top left
        if(move.first == 0 && move.second == 0) enemyCornetMoves++;
    }

    int mycornerCount = 0;
    int enemycornerCount = 0;
    int myCorneringCount = 0;
    for(int i = 0; i < state.board().width(); i+=state.board().width()-1)
    {
        for(int j = 0; j < state.board().height(); j+=state.board().height()-1)
        {
            if(state.isBlackTurn() && state.board().cellAt(i,j) == OthelloCell::black)mycornerCount++;
            if(state.isBlackTurn() && state.board().cellAt(i,j) == OthelloCell::white)enemycornerCount++;
            if(state.isWhiteTurn() && state.board().cellAt(i,j) == OthelloCell::black)enemycornerCount++;
            if(state.isWhiteTurn() && state.board().cellAt(i,j) == OthelloCell::white)mycornerCount++;
        }
    }
    // if(state.isBlackTurn() && state.board().cellAt(0,1) == OthelloCell::black) myCorneringCount++;
    // if(state.isWhiteTurn() && state.board().cellAt(0,1) == OthelloCell::white)myCorneringCount++;
    // if(state.isBlackTurn() && state.board().cellAt(1,0) == OthelloCell::black) myCorneringCount++;
    // if(state.isWhiteTurn() && state.board().cellAt(1,0) == OthelloCell::white)myCorneringCount++;
    if(state.isBlackTurn() && state.board().cellAt(1,1) == OthelloCell::black) myCorneringCount++;
    if(state.isWhiteTurn() && state.board().cellAt(1,1) == OthelloCell::white)myCorneringCount++;
    //if(state.isBlackTurn() && state.board().cellAt(0,state.board().height()-2) == OthelloCell::black) myCorneringCount++;
    //if(state.isWhiteTurn() && state.board().cellAt(0,state.board().height()-2) == OthelloCell::white)myCorneringCount++;
    if(state.isBlackTurn() && state.board().cellAt(1,state.board().height()-2) == OthelloCell::black) myCorneringCount++;
    if(state.isWhiteTurn() && state.board().cellAt(1,state.board().height()-2) == OthelloCell::white)myCorneringCount++;
    //if(state.isBlackTurn() && state.board().cellAt(1,state.board().height()-1) == OthelloCell::black) myCorneringCount++;
    //if(state.isWhiteTurn() && state.board().cellAt(1,state.board().height()-1) == OthelloCell::white)myCorneringCount++;
    //if(state.isBlackTurn() && state.board().cellAt(state.board().width()-2,0) == OthelloCell::black) myCorneringCount++;
    //if(state.isWhiteTurn() && state.board().cellAt(state.board().width()-2,0) == OthelloCell::white)myCorneringCount++;
    if(state.isBlackTurn() && state.board().cellAt(state.board().width()-2,1) == OthelloCell::black) myCorneringCount++;
    if(state.isWhiteTurn() && state.board().cellAt(state.board().width()-2,1) == OthelloCell::white)myCorneringCount++;
    //if(state.isBlackTurn() && state.board().cellAt(state.board().width()-1,1) == OthelloCell::black) myCorneringCount++;
    //if(state.isWhiteTurn() && state.board().cellAt(state.board().width()-1,1) == OthelloCell::white)myCorneringCount++;
    if(state.isBlackTurn() && state.board().cellAt(state.board().width()-2,state.board().height()-2) == OthelloCell::black) myCorneringCount++;
    if(state.isWhiteTurn() && state.board().cellAt(state.board().width()-2,state.board().height()-2) == OthelloCell::white)myCorneringCount++;
    //if(state.isBlackTurn() && state.board().cellAt(state.board().width()-2,state.board().height()-1) == OthelloCell::black) myCorneringCount++;
    //if(state.isWhiteTurn() && state.board().cellAt(state.board().width()-2,state.board().height()-1) == OthelloCell::white)myCorneringCount++;
    //if(state.isBlackTurn() && state.board().cellAt(state.board().width()-1,state.board().height()-2) == OthelloCell::black) myCorneringCount++;
    //if(state.isWhiteTurn() && state.board().cellAt(state.board().width()-1,state.board().height()-2) == OthelloCell::white)myCorneringCount++;

    //give a final move evaluation to our moves
    float totalMoves = 0;
    for(auto move : myMoves)
    {
        //std::cout << CenterEval(state, move.first, move.second) << std::endl;
        totalMoves += CenterEval(state, move.first, move.second);
    }
    int iTotalMoves = (int)totalMoves;
    //std::cout << iTotalMoves << " " << myMoves.size() << std::endl;

    //give a final move evaluation to Enemy moves
    // float totalMovesEnemy = 0;
    // for(auto move : enemyMoves)
    // {
    //     std::cout << CenterEval(state, move.first, move.second) << std::endl;
    //     totalMovesEnemy += CenterEval(state, move.first, move.second);
    // }
    // int itotalMovesEnemy = (int)totalMovesEnemy;

    //it should scale nicely to the current situation
    //todo: movesEvaluation is returning a very random number.
    int edgeEvaluation = 6 * iTotalMoves;
    //int movesEvaluation = 500 * ((int)(unsigned int)myMoves.size() - (int)(unsigned int)enemyMoves.size()) / ((int)(unsigned int)myMoves.size() + (int)(unsigned int)enemyMoves.size()) == 0 ? 1 : ((int)(unsigned int)myMoves.size() + (int)(unsigned int)enemyMoves.size());
    int movesEvaluation = 500 * ((int)(unsigned int)myMoves.size() - ENEMY_MOVE_COUNT) / ((int)(unsigned int)myMoves.size() + ENEMY_MOVE_COUNT);
    //int movesEvaluation = 0 + ENEMY_MOVE_COUNT - ENEMY_MOVE_COUNT;
    int scoreEvaluation = 0 * (myscore - enemyscore) / (myscore + enemyscore);
    //int cornerEvaluation = 0;
    //int cornerEvaluation = 1000 * mycornerCount;
    int cornerEvaluation = 1000 * (mycornerCount - enemycornerCount) / ((mycornerCount + enemycornerCount) == 0 ? 1 : (mycornerCount + enemycornerCount));
    //int cornetMoveEvaluation = 0;
    //int cornetMoveEvaluation = 1000 * myCornetMoves;
    int cornetMoveEvaluation = 1000 * (myCornetMoves - enemyCornetMoves) / ((myCornetMoves + enemyCornetMoves) == 0 ? 1 : (myCornetMoves + enemyCornetMoves));
    //   int corneringEval = -15 * myCorneringCount;
    int corneringEval = 0 * myCorneringCount;
    

    //std::cout << movesEvaluation << " " << scoreEvaluation << " " << cornerEvaluation << " " << cornetMoveEvaluation<< " " << edgeEvaluation << std::endl;

    return movesEvaluation + scoreEvaluation + cornerEvaluation + cornetMoveEvaluation + corneringEval + edgeEvaluation;
}

//minimax recursively iterate through everything
std::pair<int, std::pair<int, int>> fengkaiq::Amogus::minimax_recurs(const OthelloGameState& state, bool IPlayBlack, int max, int min, int depth)
{
    //if game has finished or we have reached the leaf
    if(state.isGameOver())
    {
        //we won
        if(state.blackScore() >= state.whiteScore() && IPlayBlack) return std::pair<int, std::pair<int, int>>(100000000, std::pair<int, int>(0,0));
        if(state.whiteScore() >= state.blackScore() && !IPlayBlack) return std::pair<int, std::pair<int, int>>(100000000, std::pair<int, int>(0,0));
        //we lost
        return std::pair<int, std::pair<int, int>>(-100000000, std::pair<int, int>(0,0));
    }
    //we reached maximum depth
    int maxDepth = MAX_DEPTH;
    if(float(state.blackScore() + state.whiteScore()) >= float(state.board().height() * state.board().width() - 4 - SCORE_RUSHB))
    {
        maxDepth += 2;
    }
    if(float(state.blackScore() + state.whiteScore()) >= float(state.board().height() * state.board().width() - 4 - FINAL_PUSH))
    {
        maxDepth += 2;
    }
    if(float(state.blackScore() + state.whiteScore()) >= float(state.board().height() * state.board().width() - 4 - FINAL_FINAL_PUSH))
    {
        maxDepth += 2;
    }
    // we can afford this loss if there aren't a lot of moves
    //if(getPossibleMoves(state, false).size() > DEPTH_CUTOFF) maxDepth-=2; 
    if(depth >= maxDepth)
    {
        return std::pair<int, std::pair<int, int>>(evaluate_winning(state), std::pair<int, int>(0,0));
    }
    //the following is some alpha beta pruning procedure
    //in essense, on each level either we or opponent will be playing
    //and the best practice would be to maximize our evaluation,
    //while keeping opponent's evaluation minimum
    if((IPlayBlack && state.isBlackTurn()) || (!IPlayBlack && state.isWhiteTurn()))
    {
        //we are moving, and so we should maximize our score
        std::vector<std::pair<int, int>> myMoves = getPossibleMoves(state, false);
        if(myMoves.size() > 0)
        {
            std::pair<int, std::pair<int, int>> returnSet = std::pair<int, std::pair<int, int>>(-2147483640, std::pair<int, int>(0,0));
            for(std::pair<int, int> move: myMoves)
            {
                //create a new state after moving
                std::unique_ptr<OthelloGameState> nextState_ptr = state.clone();
                OthelloGameState& nextState = *nextState_ptr;
                nextState.makeMove(move.first, move.second);
                std::pair<int, std::pair<int, int>> nextEvaluation = minimax_recurs(nextState, IPlayBlack, max, min, depth+1);
                if(nextEvaluation.first > returnSet.first)
                {
                    returnSet.first = nextEvaluation.first;
                    returnSet.second = move;
                }
                //our current evaluation is better than the maximum
                if(returnSet.first > max) max = returnSet.first;
                //we know that this move is better than the opponent's.
                if(min <= max) return returnSet;
            }
            return returnSet;
        }
    }
    else
    {
        //the opponent is moving, sabotage him O_O
        std::vector<std::pair<int, int>> enemyMoves = getPossibleMoves(state, false);
        if(enemyMoves.size() > 0)
        {
            std::pair<int, std::pair<int, int>> returnSet = std::pair<int, std::pair<int, int>>(2147483640, std::pair<int, int>(0,0));
            for(std::pair<int, int> move: enemyMoves)
            {
                //create a new state after moving
                std::unique_ptr<OthelloGameState> nextState_ptr = state.clone();
                OthelloGameState& nextState = *nextState_ptr;
                nextState.makeMove(move.first, move.second);
                std::pair<int, std::pair<int, int>> nextEvaluation = minimax_recurs(nextState, IPlayBlack, max, min, depth+1);
                if(nextEvaluation.first < returnSet.first)
                {
                    returnSet.first = nextEvaluation.first;
                    returnSet.second = move;
                }
                if(returnSet.first < min) min = returnSet.first;
                if(min <= max) return returnSet; //we know his move is not better than ours. return.
            }
            return returnSet;
        }
    }
    return std::pair<int, std::pair<int, int>>();
}

std::pair<int, int> fengkaiq::Amogus::chooseMove(const OthelloGameState& state)
{
    return minimax_recurs(state, state.isBlackTurn(), -2147483648, 2147483647, 0).second;
}

// #include <chrono>
// #include <iostream>
// std::pair<int, int> fengkaiq::Amogus::chooseMove(const OthelloGameState& state)
// {
//     using namespace std::chrono;
//     auto t1 = std::chrono::high_resolution_clock::now();
//     std::pair<int, int> ret = minimax_recurs(state, state.isBlackTurn(), -2147483648, 2147483647, 0).second;
//     auto t2 = std::chrono::high_resolution_clock::now();
//     int64_t ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
//     if(ms_int > 2500) std::cout << "Amogus disqualified... " << (double)ms_int / 1000 << " at score: " << state.blackScore() + state.whiteScore() << std::endl;
//     return ret;
// }

//damn you thornton! why can't thou make it easier for me to analyze my opponent!
//todo: fix this damn piece of abomination
bool fengkaiq::Amogus::isValidMove(const OthelloGameState& state, int x, int y, bool enemy)
{
    if(!enemy) return state.isValidMove(x, y);
    if(state.board().cellAt(x,y) != OthelloCell::empty) return false;

    //enemy of enemy is our friend :3
    OthelloCell enemycell = state.isBlackTurn() ? OthelloCell::black : OthelloCell::white;
    OthelloCell mycell = state.isBlackTurn() ? OthelloCell::white : OthelloCell::black;

    if(!enemy)
    {
        enemycell = OthelloCell::black == enemycell ? OthelloCell::white : OthelloCell::black;
        mycell = OthelloCell::black == mycell ? OthelloCell::white : OthelloCell::black;
        //assert(enemycell != mycell);
    }

    //a total of 8 directions we can choose
    const std::pair<int, int> directions[] = {
        std::pair<int, int>(0,1),
        std::pair<int, int>(0,-1),
        std::pair<int, int>(1,1),
        std::pair<int, int>(1,0),
        std::pair<int, int>(1,-1),
        std::pair<int, int>(-1,1),
        std::pair<int, int>(-1,0),
        std::pair<int, int>(-1,-1),
    };

    for(int i = 0; i < 8; i++)
    {
        //move up
        //std::cout << directions[i].first << " " << directions[i].second << std::endl;
        const int xDirectional = directions[i].first;
        const int yDirectional = directions[i].second;
        int newPosx = x + xDirectional;
        int newPosy = y + yDirectional;
        int counter = 0;
        //TODO: fix diagnol
        //yDirectional > 0 && newPosy < state.board().height()-1
        //yDirectional < 0 && newPosy > 0
        //yDirectional == 0
        //xDirectional > 0 && newPosx < state.board().width()-1
        //xDirectional < 0 && newPosx > 0
        //xDirectional == 0
        // ((yDirectional > 0 && newPosy < state.board().height()-1) || (yDirectional < 0 && newPosy > 0) || (yDirectional == 0))
        // ((xDirectional > 0 && newPosx < state.board().width()-1) || (xDirectional < 0 && newPosx > 0) || (xDirectional == 0))
        while(
            ((yDirectional > 0 && newPosy < state.board().height()-1) || (yDirectional < 0 && newPosy > 0) || (yDirectional == 0))
        && ((xDirectional > 0 && newPosx < state.board().width()-1) || (xDirectional < 0 && newPosx > 0) || (xDirectional == 0))
        && state.board().cellAt(newPosx, newPosy) == enemycell)
        {
            if(state.board().cellAt(newPosx,newPosy) == mycell) break;
            newPosx += xDirectional;
            newPosy += yDirectional;
            counter++;
        }
        // while(
        //     ((newPosy > 0 && newPosy < state.board().height()-1) || yDirectional == 0)
        // && ((newPosx > 0 && newPosx < state.board().width()-1) || xDirectional == 0)
        // && state.board().cellAt(newPosx, newPosy) == enemycell)
        // {
        //     if(state.board().cellAt(newPosx,newPosy) == mycell) break;
        //     newPosx += xDirectional;
        //     newPosy += yDirectional;
        //     counter++;
        // }
        if(newPosy >= 0 && newPosy <= state.board().height()-1
        && newPosx >= 0 && newPosx <= state.board().width()-1
        && state.board().cellAt(newPosx, newPosy) == mycell && counter > 0) 
        {
            return true;
        }
    }

    //exhausted :(
    return false;

}

