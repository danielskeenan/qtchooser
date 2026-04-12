/**
 * @file EnvironmentInjectorWin.h
 *
 * @author Dan Keenan
 * @date 4/12/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_ENVIRONMENTINJECTORWIN_H
#define QTCHOOSER_QTCHOOSERLIB_ENVIRONMENTINJECTORWIN_H

#include "EnvironmentInjector.h"
#include <unordered_map>

namespace qtchooser {
class EnvironmentInjectorWin : public EnvironmentInjector
{
public:
    using EnvironmentInjector::EnvironmentInjector;
    void setEnv(const std::string &var, const std::string &val) override;
    bool commit() override;

protected:
    [[nodiscard]] std::vector<std::filesystem::path> getUserPath() override;
    void setUserPath(const std::vector<std::filesystem::path> &userPath) override;

private:
    std::unordered_map<std::string, std::string> env_;
};
} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_ENVIRONMENTINJECTORWIN_H
