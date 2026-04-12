/**
 * @file EnvironmentInjectorLinux.h
 *
 * @author Dan Keenan
 * @date 4/12/26
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_ENVIRONMENTINJECTORLINUX_H
#define QTCHOOSER_QTCHOOSERLIB_ENVIRONMENTINJECTORLINUX_H

#include "EnvironmentInjector.h"
#include <unordered_map>

namespace qtchooser {
class EnvironmentInjectorLinux : public EnvironmentInjector
{
public:
    explicit EnvironmentInjectorLinux();

    void setEnv(const std::string &var, const std::string &val) override;
    bool commit() override;

protected:
    [[nodiscard]] std::vector<std::filesystem::path> getUserPath() override;
    void setUserPath(const std::vector<std::filesystem::path> &userPath) override;

private:
    std::unordered_map<std::string, std::string> originalEnv_;
    std::unordered_map<std::string, std::string> env_;
};
} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_ENVIRONMENTINJECTORLINUX_H
