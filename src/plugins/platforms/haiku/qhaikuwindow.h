/***************************************************************************
**
** Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Tobias Koenig <tobias.koenig@kdab.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QHAIKUWINDOW_H
#define QHAIKUWINDOW_H

#include <qpa/qplatformwindow.h>

#include <Window.h>

QT_BEGIN_NAMESPACE

class HaikuWindowProxy : public QObject, public BWindow
{
    Q_OBJECT

public:
    explicit HaikuWindowProxy(QWindow *window, const QRect &rect, QObject *parent = Q_NULLPTR);

    void FrameMoved(BPoint pos) Q_DECL_OVERRIDE;
    void FrameResized(float width, float height) Q_DECL_OVERRIDE;
    void WindowActivated(bool activated) Q_DECL_OVERRIDE;
    void Minimize(bool minimize) Q_DECL_OVERRIDE;
    void Zoom(BPoint pos, float width, float height) Q_DECL_OVERRIDE;
    bool QuitRequested() Q_DECL_OVERRIDE;

    void zoomByQt();

Q_SIGNALS:
    void moved(const QPoint &pos);
    void resized(const QSize &size, bool zoomInProgress);
    void windowActivated(bool activated);
    void minimized(bool minimize);
    void zoomed();
    void quitRequested();

private:
    bool m_qtCalledZoom;
    bool m_zoomInProgress;
};

class QHaikuWindow : public QObject, public QPlatformWindow
{
    Q_OBJECT

public:
    explicit QHaikuWindow(QWindow *window);
    virtual ~QHaikuWindow();

    void setGeometry(const QRect &rect) Q_DECL_OVERRIDE;
    QMargins frameMargins() const Q_DECL_OVERRIDE;
    void setVisible(bool visible) Q_DECL_OVERRIDE;

    bool isExposed() const Q_DECL_OVERRIDE;
    bool isActive() const Q_DECL_OVERRIDE;

    WId winId() const Q_DECL_OVERRIDE;
    BWindow* nativeHandle() const;

    void requestActivateWindow() Q_DECL_OVERRIDE;
    void setWindowState(Qt::WindowState state) Q_DECL_OVERRIDE;
    void setWindowFlags(Qt::WindowFlags flags) Q_DECL_OVERRIDE;

    void setWindowTitle(const QString &title) Q_DECL_OVERRIDE;

    void propagateSizeHints() Q_DECL_OVERRIDE;

protected:
    HaikuWindowProxy *m_window;

private Q_SLOTS:
    void haikuWindowMoved(const QPoint &pos);
    void haikuWindowResized(const QSize &size, bool zoomInProgress);
    void haikuWindowActivated(bool activated);
    void haikuWindowMinimized(bool minimize);
    void haikuWindowZoomed();
    void haikuWindowQuitRequested();

    void haikuMouseEvent(const QPoint &localPosition, const QPoint &globalPosition, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers, Qt::MouseEventSource source);
    void haikuWheelEvent(const QPoint &localPosition, const QPoint &globalPosition, int delta, Qt::Orientation orientation, Qt::KeyboardModifiers modifiers);
    void haikuKeyEvent(QEvent::Type type, int key, Qt::KeyboardModifiers modifiers, const QString &text);
    void haikuEnteredView();
    void haikuExitedView();
    void haikuDrawRequest(const QRect &rect);

private:
    Qt::WindowState m_windowState;
};

QT_END_NAMESPACE

#endif
