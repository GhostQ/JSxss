/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SCROLLER_P_H
#define SCROLLER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QPoint>
#include <QTimer>
#include <QTime>

#include "scroller.h"

class AbstractScrollArea;

class ScrollerPrivate
{
    Q_DECLARE_PUBLIC(Scroller)

public:
    enum State  {
        Stopped,
        Started,
        ManualScrolling,
        AutoScrolling
    };

    ScrollerPrivate(Scroller *scroller);
    ~ScrollerPrivate();
    void stopScrolling();
    bool eventFilter(QObject *obj, QEvent *ev);

    AbstractScrollArea *m_scrollArea;
    qreal m_scrollFactor;
    QPoint m_cursorPos;
    QPointF m_speed;
    State m_state;
    QTime m_lastCursorTime;
    QTime m_lastFrameTime;
    QTimer m_scrollTimer;
    int m_scrollSlowAccum;

private Q_SLOTS:

    void updateScrolling();

private:

    Q_DISABLE_COPY(ScrollerPrivate)
    Scroller * const q_ptr;
    QPointF mapToScrollArea(const QPoint &point);
    QWidget *m_eventViewport;
};

#endif // SCROLLER_P_H
