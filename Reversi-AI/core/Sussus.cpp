
// MyOthelloAI.cpp
//
// some minimax search stuff

#include <ics46/factory/DynamicFactory.hpp>
#include "Sussus.hpp"
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
//was 10
//const int DEPTH_CUTOFF = 10;

ICS46_DYNAMIC_FACTORY_REGISTER(OthelloAI, fengkaiq::Sussus, "Thorntis Patton Susbot O_O (Required)");

//returns a vector of possible moves
std::vector<std::pair<int, int>> fengkaiq::Sussus::getPossibleMoves(const OthelloGameState& state, bool enemy)
{
    std::vector<std::pair<int, int>> moves{};
    for(int i = 0; i < state.board().width(); i++)
    {
        for(int j = 0; j < state.board().height(); j++)
        {
            if(isValidMove(state, i,j, enemy))moves.push_back(std::pair<int, int>(i,j));
        }
    }
    return moves;
}

//evaluates current state's winning-ness, larger the number the more desired this state is
int fengkaiq::Sussus::evaluate_winning(const OthelloGameState& state)
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

    //it should scale nicely to the current situation
    //todo: movesEvaluation is returning a very random number.
    //int movesEvaluation = 500 * ((int)(unsigned int)myMoves.size() - (int)(unsigned int)enemyMoves.size()) / ((int)(unsigned int)myMoves.size() + (int)(unsigned int)enemyMoves.size() + 1);
    int movesEvaluation = 300 * ((int)(unsigned int)myMoves.size() - ENEMY_MOVE_COUNT) / ((int)(unsigned int)myMoves.size() + ENEMY_MOVE_COUNT);
    int scoreEvaluation = -200 * (myscore - enemyscore) / (myscore + enemyscore);
    //int cornerEvaluation = 0;
    //int cornerEvaluation = 100000 * mycornerCount;
    int cornerEvaluation = 1000 * (mycornerCount - enemycornerCount) / ((mycornerCount + enemycornerCount) == 0 ? 1 : (mycornerCount + enemycornerCount));
    //int cornetMoveEvaluation = 0;
    //int cornetMoveEvaluation = 1000 * myCornetMoves;
    int cornetMoveEvaluation = 1000 * (myCornetMoves - enemyCornetMoves) / ((myCornetMoves + enemyCornetMoves) == 0 ? 1 : (myCornetMoves + enemyCornetMoves));

    //std::cout << movesEvaluation << " " << scoreEvaluation << " " << cornerEvaluation << " " << cornetMoveEvaluation << std::endl;

    return movesEvaluation + scoreEvaluation + cornerEvaluation + cornetMoveEvaluation;
}


//minimax recursively iterate through everything
std::pair<int, std::pair<int, int>> fengkaiq::Sussus::minimax_recurs(const OthelloGameState& state, bool IPlayBlack, int max, int min, int depth)
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

// #include <chrono>
// #include <iostream>
// std::pair<int, int> fengkaiq::Sussus::chooseMove(const OthelloGameState& state)
// {
//     using namespace std::chrono;
//     auto t1 = std::chrono::high_resolution_clock::now();
//     std::pair<int, int> ret = minimax_recurs(state, state.isBlackTurn(), -2147483648, 2147483647, 0).second;
//     auto t2 = std::chrono::high_resolution_clock::now();
//     int64_t ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
//     if(ms_int > 2500) std::cout << "Sussus disqualified... " << (double)ms_int / 1000 << " at score: " << state.blackScore() + state.whiteScore() << std::endl;
//     return ret;
// }

std::pair<int, int> fengkaiq::Sussus::chooseMove(const OthelloGameState& state)
{
    return minimax_recurs(state, state.isBlackTurn(), -2147483648, 2147483647, 0).second;
}

//why can't thou make it easier for me to analyze my opponent!
//todo: fix this damn piece of abomination
bool fengkaiq::Sussus::isValidMove(const OthelloGameState& state, int x, int y, bool enemy)
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



