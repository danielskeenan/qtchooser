/**
 * @file QtChooser.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSER_H
#define QTCHOOSER_QTCHOOSER_H

#include "QtInfo.h"

namespace qtchooser {

/**
 * Select the given Qt as the system default.
 */
class QtChooser
{
public:
    /**
     * Update the system to use the Qt described in @p info.
     * @param info
     * @return TRUE if system params have changed as a result of this action.
     */
    bool choose(const QtInfo &info);
};

} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSER_H
