#include "BoardDelegate.h"
#include "Board.h"


void DefaultBoardDelegate::onDiamondsSwapped(Board* sender, 
                                             const DiamondCoords& d1, const DiamondCoords& d2)
{
    sender->onDiamondsSwappedAnimationFinished();
}
    
void DefaultBoardDelegate::onDiamondsDisappeared(Board* sender)
{
    sender->onDiamondsDisappearedAnimationFnished();
}
    
void DefaultBoardDelegate::onDiamondsMoved(Board* sender, 
                                 const CoordsArray& toCoordsArray, 
                                 const CoordsArray& fromCoordsArray)
{
    sender->onDiamondsMovedAnimationFinished();
}

