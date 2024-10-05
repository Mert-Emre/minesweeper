#ifndef HINTBUTTON_H
#define HINTBUTTON_H

#include <QPushButton>
// For information please check the source file.
class HintButton :public QPushButton
{
    Q_OBJECT
public:
    HintButton(QWidget* parent = 0);
public slots:
    void onDisableReq();
    void onEnableReq();
};

#endif // HINTBUTTON_H
