#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QIcon>
#include <QPushButton>
#include <QSpacerItem>
#include <QMainWindow>
#include "MyGrid.h"
#include "HintButton.h"

int main(int argc, char *argv[]){
    QApplication app(argc,argv);
    QWidget *mw = new QWidget;
    QVBoxLayout *mvb = new QVBoxLayout(mw);
    unsigned short rowSize = 9;
    unsigned short colSize = 9;
    unsigned short iconSize = 32;
    // I change the size of icons. If there are less cells, icons are bigger.
    if(rowSize < 5 || colSize < 5){
        iconSize = 64;
    }
    if(rowSize > 10 || colSize > 10){
        iconSize = 24;
    }

    // This widget is used if there are very few cells, it is needed to place the grid into center. This ensures that the grid is centered in such a case.
    QWidget *controlsRestrictorWidget = new QWidget();
    QLabel *scoreLabel = new QLabel;
    scoreLabel->setText("Score: 0");
    MyGrid* grid = new MyGrid(rowSize,colSize, iconSize,11, scoreLabel);
    QObject::connect(grid, SIGNAL(gameLost()), grid, SLOT(onGameFinish()));
    QObject::connect(grid, SIGNAL(gameWon()), grid, SLOT(onGameFinish()));
    // Centering the grid.
    controlsRestrictorWidget->setLayout(grid);
    controlsRestrictorWidget->setMaximumWidth(rowSize*iconSize + 24);
    QHBoxLayout *headerHBox = new QHBoxLayout();
    QPushButton *restartBtn = new QPushButton("Restart");
    HintButton *hintBtn = new HintButton();
    QObject::connect(restartBtn, SIGNAL(clicked()), grid, SLOT(onRestart()));
    QObject::connect(restartBtn, SIGNAL(clicked()), hintBtn, SLOT(onDisableReq()));
    QObject::connect(hintBtn, SIGNAL(clicked()), grid, SLOT(onHint()));
    QObject::connect(grid, SIGNAL(gameWon()), hintBtn, SLOT(onDisableReq()));
    QObject::connect(grid, SIGNAL(gameLost()), hintBtn, SLOT(onDisableReq()));
    QObject::connect(grid, SIGNAL(gameStarted()), hintBtn, SLOT(onEnableReq()));

    headerHBox->addWidget(scoreLabel);
    headerHBox->addWidget(restartBtn);
    headerHBox->addWidget(hintBtn);

    mvb->addLayout(headerHBox);
    QSpacerItem *spacer = new QSpacerItem(0,10,QSizePolicy::Expanding,QSizePolicy::Expanding);
    mvb->addSpacerItem(spacer);
    mvb->addWidget(controlsRestrictorWidget);
    mvb->setAlignment(controlsRestrictorWidget,Qt::AlignHCenter);

    mvb->addLayout(grid);
    mw->setWindowTitle("minesweeper");
    mw->resize(std::max(300, rowSize*iconSize + 24),colSize*iconSize +20);
    mw->show();
    return app.exec();
}
