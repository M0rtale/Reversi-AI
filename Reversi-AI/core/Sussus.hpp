// nikobelic29.hpp

#include "OthelloAI.hpp"


namespace fengkaiq
{
    class Sussus : public OthelloAI
    {
    public:
        std::pair<int, int> chooseMove(const OthelloGameState& state) override;
    private:
        std::vector<std::pair<int, int>> getPossibleMoves(const OthelloGameState& state, bool enemy);
        int evaluate_winning(const OthelloGameState& state);
        std::pair<int, std::pair<int, int>> minimax_recurs(const OthelloGameState& state, bool IPlayBlack, int max, int min, int depth);
        bool isValidMove(const OthelloGameState& state, int x, int y, bool enemy);
    };
}