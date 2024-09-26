#include "eventwatcher.h"
#include <QEvent>
#include <QKeyEvent>

EventWatcher::EventWatcher(QObject *parent)
    : QObject{parent}
{
    if (parent)
        parent->installEventFilter(this);
}

bool EventWatcher::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    if (event->type() == QEvent::KeyPress)
        emit keyDown(true, dynamic_cast<QKeyEvent*>(event)->key());
    else
        if (event->type() == QEvent::KeyRelease)
            emit keyDown(false, dynamic_cast<QKeyEvent*>(event)->key());

    return false;
}
