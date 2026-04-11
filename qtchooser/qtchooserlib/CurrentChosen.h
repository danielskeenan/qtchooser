/**
 * @file CurrentChosen.h
 *
 * @author Dan Keenan
 * @date 4/6/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_CURRENTCHOSEN_H
#define QTCHOOSER_QTCHOOSERLIB_CURRENTCHOSEN_H

#include "QtInfo.h"
#include <filesystem>

namespace qtchooser {

/**
 * Load information about the last Qt chosen.
 */
class CurrentChosen
{
public:
    explicit CurrentChosen();

    void setInfo(const QtInfo &info);

    [[nodiscard]] const std::filesystem::path &prefix() const { return prefix_; }
    [[nodiscard]] const std::vector<std::filesystem::path> &binDirs() const { return binDirs_; }
    [[nodiscard]] bool isValid() const;

private:
    std::filesystem::path prefix_;
    std::vector<std::filesystem::path> binDirs_;
};

} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_CURRENTCHOSEN_H
