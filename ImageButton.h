#ifndef IMAGEBUTTON_H
#define IMAGEBUTTON_H

#include <QPushButton>
// For information please check the source file
enum StatusType{OPEN, REVEALED, NOT_REVEALED} ;

class ImageButton : public QPushButton{
    Q_OBJECT
public:
    ImageButton(unsigned short row, unsigned short col, unsigned short iconIdx, unsigned short btnSize,const QIcon& btnIcon, QWidget* parent = 0);
    StatusType status;
    bool bomb;
    unsigned short neigbourBombs;
    unsigned short row;
    unsigned short col;
    unsigned short iconIdx;
private slots:
    void mousePressEvent(QMouseEvent *e);

signals:
    void rightClicked(unsigned short row, unsigned short col);
    void leftClicked(unsigned short row, unsigned short col);


public slots:
};
#endif // IMAGEBUTTON_H
