#include <QGraphicsItem>

#include <regex>

#include "qabstractsceneitem.h"
#include "utils.h"

bool isGroup(const QList<QGraphicsItem *>& group)
{
    bool is_group = false;
    if (group.size() > 0)
    {
        if (group.size() > 1)
        {
            is_group = true;
        }
        else if (sCast(group.first())->isGroup())
        {
            is_group = true;
        }
    }

    return is_group;
}

