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
#include <QObject>

namespace qtchooser {

/**
 * Select the given Qt as the system default.
 */
class QtChooser : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    /**
     * Update the system to use the Qt described in @p info.
     * @param info
     */
    void choose(const QtInfo &info);

Q_SIGNALS:
    /**
     * Emitted when environment variables have changed.
     *
     * On some platforms, this requires the user logout and back in.
     */
    void envVarsChanged();
};

} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSER_H
