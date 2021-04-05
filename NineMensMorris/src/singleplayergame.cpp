#include "include/singleplayergame.h"

// Constructor for SinglePlayerGame class
SinglePlayerGame::SinglePlayerGame(QGraphicsScene *scene, bool computerIsWhite) : Game(scene) {
    computerColorWhite = computerIsWhite;
    srand(time(NULL));
    if (computerColorWhite) {
        computerPhaseOneMove();
    }
}
//Adds unoccupied spaces to available
void SinglePlayerGame::scanSpaces() {
    unsigned int i;
    availableSpaces.clear();
    for (i = 0; i < spaceList.size(); i++) {
        if (!spaceList[i]->isOccupied()) {
            availableSpaces.push_back(i);
        }
    }
}

//Chooses an available space and moves in first phase
void SinglePlayerGame::computerPhaseOneMove() {
    scanSpaces();
    int randIndex = rand() % availableSpaces.size();
    spaceList[availableSpaces[randIndex]]->computerClickSpace();
}

//Chooses a piece and moves to an open space
void SinglePlayerGame::computerPhaseTwoMove() {
    unsigned int i;
    bool validMove = false;
    Piece *chosenPiece;
    //Loops until a piece with an open adjacent space is selected
    while (!validMove) {
        //Choosing a piece
        int randIndex = rand() % availableSelect.size();
        int pieceIndex = availableSelect[randIndex];
        if (computerColorWhite) {
            chosenPiece = whitePieces[pieceIndex];
        } else {
            chosenPiece = blackPieces[pieceIndex];
        }
        //Choosing a space to move to
        availableSpaces.clear();
        int startSpaceIndex = getSpaceIndex(chosenPiece->getSpace());
        for (i = 0; i < adjacentList[startSpaceIndex].size(); i++) {
            if (!spaceList[adjacentList[startSpaceIndex][i]]->isOccupied()) {
                availableSpaces.push_back(adjacentList[startSpaceIndex][i]);
                validMove = true;
            }
        }
    }
    //Pick a random adjacent space
    int moveSpaceIndex = availableSpaces[rand() % availableSpaces.size()];
    //Select piece and move to space
    chosenPiece->computerPlayerSelect();
    spaceList[moveSpaceIndex]->computerClickSpace();
}


//Selects piece and moves to available space
void SinglePlayerGame::computerFlyingMove() {
    Piece *chosenPiece;
    scanSpaces();
    int randIndex = rand() % availableSpaces.size();
    int spaceIndex = availableSpaces[randIndex];
    randIndex = rand() % availableSelect.size();
    int pieceIndex = availableSelect[randIndex];
    if (computerColorWhite) {
        chosenPiece = whitePieces[pieceIndex];
    } else {
        chosenPiece = blackPieces[pieceIndex];
    }
    chosenPiece->computerPlayerSelect();
    spaceList[spaceIndex]->computerClickSpace();
}

//Selects capturable piece and captures it
void SinglePlayerGame::computerCapture() {
    int randIndex = rand() % availableCapture.size();
    int pieceIndex = availableCapture[randIndex];
    if (computerColorWhite) {
        blackPieces[pieceIndex]->computerPlayerCapture();
    } else {
        whitePieces[pieceIndex]->computerPlayerCapture();
    }
}

//In addition to base functionality, adds selectable piece indices to available vector
void SinglePlayerGame::enableSelectPiece() {
    unsigned int i;
    availableSelect.clear();
    if (whiteTurn) {
        for (i = 0; i < whitePieces.size(); i++) {
            if (!whitePieces[i]->isCaptured()) {
                connect(whitePieces[i], SIGNAL(clickSelect(Piece*)), this, SLOT(pieceSelectAction(Piece*)));
                whitePieces[i]->setSelectable(true);
                availableSelect.push_back(i);
            }
        }
    } else {
        for (i = 0; i < blackPieces.size(); i++) {
            if (!blackPieces[i]->isCaptured()) {
                connect(blackPieces[i], SIGNAL(clickSelect(Piece*)), this, SLOT(pieceSelectAction(Piece*)));
                blackPieces[i]->setSelectable(true);
                availableSelect.push_back(i);
            }
        }
    }
    if ((whiteTurn && whiteFlying) || (!whiteTurn && blackFlying)) {
        setAllSpaceValidity(true);
    }
}

//In addition to base functionality, adds pieces available to capture to vector
void SinglePlayerGame::enableCapturePiece() {
    unsigned int i;
    int count = 0;
    captureMode = true;
    setInstructionText(turnNumber, captureMode);
    availableCapture.clear();
    if (whiteTurn) {
        //First pass selects pieces not in a mill
        for (i = 0; i < blackPieces.size(); i++) {
            if (!pieceInMill(blackPieces[i]) && !blackPieces[i]->isCaptured() && blackPieces[i]->isInPlay()) {
                connect(blackPieces[i], SIGNAL(clickCapture(Piece*)), this, SLOT(pieceCaptureAction(Piece*)));
                blackPieces[i]->setCaptureEnabled(true);
                availableCapture.push_back(i);
                count++;
            }
        }
        //Optional second pass if all pieces in mill, selects pieces in mill
        if (count == 0) {
            for (i = 0; i < blackPieces.size(); i++) {
                if (blackPieces[i]->isInPlay()) {
                    connect(blackPieces[i], SIGNAL(clickCapture(Piece*)), this, SLOT(pieceCaptureAction(Piece*)));
                    blackPieces[i]->setCaptureEnabled(true);
                    availableCapture.push_back(i);
                }
            }
        }
    } else {
        //First pass selects pieces not in a mill
        for (i = 0; i < whitePieces.size(); i++) {
            if (!pieceInMill(whitePieces[i]) && !whitePieces[i]->isCaptured() && whitePieces[i]->isInPlay()) {
                connect(whitePieces[i], SIGNAL(clickCapture(Piece*)), this, SLOT(pieceCaptureAction(Piece*)));
                whitePieces[i]->setCaptureEnabled(true);
                availableCapture.push_back(i);
                count++;
            }
        }
        //Optional second pass if all pieces in mill, selects pieces in mill
        if (count == 0) {
            for (i = 0; i < whitePieces.size(); i++) {
                if (whitePieces[i]->isInPlay()) {
                    connect(whitePieces[i], SIGNAL(clickCapture(Piece*)), this, SLOT(pieceCaptureAction(Piece*)));
                    whitePieces[i]->setCaptureEnabled(true);
                    availableCapture.push_back(i);
                }
            }
        }
    }
}

//Overrides function to add computer player capture
void SinglePlayerGame::checkForNewMill() {
    Game::checkForNewMill();
    if ((whiteTurn == computerColorWhite) && captureMode) {
        computerCapture();
    }
}

//Overrides function to add computer move functionality
void SinglePlayerGame::startNewTurn() {
    Game::startNewTurn();
    if (whiteTurn == computerColorWhite) {
        if (!phaseOneComplete) {
            computerPhaseOneMove();
        } else if ((!whiteFlying && computerColorWhite) || (!blackFlying && !computerColorWhite)) {
            computerPhaseTwoMove();
        } else {
            computerFlyingMove();
        }
    }
}


