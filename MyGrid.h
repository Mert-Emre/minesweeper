#ifndef MYGRID_H
#define MYGRID_H

#include <QGridLayout>
#include <QIcon>
#include <QHash>
#include "ImageButton.h"
#include "qlabel.h"

enum GameStatus{WIN, LOSE, INIT,PLAYING};
// For information plesase check the source file.
class MyGrid : public QGridLayout
{
    Q_OBJECT
public:
    MyGrid(unsigned short rowSize, unsigned short colSize, unsigned short iconSize,unsigned short bombs, QLabel* scoreLabel);
    void openCells(unsigned short row, unsigned short col);
    bool isInGrid(short row, short col);
    ImageButton* lastHint;
    GameStatus gameStatus;
    unsigned short rowSize;
    unsigned short colSize;
    unsigned short bombs;
    QHash<QString, bool> *bombLocations;
    QIcon icons[14];
    QLabel* scoreLabel;

public slots:
    void onRightClick(unsigned short row, unsigned short col);
    void onLeftClick(unsigned short row, unsigned short col);
    void onRestart();
    void onHint();
    void onGameFinish();
signals:
    void gameLost();
    void gameWon();
    void gameStarted();
};

#endif // MYGRID_H
