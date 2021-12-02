/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DoubleClickButton.h
 * Author: Aaron
 *
 * Created on 27 de noviembre de 2017, 12:00
 */

#ifndef DOUBLECLICKBUTTON_H
#define DOUBLECLICKBUTTON_H

#include <QPushButton>

class DoubleClickButton : public QPushButton
{
    Q_OBJECT
public:
    DoubleClickButton(QWidget *parent=0);
    DoubleClickButton(const QString &text, QWidget *parent=0);
    
signals:
    void doubleClicked(void); 

private:
    void mouseDoubleClickEvent(QMouseEvent * event);
};

#endif /* DOUBLECLICKBUTTON_H */

