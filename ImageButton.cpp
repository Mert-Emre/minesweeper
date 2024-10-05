#include "ImageButton.h"
#include <QMouseEvent>
// This button is a special type of button that shows an icon on itself and the cell abstraction in the game.
ImageButton::ImageButton(unsigned short row, unsigned short col, unsigned short iconIdx, unsigned short btnSize,const QIcon& btnIcon, QWidget* parent):QPushButton(parent){
    this->row = row;
    this->col = col;
    this->iconIdx = iconIdx;
    this->status = NOT_REVEALED;
    this->bomb = false;
    this->neigbourBombs = 0;
    this->setIcon(btnIcon);
    this->setFixedSize(QSize(btnSize,btnSize));
    this->setIconSize(QSize(btnSize,btnSize));
    setFlat(true);
    update();
}
// This is used to differentiate left and right click to the button.
void ImageButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button()==Qt::RightButton)
        emit rightClicked(this->row,this->col);
    else if(e->button() == Qt::LeftButton)
        emit leftClicked(this->row,this->col);
    QPushButton::mousePressEvent(e);
}
