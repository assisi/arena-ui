/*! \file utils.h
 *  Definitions of utility functions that don't quite belong anywhere.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#include <QList>

class QGraphicsItem;

/*!
 * \brief Utility function for setpoint dialogs.
 *
 * Checks if the selected entity is a group.
 *
 * \param group Entity to check.
 *
 * \return true if the entity is a group.
 */

bool isGroup(const QList<QGraphicsItem *>& group);

/*!
 * \brief Utility function for checking values.
 *
 * Checks if a value is within the given bounds.
 *
 */

template <typename T>
    bool isInBounds(const T& value, const T& low, const T& high)
    {
        return !(value < low) && !(high < value);
    }

#endif // UTILS_H

