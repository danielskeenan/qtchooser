/**
 * @file EnvironmentInjector.h
 *
 * @author Dan Keenan
 * @date 4/5/2026
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_ENVIRONMENTINJECTOR_H
#define QTCHOOSER_ENVIRONMENTINJECTOR_H

#include <filesystem>
#include <vector>

namespace qtchooser {

/**
 * Persist env vars in a platform-specific way.
 */
class EnvironmentInjector
{
public:
    explicit EnvironmentInjector() = default;
    virtual ~EnvironmentInjector() = default;

    static std::unique_ptr<EnvironmentInjector> get();

    /**
     * Set the persistent environment variable @p var to @p val.
     *
     * @param var
     * @param val
     * @return `TRUE` if the value has changed from its previous value.
     */
    virtual void setEnv(const std::string &var, const std::string &val) = 0;

    /**
     * Add @p path to the system PATH variable.
     * @param path
     * @return `TRUE` if the value has changed from its previous value.
     */
    void addToPath(const std::filesystem::path &path);

    /**
     * Remove @p path from the system PATH variable.
     * @param path
     * @return `TRUE` if the value has changed from its previous value.
     */
    void removeFromPath(const std::filesystem::path &path);

    /**
     * Save the changes to the system.
     */
    virtual bool commit() = 0;

protected:
    /**
     * Get a list of paths the system will search for executables.
     */
    [[nodiscard]] virtual std::vector<std::filesystem::path> getUserPath() = 0;

    /**
     * Set the list of paths the system will search for executables.
     * @param userPath
     */
    virtual void setUserPath(const std::vector<std::filesystem::path> &userPath) = 0;
};

} // namespace qtchooser

#endif //QTCHOOSER_ENVIRONMENTINJECTOR_H
