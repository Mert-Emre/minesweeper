#include "MyGrid.h"
#include "ImageButton.h"
#include <cstdlib>
#include <QMessageBox>
#include <QDir>

/* This class is responsible from creating the mine map, determining their location at the start of the game, assigning each cell a status(open,revealed, not revealed)
and controlling user input and change the status of the cells accordingly.
When an instance of this class is created, it loads the asssets and create icons from them. When the cells are created, these icons are passed to the cell constructor(ImageButton).
This class has 5 slots and 3 signals.
Slots are:
    1) onRightClick => When an image button is right clicked it sends a signal with its row and column information. This slot gets the row and column from the signal
    and toggles the icon of the ImageButton that sends the signal.
    2) onLeftClick => When an image button is left clicked it sends a signal with its row and column information. This slot gets the row and column from the signal
    and starts opening the cells recursively.
    3) onRestart => When restart button is clicked, this slot reactives the signals for ImageButtons(thus they are clickable again), resets their icon to empty icon, changes the status of
    all cells to not_revealed and resets bomb locations.
    4) onHint => When hint button is clicked this slot first changes the color of a cell that does not contain a bomb(this location can be also detected by a human), when the
    hint button is clicked again, it opens that cell recursively.
    5) onGameFinish => This slot listens its own gameWon and gameLost signals. It makes everything unclickable except the restart button and opens all of the mines.
Signals are:
    1) gameWon =>  When remaining unopened cell count is equal to the bomb count in the map, this signal is emitted, ongamefinish slot listens for it.
    2) gameLost => When the player clicks to a mine, this signal is emitted, ongamefinish slot listens for it.
    3) gameStarted => When game is started by clicking to a cell for the first time(after restart or after starting the app), this signal is emitted, hint button listens for it
    and it becomes clickable when it gets this signal.

    There are 4 types of game status:
    INIT: None of the cells are opened, game can be started by left clicking to a cell.
    PLAYING: Game has already startes and it continues.
    WIN: Game is finished and it is won.
    LOSE: Game is finished and it is lost.
*/

MyGrid::MyGrid(unsigned short rowSize, unsigned short colSize, unsigned short iconSize,unsigned short bombs, QLabel* scoreLabel) : QGridLayout() {
    // row size and column size and total bom count can be given from the main function and can be changed easliy
    this->gameStatus = INIT;
    this->rowSize = rowSize;
    this->colSize = colSize;
    this->setHorizontalSpacing(0);
    this->setVerticalSpacing(0);
    this->bombs = bombs;
    this->bombLocations = new QHash<QString, bool>();
    this->scoreLabel = scoreLabel;
    this->lastHint = nullptr;
    // Load icons
    QPixmap pixmap0("../../assets/0.png");
    pixmap0 = pixmap0.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon0(pixmap0);
    this->icons[0] = icon0;

    QPixmap pixmap1("../../assets/1.png");
    pixmap1 = pixmap1.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon1(pixmap1);
    this->icons[1] = icon1;

    QPixmap pixmap2("../../assets/2.png");
    pixmap2 = pixmap2.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon2(pixmap2);
    this->icons[2] = icon2;

    QPixmap pixmap3("../../assets/3.png");
    pixmap3 = pixmap3.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon3(pixmap3);
    this->icons[3] = icon3;

    QPixmap pixmap4("../../assets/4.png");
    pixmap4 = pixmap4.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon4(pixmap4);
    this->icons[4] = icon4;

    QPixmap pixmap5("../../assets/5.png");
    pixmap5 = pixmap5.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon5(pixmap5);
    this->icons[5] = icon5;

    QPixmap pixmap6("../../assets/6.png");
    pixmap6 = pixmap6.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon6(pixmap6);
    this->icons[6] = icon6;

    QPixmap pixmap7("../../assets/7.png");
    pixmap7 = pixmap7.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon7(pixmap7);
    this->icons[7] = icon7;

    QPixmap pixmap8("../../assets/8.png");
    pixmap8 = pixmap8.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon icon8(pixmap8);
    this->icons[8] = icon8;

    QPixmap pixmapEmpty("../../assets/empty.png");
    pixmapEmpty = pixmapEmpty.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon iconEmpty(pixmapEmpty);
    this->icons[9] = iconEmpty;

    QPixmap pixmapFlag("../../assets/flag.png");
    pixmapFlag = pixmapFlag.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon iconFlag(pixmapFlag);
    this->icons[10] = iconFlag;

    QPixmap pixmapHint("../../assets/hint.png");
    pixmapHint = pixmapHint.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon iconHint(pixmapHint);
    this->icons[11] = iconHint;

    QPixmap pixmapMine("../../assets/mine.png");
    pixmapMine = pixmapMine.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon iconMine(pixmapMine);
    this->icons[12] = iconMine;

    QPixmap pixmapWrongFlag("../../assets/wrong-flag.png");
    pixmapWrongFlag = pixmapWrongFlag.scaled(QSize(iconSize, iconSize), Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QIcon iconWrongFlag(pixmapWrongFlag);
    this->icons[13] = iconWrongFlag;
    // create the cells but do not add bombs to any of them. Wait until a cell is clicked.
    for(int row=0;row<rowSize;row++){
        for(int column=0; column <colSize; column++){
            ImageButton* imgBtn = new ImageButton(row,column,9,iconSize,this->icons[9]);
            QObject::connect(imgBtn, SIGNAL(rightClicked(unsigned short,unsigned short)), this, SLOT(onRightClick(ushort,ushort)));
            QObject::connect(imgBtn, SIGNAL(leftClicked(unsigned short,unsigned short)), this, SLOT(onLeftClick(ushort,ushort)));
            this->addWidget(imgBtn,row,column);
        }
    }
}
// Check if the given row and column is is inside the map.
bool MyGrid::isInGrid(short row, short col){
    if(row < this->rowSize && row >= 0 && col < this-> colSize && col >= 0){
        return true;
    }
    return false;
}
// This function opens the cells recursively when a cell is clicked.
// The general idea behind this function is:
// If the clicked button is a mine, this function is never called, so if this function is called, the cell that the player clicked is not a mine.
// So open the clicked cell first and meke it unclickable and change its icon to the number of neighbour bombs.
// Then for each neighbour if the neighbour is not a mine and is not opened already call this function recursively.
// When it reaches to a cell such that all of its neighbours are a mine or opened, it starts returning.
void MyGrid::openCells(unsigned short row, unsigned short col){
    ImageButton* imgBtn = qobject_cast<ImageButton*>(this->itemAtPosition(row,col)->widget());
    imgBtn->status = OPEN;
    imgBtn ->iconIdx = imgBtn->neigbourBombs;
    imgBtn->setIcon(icons[imgBtn->neigbourBombs]);
    imgBtn->blockSignals(true);
    if(imgBtn->neigbourBombs != 0){
        return;
    }
    if(isInGrid(row-1,col)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row-1,col);
        }
    }
    if(isInGrid(row-1,col+1)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col+1)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row-1,col+1);
        }
    }
    if(isInGrid(row,col+1)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row,col+1)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row,col+1);
        }
    }
    if(isInGrid(row+1,col+1)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col+1)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row+1,col+1);
        }
    }
    if(isInGrid(row+1,col)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row+1,col);
        }
    }
    if(isInGrid(row+1,col-1)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col-1)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row+1,col-1);
        }
    }
    if(isInGrid(row,col-1)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row,col-1)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row,col-1);
        }
    }
    if(isInGrid(row -1,col-1)){
        ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col-1)->widget());
        if(!neighbour->bomb && neighbour->status != OPEN){
            openCells(row-1,col-1);
        }
    }
}
/*
    If the user cliked to the hint button this slot is called. If the user clicks for the first time, than the class will not have a lsat hint and calculate the hint
    and save it to the last hint. When the method is called and there exists a last hint, this cell is opened recursively and last hint is assigned nullptr. So when the user
    clicks to the hint button again it will not open the cell but find a cell that does not contain a bomb and will change its icon to the green icon.
    The general idea behind finding a hint is:
    First traverse all of the opened cells and check if their unopened neighbour cell count is equal to their neighbour mine count. If they are equal than these mines are
    revealed to human eye. Change their status to revealed. When traversing is done we have some number of revealed bombs.
    Then start traversing unopened cells that does not contain a bomb(part2). If the number of neighbour bombs is not equal to unopened neighbour cells and all of the neighbour mines
    are revealed than it is safe to open a neighbour that is not a bomb. When hint button is clicked for the first time, such a neighbour is saved to the last hint and
    when it is clicked for the second time we open the last hint recursively.
*/
void MyGrid::onHint(){
    if(lastHint != nullptr){
        openCells(lastHint->row, lastHint->col);
        lastHint = nullptr;
        // check if the game is finished
        int openCellCount = 0;
        for(int row = 0; row<rowSize; row++){
            for(int col=0; col < colSize; col++){
                ImageButton* currBtn = qobject_cast<ImageButton*>(itemAtPosition(row,col)->widget());
                if(currBtn->status == OPEN)
                    openCellCount++;
            }
        }
        scoreLabel->setText("Score: " + QString::number(openCellCount));
        if (rowSize * colSize - openCellCount == bombs){
            emit gameWon();
            QMessageBox msgBox;
            msgBox.setText("You win!");
            msgBox.exec();
            msgBox.setStandardButtons(QMessageBox::Ok);   
        }
        return;
    }
    // Find the revealed bombs. They are added to a list as row,column pair string.
    for(int row = 0; row < rowSize; row++){
        for(int col = 0; col < colSize; col++){
            // If the button is not open or it is a bomb do not check its neigbhours.
            ImageButton* currentBtn = qobject_cast<ImageButton*>(itemAtPosition(row,col)->widget());
            if(currentBtn->status != OPEN || currentBtn->bomb){
                continue;
            }
            unsigned short notOpenedCells = 0;
            QList<QString>* neighbourBombs = new QList<QString>();
            if(isInGrid(row-1,col)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row-1).arg(col);
                    neighbourBombs->append(rowColPair);
                }
            }
            if(isInGrid(row-1,col+1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col+1)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row-1).arg(col+1);
                    neighbourBombs->append(rowColPair);
                }
            }
            if(isInGrid(row,col+1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row,col+1)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row).arg(col+1);
                    neighbourBombs->append(rowColPair);
                }
            }
            if(isInGrid(row+1,col+1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col+1)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row+1).arg(col+1);
                    neighbourBombs->append(rowColPair);
                }
            }
            if(isInGrid(row+1,col)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row+1).arg(col);
                    neighbourBombs->append(rowColPair);
                }
            }
            if(isInGrid(row+1,col-1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col-1)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row+1).arg(col-1);
                    neighbourBombs->append(rowColPair);
                }
            }
            if(isInGrid(row,col-1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row,col-1)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row).arg(col-1);
                    neighbourBombs->append(rowColPair);
                }
            }
            if(isInGrid(row -1,col-1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col-1)->widget());
                if(neighbour->status != OPEN){
                    notOpenedCells++;
                    auto rowColPair = QStringLiteral("%1,%2").arg(row-1).arg(col-1);
                    neighbourBombs->append(rowColPair);
                }
            }
            // If unopened neighbour cell count is equal to the neihbour bomb count, mark these bombs as revealed.
            if(notOpenedCells == currentBtn->neigbourBombs){
                for(int i=0; i < neighbourBombs->size(); i++){
                    QStringList rowColList = neighbourBombs->at(i).split(u',');
                    int bombRow = rowColList.at(0).toInt();
                    int bombCol = rowColList.at(1).toInt();
                    ImageButton* bombBtn = qobject_cast<ImageButton*>(itemAtPosition(bombRow,bombCol)->widget());
                    bombBtn->status = REVEALED;
                }
            }
            delete neighbourBombs;
        }
    }
    bool hintMarked = false;
    // Iterate unopened cells that does not contain a mine. Part2
    for(int row = 0; row< rowSize; row++){
        for(int col = 0; col < colSize; col++){
            ImageButton* currentBtn = qobject_cast<ImageButton*>(itemAtPosition(row,col)->widget());
            if(currentBtn->status != OPEN || currentBtn->bomb){
                continue;
            }
            unsigned short revealedCount = 0;
            QList<QString>* notRevealedList = new QList<QString>();
            if(isInGrid(row-1,col)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row-1).arg(col);
                    notRevealedList->append(rowColPair);
                }
            }
            if(isInGrid(row-1,col+1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col+1)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row-1).arg(col+1);
                    notRevealedList->append(rowColPair);
                }
            }
            if(isInGrid(row,col+1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row,col+1)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row).arg(col+1);
                    notRevealedList->append(rowColPair);
                }
            }
            if(isInGrid(row+1,col+1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col+1)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row+1).arg(col+1);
                    notRevealedList->append(rowColPair);
                }
            }
            if(isInGrid(row+1,col)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row+1).arg(col);
                    notRevealedList->append(rowColPair);
                }
            }
            if(isInGrid(row+1,col-1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row+1,col-1)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row+1).arg(col-1);
                    notRevealedList->append(rowColPair);
                }
            }
            if(isInGrid(row,col-1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row,col-1)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row).arg(col-1);
                    notRevealedList->append(rowColPair);
                }
            }
            if(isInGrid(row -1,col-1)){
                ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(row-1,col-1)->widget());
                if(neighbour->status == REVEALED){
                    revealedCount++;
                }else if(neighbour->status == NOT_REVEALED){
                    auto rowColPair = QStringLiteral("%1,%2").arg(row-1).arg(col-1);
                    notRevealedList->append(rowColPair);
                }
            }
            // If there is a neighbour that is safe to open, save it to the last hint and change its icon to green icon.
            if(currentBtn->neigbourBombs == revealedCount && notRevealedList->size() > 0){
                for(int i=0; i < notRevealedList->size(); i++){
                    QStringList rowColList = notRevealedList->at(i).split(u',');
                    int safeRow = rowColList.at(0).toInt();
                    int safeCol = rowColList.at(1).toInt();
                    ImageButton* safeBtn = qobject_cast<ImageButton*>(itemAtPosition(safeRow,safeCol)->widget());
                    lastHint = safeBtn;
                    safeBtn->setIcon(icons[11]);
                    safeBtn->iconIdx = 11;
                    hintMarked = true;
                    break;
                }
            }
            delete notRevealedList;
            if(hintMarked){
                break;
            }
        }
        if(hintMarked){
            break;
        }
    }
}
// When this slot is called, it changes the icon of the clicked cell to flag.
void MyGrid::onRightClick(unsigned short row, unsigned short col){
    ImageButton* imgBtn = qobject_cast<ImageButton*>(this->itemAtPosition(row,col)->widget());
    if(imgBtn->iconIdx == 9){
        imgBtn->setIcon(this->icons[10]);
        imgBtn->iconIdx = 10;
    }else if(imgBtn->iconIdx == 10){
        imgBtn->setIcon(this->icons[9]);
        imgBtn->iconIdx = 9;
    }
    update();
}
/*
    If a cell is clicked for the first time we need to start the game. This method creates the bombs and assign them to cells randomly. First clicked cell can not be a mine.
    Other than that general idea is simple, call openCells method on the clicked cell,if the clicked cell is a mine, emit gameLost signal,
    if the game is in a win condition emit gameWin signal.
*/
void MyGrid::onLeftClick(unsigned short row, unsigned short col){
    if(gameStatus == INIT){
        emit gameStarted();
        gameStatus = PLAYING;
        std::srand(std::time(0));
        int currentBombs = 0;
        while(currentBombs < bombs){
            if(rowSize == 0 || colSize == 0){
                break;
            }
            int randomRow = std::rand() % rowSize;
            int randomCol = std::rand() % colSize;
            if(randomRow - row <= 1 && randomCol - col <= 1 && randomRow - row >= -1 && randomCol - col >= -1){
                continue;
            };
            ImageButton* imgBtn = qobject_cast<ImageButton*>(itemAtPosition(randomRow,randomCol)->widget());
            QString rowColPair = QString::fromStdString(std::to_string(randomRow) + "," + std::to_string(randomCol));
            if(!bombLocations->contains(rowColPair)){
                currentBombs++;
                bombLocations->insert(rowColPair,true);
                imgBtn->bomb = true;

                if(isInGrid(randomRow-1,randomCol)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow-1,randomCol)->widget());
                    neighbour->neigbourBombs = neighbour->neigbourBombs + 1;
                }
                if(isInGrid(randomRow-1,randomCol+1)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow-1,randomCol+1)->widget());
                   neighbour->neigbourBombs = neighbour->neigbourBombs + 1;;
                }
                if(isInGrid(randomRow,randomCol+1)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow,randomCol+1)->widget());
                    neighbour->neigbourBombs = neighbour->neigbourBombs + 1;
                }
                if(isInGrid(randomRow+1,randomCol+1)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow+1,randomCol+1)->widget());
                    neighbour->neigbourBombs = neighbour->neigbourBombs + 1;
                }
                if(isInGrid(randomRow+1,randomCol)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow+1,randomCol)->widget());
                    neighbour->neigbourBombs = neighbour->neigbourBombs + 1;
                }
                if(isInGrid(randomRow+1,randomCol-1)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow+1,randomCol-1)->widget());
                    neighbour->neigbourBombs = neighbour->neigbourBombs + 1;
                }
                if(isInGrid(randomRow,randomCol-1)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow,randomCol-1)->widget());
                    neighbour->neigbourBombs = neighbour->neigbourBombs + 1;
                }
                if(isInGrid(randomRow -1,randomCol-1)){
                    ImageButton* neighbour = qobject_cast<ImageButton*>(itemAtPosition(randomRow-1,randomCol-1)->widget());
                    neighbour->neigbourBombs = neighbour->neigbourBombs + 1;
                }
            }
        }
        openCells(row,col);
    }
    else if(gameStatus == PLAYING){
        ImageButton* imgBtn = qobject_cast<ImageButton*>(this->itemAtPosition(row,col)->widget());
        if(imgBtn->bomb){
            imgBtn->setIcon(this->icons[12]);
            imgBtn->iconIdx = 12;
            imgBtn->blockSignals(true);
            emit gameLost();
            QMessageBox msgBox;
            msgBox.setText("You lose!");
            msgBox.exec();
            msgBox.setStandardButtons(QMessageBox::Ok);
        }else{
            openCells(row,col);
        }
    }
    int openCellCount = 0;
    for(int row = 0; row<rowSize; row++){
        for(int col=0; col < colSize; col++){
            ImageButton* currBtn = qobject_cast<ImageButton*>(itemAtPosition(row,col)->widget());
            if(currBtn->status == OPEN)
                openCellCount++;
        }
    }
    scoreLabel->setText("Score: " + QString::number(openCellCount));
    if (rowSize * colSize - openCellCount == bombs){
        emit gameWon();
        QMessageBox msgBox;
        msgBox.setText("You win!");
        msgBox.exec();
        msgBox.setStandardButtons(QMessageBox::Ok);
    }
}
// When restart button is clicked make buttons clickable again, change icons of the cells and reset bomb locations.
void MyGrid::onRestart(){
    bombLocations = new QHash<QString, bool>();
    gameStatus = INIT;
    scoreLabel->setText("Score: 0");
    for(int row = 0; row< this->rowSize; row++){
        for(int col=0; col< this->colSize; col++){
            ImageButton* imgBtn = qobject_cast<ImageButton*>(this->itemAtPosition(row,col)->widget());
            imgBtn->setIcon(this->icons[9]);
            imgBtn->iconIdx = 9;
            imgBtn->blockSignals(false);
            imgBtn->status = NOT_REVEALED;
            imgBtn->bomb = false;
            imgBtn->neigbourBombs = 0;
        }
    }
}
// When this slot is called, it makes everything, except the restart button, unclickable and reveals all of the mines.
void MyGrid::onGameFinish(){
    for(int row = 0; row< rowSize; row++){
        for(int col=0; col< colSize; col++){
            ImageButton* imgBtn = qobject_cast<ImageButton*>(itemAtPosition(row,col)->widget());
            if(imgBtn->bomb){
                imgBtn->setIcon(icons[12]);
            }
            imgBtn->blockSignals(true);
        }
    }
}
