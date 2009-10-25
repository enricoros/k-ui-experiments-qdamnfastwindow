/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2009-2009 by Enrico Ros <enrico.ros@gmail.com>        *
 *   Started on 25 Oct 2009 by root.
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include "QDamnFastWindow.h"
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QDamnFastWindow w;
    w.setWindowTitle("Title");
//    w.setCentralWidget(new QPushButton("Push it!"));
    w.show();

    return a.exec();
}
