#include "HintButton.h"
// This class is just created to make the hint button unclickable before the game is started.
HintButton::HintButton(QWidget* parent):QPushButton("Hint",parent){
    setEnabled(false);
}

void HintButton::onDisableReq(){
    setEnabled(false);
}

void HintButton::onEnableReq(){
    setEnabled(true);
}
