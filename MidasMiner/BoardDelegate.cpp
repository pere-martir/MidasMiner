#include "BoardDelegate.h"
#include "Board.h"


void DefaultBoardDelegate::onDiamondsSwapped(Board* sender, 
                                             const DiamondCoords& d1, const DiamondCoords& d2)
{
    sender->onAnimationFinished(Board::ANIMATION_SWAPPING);
}
    
void DefaultBoardDelegate::onDiamondsRemoved(Board* sender)
{
    sender->onAnimationFinished(Board::ANIMATION_REMOVING);
}
    
void DefaultBoardDelegate::onDiamondsFallen(Board* sender, 
                                 const CoordsArray& fromCoordsArray, 
                                 const CoordsArray& toCoordsArray)
{
    sender->onAnimationFinished(Board::ANIMATION_FALLING);
}

