#include <cstdio>
#include <iostream>
#include <gtest/gtest.h>

#include "GameState.hpp"


// TODO: Test for setting onBinaryLot to true after every onMovePiece
TEST(LibUr, NewPieceEntersOntoRosette)
{
    ur::GameState gs;
    ASSERT_TRUE(gs.waitingOnBinaryLot);
    gs.onBinaryLot(0);
    ASSERT_FALSE(gs.waitingOnBinaryLot);
    ASSERT_TRUE(gs.currentPlayerMayMovePieceOntoBoard());
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    ASSERT_TRUE(gs.movePieceOntoBoard());
    ASSERT_TRUE(gs.waitingOnBinaryLot);
    // it moved onto a Rosette, so we get an extra turn...
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
}

TEST(LibUr, NewPieceDoesntEnterOntoRosette)
{
    ur::GameState gs;
    ASSERT_TRUE(gs.waitingOnBinaryLot);
    gs.onBinaryLot(1);
    ASSERT_FALSE(gs.waitingOnBinaryLot);
    ASSERT_TRUE(gs.currentPlayerMayMovePieceOntoBoard());
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    ASSERT_TRUE(gs.movePieceOntoBoard());
    ASSERT_TRUE(gs.waitingOnBinaryLot);
    // Did not enter onto a Rosette, so it goes to next player
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    ASSERT_TRUE(gs.waitingOnBinaryLot);
}

TEST(LibUr, SecondPlayerEntersOntoRosette)
{
    ur::GameState gs;
    gs.onBinaryLot(1);
    ASSERT_TRUE(gs.movePieceOntoBoard());
    gs.onBinaryLot(0);
    ASSERT_TRUE(gs.movePieceOntoBoard());
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    ASSERT_TRUE(gs.waitingOnBinaryLot);
}

TEST(LibUr, CantEnterOnInitialSpaceFilled)
{
    ur::GameState gs;
    gs.onBinaryLot(1);
    ASSERT_TRUE(gs.movePieceOntoBoard());
    gs.onBinaryLot(1);
    ASSERT_TRUE(gs.movePieceOntoBoard());
    ASSERT_TRUE(gs.waitingOnBinaryLot);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    ASSERT_FALSE(gs.currentPlayerMayMovePieceOntoBoard());
    ASSERT_FALSE(gs.waitingOnBinaryLot);
}

TEST(LibUr, PieceCanLeaveBoard)
{
    ur::GameState gs;
    for(int i = 0; i < 4; i++) {
        gs.onBinaryLot(0);
        gs.onMovePiece(0);
        ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
        ASSERT_TRUE(gs.waitingOnBinaryLot);
    }
    gs.onBinaryLot(1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.waitingOnBinaryLot);
    ASSERT_EQ(gs.finishedPieceCount(PLAYER_1), 1);
}

TEST(LibUr, PieceCantLeaveBoardIfGoingTooFar)
{
    ur::GameState gs;
    for(int i = 0; i < 3; i++) {
        gs.onBinaryLot(0);
        gs.onMovePiece(0);
        ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    }
    // should be at last space, never going to the other player...
    gs.onBinaryLot(0b111);
    gs.onMovePiece(0);
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onBinaryLot(1);
    gs.onMovePiece(1);
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onBinaryLot(0);
    ASSERT_FALSE(gs.piecesThatMayMove[0]);
    ASSERT_FALSE(gs.onMovePiece(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    ASSERT_EQ(gs.finishedPieceCount(PLAYER_1), 0);
}

TEST(LibUr, Capture)
{
    ur::GameState gs;
    gs.onBinaryLot(0b111);
    gs.onMovePiece(0);
    gs.onBinaryLot(0b111);
    gs.onMovePiece(0);
    // both right before rosette
    gs.onBinaryLot(0b101);
    gs.onMovePiece(0);
    // player 1 on first shared piece
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    // both players have pieces on board...
    ASSERT_EQ(gs.pieceWaitingToEnterCount(PLAYER_1), 4);
    ASSERT_EQ(gs.pieceWaitingToEnterCount(PLAYER_2), 4);
    gs.onBinaryLot(0b101);
    gs.onMovePiece(0);
    // captured!
    ASSERT_EQ(gs.pieceWaitingToEnterCount(PLAYER_1), 5);
    ASSERT_EQ(gs.pieceWaitingToEnterCount(PLAYER_2), 4);
    const ur::coord player2Counter = gs.getNthPieceByPlayer(0, PLAYER_2);
    ASSERT_EQ(player2Counter.x, 1);
    ASSERT_EQ(player2Counter.y, 0);
}

TEST(LibUr, WhileOnBoardCantMoveOntoSpaceIfFriendlyPieces)
{
    // P2 movements aren't relevant for this test, just given moves to advance the state
    ur::GameState gs;
    gs.onBinaryLot(1);
    gs.onMovePiece(0);

    // P2
    gs.onBinaryLot(1);
    gs.onMovePiece(0);

    // P1
    gs.onBinaryLot(1);
    gs.onMovePiece(0);

    // P2
    gs.onBinaryLot(1);
    gs.onMovePiece(0);

    // P1
    gs.onBinaryLot(1);
    gs.onMovePiece(1);

    // P2
    gs.onBinaryLot(2);
    gs.onMovePiece(0);

    // P1
    gs.onBinaryLot(1);
    // it's NOT a rosette so we know we can't move onto that space
    ASSERT_FALSE(gs.settingsPtr->isRosette(gs.counters[gs.playerOffset(PLAYER_1) + 0]));
    ASSERT_FALSE(gs.onMovePiece(1));
}

// Runs a full game, including with attempts to move unallowed pieces, rosettes and captures happen.
TEST(LibUr, GameToWin)
{
    ur::GameState gs;

    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    gs.onMovePiece(0);
    gs.onMovePiece(1);
    gs.onMovePiece(0);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    gs.onMovePiece(2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    gs.onMovePiece(1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    gs.onMovePiece(0);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(0));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(0);
    gs.onMovePiece(1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(4));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    gs.onMovePiece(0);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(5));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    ASSERT_TRUE(gs.onBinaryLot(3));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    gs.onMovePiece(2);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(0);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(1);
    ASSERT_TRUE(gs.onBinaryLot(6));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    ASSERT_TRUE(gs.onBinaryLot(7));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.onMovePiece(3);
    gs.onMovePiece(2);
    gs.onMovePiece(0);
    gs.onMovePiece(4);
    ASSERT_TRUE(gs.onBinaryLot(1));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_2);
    gs.onMovePiece(1);
    gs.onMovePiece(3);
    ASSERT_TRUE(gs.onBinaryLot(2));
    ASSERT_EQ(gs.currentPlayerTurn, PLAYER_1);
    gs.checkIfPlayerHasWon();
    ASSERT_TRUE(gs.playerHasWon);
    ASSERT_EQ(gs.winningPlayer, PLAYER_2);
}