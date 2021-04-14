#include "include/singleplayergame.h"

// Constructor for SinglePlayerGame class
SinglePlayerGame::SinglePlayerGame(QGraphicsScene *scene, bool computerIsWhite) : Game(scene) {
    computerColorWhite = computerIsWhite;
    srand(time(NULL));
    if (computerColorWhite) {
        computerPhaseOneMove();
    }
    menuButton = new QPushButton(QString("Main Menu"));
    menuButton->setGeometry(325,800,150,50);
    QFont buttonFont("comic sans MS", 14);
    menuButton->setFont(buttonFont);
    menuButton->setStyleSheet("background-color: brown; color: #00DCDC; border-style: outset; border-width: 2px; border-radius: 3px; border-color: yellow; padding: 6px;");
    scene->addWidget(menuButton);
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


// Scans the board for priority population
void SinglePlayerGame::priorityScan() {
    unsigned int i;
    unsigned int j;
    unsigned int blackCounter;
    unsigned int whiteCounter;
    int emptyIndex;
    
    // Clears the vectors for repopulation
    possibleMill.clear();
    possibleBlock.clear();
    
    // Scans the board to find possible mills and blocks
    for (i = 0; i < millList.size(); i++){
        blackCounter = 0;
        whiteCounter = 0;
        emptyIndex = -1;
        for (j = 0; j < 3; j++){
            if (spaceList[millList[i][j]]->hasWhitePiece()){
                whiteCounter++;
            } else if (spaceList[millList[i][j]]->hasBlackPiece()){
                blackCounter++;
            } else{
              emptyIndex = millList[i][j];
            }
            if (whiteCounter == 2 && emptyIndex != -1){
                (computerColorWhite) ? possibleMill.push_back(emptyIndex) : possibleBlock.push_back(emptyIndex);
            }
            if (blackCounter == 2 && emptyIndex != -1) {
                (!computerColorWhite) ? possibleMill.push_back(emptyIndex) : possibleBlock.push_back(emptyIndex);
            }
        }
    }
}


//Chooses an available space and moves in first phase
void SinglePlayerGame::computerPhaseOneMove() {
    priorityScan();
    // Going to want to prioritize mills over blocks
    if (!possibleMill.empty()){
        spaceList[possibleMill[0]]->computerClickSpace();
    } else if (!possibleBlock.empty()){
        spaceList[possibleBlock[0]]->computerClickSpace();
    } else {
        scanSpaces();
        int randIndex = rand() % availableSpaces.size();
        spaceList[availableSpaces[randIndex]]->computerClickSpace();
    }
}

void SinglePlayerGame::computerPhaseTwoPriority(std::vector<std::vector<int>> &adjacentSpaces){
    unsigned int i;
    for (i = 0; i < availableSelect.size(); i++){
        adjacentSpaces.push_back(adjacentList[availableSelect[i]]);
    }
}

void SinglePlayerGame::computerIntersectionFind(std::vector<std::vector<int>> &adjacentSpaces){
    unsigned int i;
    unsigned int j;
    priorityList.clear();
    if (!possibleMill.empty()){
        for (i = 0; i < adjacentSpaces.size(); i++){
            for (j = 0; i < adjacentSpaces[i].size(); j++){
                if (possibleMill[i] == adjacentSpaces[i][j]){
                    priorityList.push_back(possibleMill[i]);
                    break;
                }
            }
        }
    }
    if (!possibleBlock.empty()){
        for (i = 0; i < adjacentSpaces.size(); i++){
            for (j = 0; j < adjacentSpaces[i].size(); j++){
                if (possibleBlock[i] == adjacentSpaces[i][j]){
                    priorityList.push_back(possibleBlock[i]);
                    break;
                }
            }
        }
    }
}

//Chooses a piece and moves to an open space
void SinglePlayerGame::computerPhaseTwoMove() {
    unsigned int i;
    std::vector<std::vector<int>> adjacentSpaces;
    bool validMove = false;
    scanSpaces();
    priorityScan();
    Piece *chosenPiece;
    //Loops until a piece with an open adjacent space is selected
    while (!validMove) {
        //Choosing a piece
        //int randIndex = rand() % availableSelect.size();
        //int pieceIndex = availableSelect[randIndex];
        /*if (computerColorWhite) {
            chosenPiece = whitePieces[pieceIndex];
        } else {
            chosenPiece = blackPieces[pieceIndex];
        }*/
        //Choosing a space to move to
        computerPhaseTwoPriority(adjacentSpaces);
        computerIntersectionFind(adjacentSpaces);
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

//Overrides function to add computer move functionality
void SinglePlayerGame::startNewTurn() {
    Game::startNewTurn();
    if (whiteTurn == computerColorWhite) {
        if (!phaseOneComplete) {
            computerPhaseOneMove();
        } else if ((!whiteFlying && computerColorWhite) || (!blackFlying && !computerColorWhite)) {
            computerPhaseTwoMove();
        } else if ((whiteFlying && computerColorWhite) || (blackFlying && !computerColorWhite)) {
            computerFlyingMove();
        }
    }
}

void SinglePlayerGame::nextTurn(Piece *piece) {
    endTurn(piece);
    checkForNewMill();
    if (!captureMode) {
        evaluateVictoryConditions();
    } else if (whiteTurn == computerColorWhite) {
        computerCapture();
    }
}
