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

#ifndef QDAMNFASTWINDOW_H
#define QDAMNFASTWINDOW_H

#include <QtGui/QWidget>

/**
  \brief Window decoration emulator. Makes resizing fast!
*/
class QDamnFastWindow : public QWidget
{
    Q_OBJECT
    public:
        QDamnFastWindow(QWidget *parent = 0);
        ~QDamnFastWindow();

        // QMainWindow like
        void setCentralWidget(QWidget * widget);
        QWidget * centralWidget() const;

    protected:
        // ::QWidget
        QSize minimumSizeHint();
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void paintEvent(QPaintEvent *);
        void resizeEvent(QResizeEvent *);

    private:
        void calcRects(const QRect & baseRect);
        QWidget * m_centralWidget;
        int m_borderWidth;
        QRect m_shadowRect[9];
        QRect m_borderRect;
        QRect m_titleRect;
        QRect m_centralRect;

        QString m_cachedTitleString;
        QPixmap m_cachedTitlePixmap;

        bool m_resizing;
        bool m_moving;
        int m_resizeAnchor;
        QPoint m_anchorOffset;
};

#endif
