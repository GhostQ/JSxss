/****************************************************************************
**
** Copyright (C) 2011 - 2012 Research In Motion
** Contact: http://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
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

#ifndef QQNXBUFFER_H
#define QQNXBUFFER_H

#include <QtGui/QImage>

#include <screen/screen.h>

QT_BEGIN_NAMESPACE

class QQnxBuffer
{
public:
    QQnxBuffer();
    QQnxBuffer(screen_buffer_t buffer);
    QQnxBuffer(const QQnxBuffer &other);
    virtual ~QQnxBuffer();

    screen_buffer_t nativeBuffer() const { return m_buffer; }
    const QImage *image() const { return (m_buffer != 0) ? &m_image : 0; }
    QImage *image() { return (m_buffer != 0) ? &m_image : 0; }

    QRect rect() const { return m_image.rect(); }

    void invalidateInCache();

private:
    screen_buffer_t m_buffer;
    QImage m_image;
};

QT_END_NAMESPACE

#endif // QQNXBUFFER_H
