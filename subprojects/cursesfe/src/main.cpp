#include <curses.h>
#include <algorithm>
#include <format>
#include <sstream>
#include <string>
#include <unordered_map>

#include "GameState.hpp"
#include "BinaryLotMachine.hpp"

void drawPiecesWaitingToEnter(const ur::GameState& gs);
int drawBoardOutline(const ur::GameState& gs);
void drawStats(const ur::GameState& gs, int xBegin);
void drawStatus(const ur::GameState& gs, int xBegin);
void drawPieceCounts(const ur::GameState& gs);
void drawPieceCountForPlayer(const ur::GameState& gs, unsigned int player);

int main(int argc, char **argv)
{
    ur::GameState gs;
    ur::BinaryLotMachine blm;
    initscr();
    curs_set(0);
    noecho();
    int iters;
    int playersFound;
    std::ostringstream generatedTest;
    generatedTest << "ur::GameState gs;\n\n";

    while(true) {
        iters = 0;
        playersFound = 0;
        while(gs.waitingOnBinaryLot) {
            int roll = blm.next();
            gs.onBinaryLot(roll);
            generatedTest << "ASSERT_TRUE(gs.onBinaryLot(" << roll << "));\n";
            generatedTest << "ASSERT_EQ(gs.currentPlayerTurn, PLAYER_" << (gs.currentPlayerTurn + 1) << ");\n";
            playersFound |= (1 << gs.currentPlayerTurn);
            iters++;
            if(iters >= 100) {
                endwin();
                std::cerr << "Couldn't roll a binary lot to allow player to play. Players who it passed to:" << playersFound << std::endl;
                return -1;
            }
        }

        gs.checkIfPlayerHasWon();
        if(gs.playerHasWon) {
            endwin();
            std::cout << "Player " << (gs.winningPlayer + 1) << " wins!\n\n";
            generatedTest << "ASSERT_TRUE(gs.playerHasWon);\n";
            generatedTest << "ASSERT_EQ(gs.winningPlayer, PLAYER_" << (gs.winningPlayer + 1) << ");\n";
            std::cout << generatedTest.str();
            std::cout << std::flush;
            return 0;
        }
        clear();
        int maxX = drawBoardOutline(gs);
        drawStats(gs, maxX);
        drawStatus(gs, maxX);
        drawPieceCounts(gs);
        refresh();
        char c = getch();
        switch(c) {
            case 'q':
                goto ENDPROGRAM;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
                generatedTest << "gs.onMovePiece(" << c << ");\n";
                gs.onMovePiece(c - '0');
                break;
        }
    }
ENDPROGRAM:
    endwin();

    return 0;
}

void drawPieceCounts(const ur::GameState& gs)
{
    drawPieceCountForPlayer(gs, PLAYER_1);
    drawPieceCountForPlayer(gs, PLAYER_2);
}

void drawPieceCountForPlayer(const ur::GameState& gs, unsigned int player)
{
    std::unordered_map<const ur::coord, int, ur::coord::HashFunction> coords;
    for(int i = 0; i < gs.settingsPtr->counterCountPerPlayer; i++) {
        const ur::coord c = gs.counters[gs.playerOffset(player) + i];
        if(!coords.contains(c)) {
            coords[c] = 0;
        }
        coords[c] += 1;
    }

    for(const auto pair : coords) {
        if(!gs.settingsPtr->pieceOnBoard(pair.first)) {
            continue;
        }
        mvaddch(pair.first.y * 2 + 1, pair.first.x * 4 + (player == PLAYER_1 ? 1 : 3), '0' + pair.second);
    }
}

void drawStatus(const ur::GameState& gs, int xBegin)
{
    const unsigned int player = gs.currentPlayerTurn;
    if(gs.waitingOnBinaryLot) {
        mvaddstr(7, xBegin, "Waiting on Binary Lot                 ");
    } else {
        mvaddstr(7, xBegin, std::format("Player {} may move {} spaces", player + 1, gs.settingsPtr->decodeBinaryLot(gs.binaryLotResult)).c_str());
        mvaddstr(9, xBegin, "Pieces that may move:");
        int y = 10;
        for(int i = 0; i < gs.piecesThatMayMove.size(); i++) {
            if(gs.piecesThatMayMove[i]) {
                const ur::coord& pieceCoord = gs.getNthPieceByPlayer(i, player);
                const char *loc;
                if(gs.settingsPtr->pieceWaitingToEnter(pieceCoord)) {
                    loc = "Waiting to enter";
                } else {
                    loc = std::format("({}, {})", pieceCoord.x, pieceCoord.y).c_str();
                }
                mvaddstr(y++, xBegin, std::format("({}) @ {}", i, loc).c_str());
            }
        }
    }
}

void drawStats(const ur::GameState& gs, int xBegin)
{
    mvaddstr(2, xBegin, "Player 1:");
    mvaddstr(3, xBegin, std::format("    Yet to move: {} Finished: {}", gs.pieceWaitingToEnterCount(PLAYER_1), gs.finishedPieceCount(PLAYER_1)).c_str());
    mvaddstr(4, xBegin, "Player 2:");
    mvaddstr(5, xBegin, std::format("    Yet to move: {} Finished: {}", gs.pieceWaitingToEnterCount(PLAYER_2), gs.finishedPieceCount(PLAYER_2)).c_str());
}

int drawBoardOutline(const ur::GameState& gs)
{
    int maxRenderedX = 0;
    const char BOX_TOP[] = "+---+";

    for(const auto& path : gs.settingsPtr->paths) {
        for(int i = 1; i < path->size() - 1; i++) {
            const ur::coord c = (*path)[i];
            maxRenderedX = std::max(maxRenderedX, ((int)(c.x * 4 + sizeof(BOX_TOP))));
            mvaddstr(c.y * 2, c.x * (sizeof(BOX_TOP) - 2),     BOX_TOP);
            mvaddstr(c.y * 2 + 1, c.x * 4, "|   |");
            mvaddstr(c.y * 2 + 2, c.x * (sizeof(BOX_TOP) - 2),     BOX_TOP);
            if(gs.settingsPtr->isRosette(c)) {
                mvaddch(c.y * 2 + 1, c.x * 4 + 2, '*');
            }
        }
    }
    return maxRenderedX;
}

void drawPiecesWaitingToEnter(const ur::GameState& gs)
{
    const char MESSAGE[] = "Player {} has {} pieces wa";
}