/*
 * Copyright (C) 2015, Simon Fuhrmann
 * TU Darmstadt - Graphics, Capture and Massively Parallel Computing
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-Clause license. See the LICENSE.txt file for details.
 */

#include <ctime>
#include <iostream>

#include "ogl/opengl.h"
#include "ogl/events.h"
#include "glwidget.h"

#if QT_VERSION >= 0x050000
#   include <QWindow>
#endif

GLWidget::GLWidget (QWidget *parent)
    : QGLWidget(parent)
    , context(nullptr)
    , gl_width(0)
    , gl_height(0)
    , cx_init(false)
{
    this->setFocusPolicy(Qt::ClickFocus);
    this->makeCurrent();

    /* This timer triggers a repaint after all events in the window system's
     * event queue have been processed. Thus a snappy 3D view is provided. */
    this->repaint_timer = new QTimer(this);
    this->repaint_timer->setSingleShot(true);
    connect (this->repaint_timer, SIGNAL (timeout()), this, SLOT (repaint()));
}

/* ---------------------------------------------------------------- */

GLWidget::~GLWidget (void)
{
}

/* ---------------------------------------------------------------- */

void
GLWidget::initializeGL()
{
}

/* ---------------------------------------------------------------- */

void
GLWidget::resizeGL(int width, int height)
{
    std::cout << "Resizing GL from "
        << this->gl_width << "x" << this->gl_height << " to "
        << width << "x" << height << std::endl;

    this->gl_width = width;
    this->gl_height = height;
    if (this->context != nullptr)
        this->context->resize(width, height);
}

/* ---------------------------------------------------------------- */

void
GLWidget::paintGL()
{
    if (this->context == 0)
        return;

    /* Current context may need initialization. */
    if (this->cx_init)
    {
        if (this->init_set.find(this->context) == this->init_set.end())
        {
            std::cout << "Using OpenGL " << this->format().majorVersion()
                      << '.' << this->format().minorVersion() << " ..."
                      << std::endl;
            this->context->init();
            this->context->resize(this->gl_width, this->gl_height);
            this->init_set.insert(this->context); // Mark initialized
        }
        this->cx_init = false;
    }

    /* Paint it! */
    this->context->paint();
}

/* ---------------------------------------------------------------- */

void
GLWidget::set_context (ogl::Context* context)
{
    this->context = context;
    this->cx_init = true;
}

/* ---------------------------------------------------------------- */

void
GLWidget::mousePressEvent (QMouseEvent *event)
{
    ogl::MouseEvent e;
    e.type = ogl::MOUSE_EVENT_PRESS;
    e.button = (ogl::MouseButton)event->button();
    e.button_mask = event->buttons();
#if QT_VERSION >= 0x050000
    qreal const pixel_ratio = this->windowHandle()->devicePixelRatio();
    e.x = static_cast<int>(event->x() * pixel_ratio);
    e.y = static_cast<int>(event->y() * pixel_ratio);
#else
    e.x = event->x();
    e.y = event->y();
#endif
    this->context->mouse_event(e);
    this->repaint_async();
}

/* ---------------------------------------------------------------- */

void
GLWidget::mouseReleaseEvent (QMouseEvent *event)
{
    ogl::MouseEvent e;
    e.type = ogl::MOUSE_EVENT_RELEASE;
    e.button = (ogl::MouseButton)event->button();
    e.button_mask = event->buttons();
#if QT_VERSION >= 0x050000
    qreal const pixel_ratio = this->windowHandle()->devicePixelRatio();
    e.x = static_cast<int>(event->x() * pixel_ratio);
    e.y = static_cast<int>(event->y() * pixel_ratio);
#else
    e.x = event->x();
    e.y = event->y();
#endif
    this->context->mouse_event(e);
    this->repaint_async();
}

/* ---------------------------------------------------------------- */

void
GLWidget::mouseMoveEvent (QMouseEvent *event)
{
    ogl::MouseEvent e;
    e.type = ogl::MOUSE_EVENT_MOVE;
    e.button = (ogl::MouseButton)event->button();
    e.button_mask = event->buttons();
#if QT_VERSION >= 0x050000
    qreal const pixel_ratio = this->windowHandle()->devicePixelRatio();
    e.x = static_cast<int>(event->x() * pixel_ratio);
    e.y = static_cast<int>(event->y() * pixel_ratio);
#else
    e.x = event->x();
    e.y = event->y();
#endif
    this->context->mouse_event(e);
    this->repaint_async();
}

/* ---------------------------------------------------------------- */

void
GLWidget::wheelEvent (QWheelEvent* event)
{
    ogl::MouseEvent e;
    if (event->delta() < 0)
        e.type = ogl::MOUSE_EVENT_WHEEL_DOWN;
    else
        e.type = ogl::MOUSE_EVENT_WHEEL_UP;
    e.button = ogl::MOUSE_BUTTON_NONE;
    e.button_mask = event->buttons();
#if QT_VERSION >= 0x050000
    qreal const pixel_ratio = this->windowHandle()->devicePixelRatio();
    e.x = static_cast<int>(event->x() * pixel_ratio);
    e.y = static_cast<int>(event->y() * pixel_ratio);
#else
    e.x = event->x();
    e.y = event->y();
#endif
    this->context->mouse_event(e);
    this->repaint_async();
}

/* ---------------------------------------------------------------- */

void
GLWidget::keyPressEvent (QKeyEvent* event)
{
    if (event->isAutoRepeat())
    {
        this->QWidget::keyPressEvent(event);
        return;
    }

    ogl::KeyboardEvent e;
    e.type = ogl::KEYBOARD_EVENT_PRESS;
    e.keycode = event->key();
    this->context->keyboard_event(e);
    this->repaint_async();
}

/* ---------------------------------------------------------------- */

void
GLWidget::keyReleaseEvent (QKeyEvent* event)
{
    if (event->isAutoRepeat())
    {
        this->QWidget::keyReleaseEvent(event);
        return;
    }

    ogl::KeyboardEvent e;
    e.type = ogl::KEYBOARD_EVENT_RELEASE;
    e.keycode = event->key();
    this->context->keyboard_event(e);
    this->repaint_async();
}
