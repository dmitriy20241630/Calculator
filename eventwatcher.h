#ifndef EVENTWATCHER_H
#define EVENTWATCHER_H

#include <QObject>

class EventWatcher : public QObject
{
    Q_OBJECT
public:
    explicit EventWatcher(QObject* = nullptr);
    bool eventFilter(QObject*, QEvent*) override;

signals:
    void keyDown(bool, int);
};

#endif // EVENTWATCHER_H
