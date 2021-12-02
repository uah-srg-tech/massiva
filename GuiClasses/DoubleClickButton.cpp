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
#include "DoubleClickButton.h"

#define UNUSED(x) (void)(x)

DoubleClickButton::DoubleClickButton(QWidget *parent) : QPushButton(parent)
{
}

DoubleClickButton::DoubleClickButton(const QString &text, QWidget *parent)
        : QPushButton(text, parent)
{
}

void DoubleClickButton::mouseDoubleClickEvent(QMouseEvent * event)
{
    UNUSED(event);
    emit doubleClicked();
}