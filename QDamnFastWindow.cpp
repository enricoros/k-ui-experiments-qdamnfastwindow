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

#include "QDamnFastWindow.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QPlastiqueStyle>

#define TITLEBAR_HEIGHT 20
#define SHADOW_SIZE 16
#define BORDER_SIZE 0

QDamnFastWindow::QDamnFastWindow(QWidget *parent)
  : QWidget(parent)
  , m_centralWidget(0)
  , m_borderWidth(BORDER_SIZE)
  , m_resizing(false)
  , m_moving(false)
  , m_resizeAnchor(0)
{
    // use a light style
    QApplication::setStyle(new QPlastiqueStyle);

    // windowless and transparent
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);

    // size policy
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

    // mouse tracking (for resizing)
    setMouseTracking(true);

    // load tiles
    for (int i = 0; i < 9; i++)
        m_shadowTile[i].load(QLatin1String(":/tiles/") + QString::number(i) + QLatin1String(".png"));
}

QDamnFastWindow::~QDamnFastWindow()
{
    delete m_centralWidget;
}

void QDamnFastWindow::setCentralWidget(QWidget * widget)
{
    delete m_centralWidget;
    m_centralWidget = widget;
    if (m_centralWidget) {
        m_centralWidget->setParent(this);
        m_centralWidget->setGeometry(m_centralRect);
        m_centralWidget->show();
    }
}

QWidget * QDamnFastWindow::centralWidget() const
{
    return m_centralWidget;
}

QSize QDamnFastWindow::minimumSizeHint()
{
    return m_centralWidget ? m_centralWidget->minimumSizeHint().expandedTo(QSize(64, 64)) : QSize(64, 64);
}

void QDamnFastWindow::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        // eventually start move
        const QPoint pos = event->pos();
        if (m_titleRect.contains(pos)) {
            m_moving = true;
            m_anchorOffset = pos;
            setCursor(Qt::SizeAllCursor);
            return;
        }

        // eventually start resize
        for (int i = 0; i < 9; i++) {
            if (i == 4 || !m_shadowRect[i].contains(pos))
                continue;
            m_resizing = true;
            m_beginGeometry = geometry();
            m_anchorOffset = pos;
            m_resizeAnchor = i;
            return;
        }
    }
}

void QDamnFastWindow::mouseMoveEvent(QMouseEvent * event)
{
    if (m_moving) {
        // window movement
        move(event->globalPos() - m_anchorOffset);
    } else if (m_resizing) {
        // window resize
        const QPoint pos = event->globalPos();
        QRect newGeometry = m_beginGeometry;
        switch (m_resizeAnchor) {
            case 0: newGeometry.setTopLeft(pos); break;
            case 1: newGeometry.setTop(pos.y()); break;
            case 2: newGeometry.setTopRight(pos); break;
            case 3: newGeometry.setLeft(pos.x()); break;
            case 5: newGeometry.setRight(pos.x()); break;
            case 6: newGeometry.setBottomLeft(pos); break;
            case 7: newGeometry.setBottom(pos.y()); break;
            case 8: newGeometry.setBottomRight(pos); break;
        }
        QSize minSize = minimumSizeHint();
        if (newGeometry.width() < minSize.width())
            newGeometry.setWidth(minSize.width());
        if (newGeometry.height() < minSize.height())
            newGeometry.setHeight(minSize.height());
        if (newGeometry != geometry())
            setGeometry(newGeometry);
    } else {
        // just hovering: change cursors
        const QPoint pos = event->pos();
        if (m_shadowRect[0].contains(pos) || m_shadowRect[8].contains(pos))
            setCursor(Qt::SizeFDiagCursor);
        else if (m_shadowRect[2].contains(pos) || m_shadowRect[6].contains(pos))
            setCursor(Qt::SizeBDiagCursor);
        else if (m_shadowRect[1].contains(pos) || m_shadowRect[7].contains(pos))
            setCursor(Qt::SizeVerCursor);
        else if (m_shadowRect[3].contains(pos) || m_shadowRect[5].contains(pos))
            setCursor(Qt::SizeHorCursor);
        else if (m_titleRect.contains(pos))
            setCursor(Qt::SizeAllCursor);
        else
            setCursor(Qt::ArrowCursor);
    }
}

void QDamnFastWindow::mouseReleaseEvent(QMouseEvent *)
{
    m_resizing = false;
    m_moving = false;
    setCursor(Qt::ArrowCursor);
}

void QDamnFastWindow::paintEvent(QPaintEvent * event)
{
    QPainter windowPainter(this);
    windowPainter.setCompositionMode(QPainter::CompositionMode_Source);
    QColor windowColor = palette().color(QPalette::Window);
    QRect exposedRect = event->rect();

    // shadow
    windowPainter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, false);
    for (int i = 0; i < 9; i++)
        if (m_shadowRect[i].intersects(exposedRect) && i != 4)
            windowPainter.drawPixmap(m_shadowRect[i], m_shadowTile[i]);

    // title pixmap
    if (m_titleRect.intersects(exposedRect)) {
        windowPainter.setFont(QFont(QLatin1String("Sans Serif"), 9, QFont::Bold));
        windowPainter.drawText(m_titleRect, Qt::AlignCenter, windowTitle());
    }

    // border
    if (BORDER_SIZE) {
        int halfPen = m_borderWidth / 2;
        windowPainter.setPen(QPen(windowColor, m_borderWidth));
        windowPainter.setBrush(Qt::NoBrush);
        windowPainter.drawRect(m_borderRect.adjusted(halfPen, halfPen, -halfPen, -halfPen));
    }

    // inner fill (TODO: can be removed using a non-transparent ARGB filling)
    windowPainter.fillRect(m_centralRect.intersected(exposedRect), windowColor);
}

void QDamnFastWindow::resizeEvent(QResizeEvent *)
{
    calcRects(rect());
    if (m_centralWidget)
        m_centralWidget->setGeometry(m_centralRect);
}

void QDamnFastWindow::calcRects(const QRect & baseRect)
{
    const int left      = baseRect.left();
    const int top       = baseRect.top();
    const int width     = baseRect.width();
    const int height    = baseRect.height();

    // shadow rects
    const int sx[3] = { left, left + SHADOW_SIZE, left + width - SHADOW_SIZE };
    const int sy[3] = { top, top + SHADOW_SIZE, top + height - SHADOW_SIZE };
    const int sInnerWidth = sx[2] - sx[1];
    const int sInnerHeight = sy[2] - sy[1];
    m_shadowRect[0] = QRect(sx[0], sy[0], SHADOW_SIZE, SHADOW_SIZE);
    m_shadowRect[1] = QRect(sx[1], sy[0], sInnerWidth, SHADOW_SIZE);
    m_shadowRect[2] = QRect(sx[2], sy[0], SHADOW_SIZE, SHADOW_SIZE);
    m_shadowRect[3] = QRect(sx[0], sy[1], SHADOW_SIZE, sInnerHeight);
    m_shadowRect[4] = QRect(sx[1], sy[1], sInnerWidth, sInnerHeight);
    m_shadowRect[5] = QRect(sx[2], sy[1], SHADOW_SIZE, sInnerHeight);
    m_shadowRect[6] = QRect(sx[0], sy[2], SHADOW_SIZE, SHADOW_SIZE);
    m_shadowRect[7] = QRect(sx[1], sy[2], sInnerWidth, SHADOW_SIZE);
    m_shadowRect[8] = QRect(sx[2], sy[2], SHADOW_SIZE, SHADOW_SIZE);
    
    // other rects
    m_borderRect = m_shadowRect[4];
    if (!windowTitle().isEmpty()) {
        m_titleRect = QRect(sx[1] + BORDER_SIZE, sy[1] + BORDER_SIZE, sInnerWidth - BORDER_SIZE - BORDER_SIZE, TITLEBAR_HEIGHT);
        m_centralRect = QRect(sx[1] + BORDER_SIZE, sy[1]+ BORDER_SIZE + TITLEBAR_HEIGHT, sInnerWidth - BORDER_SIZE - BORDER_SIZE, sInnerHeight - BORDER_SIZE - BORDER_SIZE - TITLEBAR_HEIGHT);
    } else {
        m_titleRect = QRect();
        m_centralRect = QRect(sx[1] + BORDER_SIZE, sy[1]+ BORDER_SIZE, sInnerWidth - BORDER_SIZE - BORDER_SIZE, sInnerHeight - BORDER_SIZE - BORDER_SIZE);
    }
}
