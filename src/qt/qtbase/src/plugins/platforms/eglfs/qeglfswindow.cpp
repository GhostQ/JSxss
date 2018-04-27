/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
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

#include <QtCore/qtextstream.h>
#include <qpa/qwindowsysteminterface.h>
#include <qpa/qplatformintegration.h>
#include <private/qguiapplication_p.h>
#include <QtGui/private/qopenglcontext_p.h>
#include <QtGui/QOpenGLContext>
#include <QtPlatformSupport/private/qeglplatformcursor_p.h>
#include <QtPlatformSupport/private/qeglconvenience_p.h>

#include "qeglfswindow.h"
#include "qeglfshooks.h"

#include <QtDebug>

QT_BEGIN_NAMESPACE

QEglFSWindow::QEglFSWindow(QWindow *w)
    : QEGLPlatformWindow(w)
    , m_surface(0)
    , m_window(0)
    , m_flags(0)
{
}

QEglFSWindow::~QEglFSWindow()
{
    destroy();
}

void QEglFSWindow::create()
{
    if (m_flags.testFlag(Created))
        return;

    QEGLPlatformWindow::create();

    m_flags = Created;

    if (window()->type() == Qt::Desktop)
        return;

    // Stop if there is already a window backed by a native window and surface. Additional
    // raster windows will not have their own native window, surface and context. Instead,
    // they will be composited onto the root window's surface.
    QEglFSScreen *screen = this->screen();
    QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    if (screen->primarySurface() != EGL_NO_SURFACE) {
        if (isRaster() && compositor->targetWindow()) {
            m_format = compositor->targetWindow()->format();
            return;
        }

#if !defined(Q_OS_ANDROID) || defined(Q_OS_ANDROID_NO_SDK)
        // We can have either a single OpenGL window or multiple raster windows.
        // Other combinations cannot work.
        qFatal("EGLFS: OpenGL windows cannot be mixed with others.");
#endif

        return;
    }

    m_flags |= HasNativeWindow;
    setGeometry(QRect()); // will become fullscreen
    QWindowSystemInterface::handleExposeEvent(window(), QRect(QPoint(0, 0), geometry().size()));

    EGLDisplay display = static_cast<QEglFSScreen *>(screen)->display();
    QSurfaceFormat platformFormat = qt_egl_device_integration()->surfaceFormatFor(window()->requestedFormat());
    m_config = QEglFSIntegration::chooseConfig(display, platformFormat);
    m_format = q_glFormatFromConfig(display, m_config, platformFormat);

    resetSurface();

    screen->setPrimarySurface(m_surface);

    if (isRaster()) {
        QOpenGLContext *context = new QOpenGLContext(QGuiApplication::instance());
        context->setShareContext(qt_gl_global_share_context());
        context->setFormat(m_format);
        context->setScreen(window()->screen());
        if (!context->create())
            qFatal("EGLFS: Failed to create compositing context");
        compositor->setTarget(context, window());
    }
}

void QEglFSWindow::destroy()
{
    QEglFSScreen *screen = this->screen();
    if (m_flags.testFlag(HasNativeWindow)) {
        QEGLPlatformCursor *cursor = qobject_cast<QEGLPlatformCursor *>(screen->cursor());
        if (cursor)
            cursor->resetResources();

        if (screen->primarySurface() == m_surface)
            screen->setPrimarySurface(EGL_NO_SURFACE);

        invalidateSurface();
    }

    m_flags = 0;
    QOpenGLCompositor::instance()->removeWindow(this);
}

// The virtual functions resetSurface and invalidateSurface may get overridden
// in derived classes, for example in the Android port, to perform the native
// window and surface creation differently.

void QEglFSWindow::invalidateSurface()
{
    if (m_surface != EGL_NO_SURFACE) {
        EGLDisplay display = static_cast<QEglFSScreen *>(screen())->display();
        eglDestroySurface(display, m_surface);
        m_surface = EGL_NO_SURFACE;
    }
    qt_egl_device_integration()->destroyNativeWindow(m_window);
    m_window = 0;
}

void QEglFSWindow::resetSurface()
{
    QEglFSScreen *nativeScreen = static_cast<QEglFSScreen *>(screen());
    EGLDisplay display = nativeScreen->display();
    m_window = qt_egl_device_integration()->createNativeWindow(this, nativeScreen->geometry().size(), m_format);
    m_surface = eglCreateWindowSurface(display, m_config, m_window, NULL);
    if (m_surface == EGL_NO_SURFACE) {
        EGLint error = eglGetError();
        eglTerminate(display);
        qFatal("EGL Error : Could not create the egl surface: error = 0x%x\n", error);
    }
}

void QEglFSWindow::setVisible(bool visible)
{
    QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    QList<QOpenGLCompositorWindow *> windows = compositor->windows();
    QWindow *wnd = window();

    if (wnd->type() != Qt::Desktop) {
        if (visible) {
            compositor->addWindow(this);
        } else {
            compositor->removeWindow(this);
            windows = compositor->windows();
            if (windows.size())
                windows.last()->sourceWindow()->requestActivate();
        }
    }

    QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));

    if (visible)
        QWindowSystemInterface::flushWindowSystemEvents();
}

void QEglFSWindow::setGeometry(const QRect &r)
{
    QRect rect;
    bool forceFullscreen = m_flags.testFlag(HasNativeWindow);
    if (forceFullscreen)
        rect = screen()->availableGeometry();
    else
        rect = r;

    QPlatformWindow::setGeometry(rect);

    // if we corrected the size, trigger a resize event
    if (rect != r)
        QWindowSystemInterface::handleGeometryChange(window(), rect, r);
}

QRect QEglFSWindow::geometry() const
{
    // For yet-to-become-fullscreen windows report the geometry covering the entire
    // screen. This is particularly important for Quick where the root object may get
    // sized to some geometry queried before calling create().
    if (!m_flags.testFlag(Created) && screen()->primarySurface() == EGL_NO_SURFACE)
        return screen()->availableGeometry();

    return QPlatformWindow::geometry();
}

void QEglFSWindow::requestActivateWindow()
{
    if (window()->type() != Qt::Desktop)
        QOpenGLCompositor::instance()->moveToTop(this);

    QWindow *wnd = window();
    QWindowSystemInterface::handleWindowActivated(wnd);
    QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));
}

void QEglFSWindow::raise()
{
    QWindow *wnd = window();
    if (wnd->type() != Qt::Desktop) {
        QOpenGLCompositor::instance()->moveToTop(this);
        QWindowSystemInterface::handleExposeEvent(wnd, QRect(QPoint(0, 0), wnd->geometry().size()));
    }
}

void QEglFSWindow::lower()
{
    QOpenGLCompositor *compositor = QOpenGLCompositor::instance();
    QList<QOpenGLCompositorWindow *> windows = compositor->windows();
    if (window()->type() != Qt::Desktop && windows.count() > 1) {
        int idx = windows.indexOf(this);
        if (idx > 0) {
            compositor->changeWindowIndex(this, idx - 1);
            QWindowSystemInterface::handleExposeEvent(windows.last()->sourceWindow(),
                                                      QRect(QPoint(0, 0), windows.last()->sourceWindow()->geometry().size()));
        }
    }
}

EGLSurface QEglFSWindow::surface() const
{
    return m_surface != EGL_NO_SURFACE ? m_surface : screen()->primarySurface();
}

QSurfaceFormat QEglFSWindow::format() const
{
    return m_format;
}

EGLNativeWindowType QEglFSWindow::eglWindow() const
{
    return m_window;
}

QEglFSScreen *QEglFSWindow::screen() const
{
    return static_cast<QEglFSScreen *>(QPlatformWindow::screen());
}

QT_END_NAMESPACE
