/**
 * @file VersionNumber.h
 *
 * @author Dan Keenan
 * @date 4/18/26
 * @copyright GPL-3.0
 */

#ifndef QTCHOOSER_QTCHOOSERLIB_VERSIONNUMBER_H
#define QTCHOOSER_QTCHOOSERLIB_VERSIONNUMBER_H

#include <string>
#include <vector>

namespace qtchooser {

/**
 * Store a major.minor.patch version number.
 */
class VersionNumber
{
public:
    auto operator<=>(const VersionNumber &) const = default;

    explicit VersionNumber() = default;
    explicit VersionNumber(const std::string &str);

    [[nodiscard]] unsigned int major() const { return getPart(0); }
    [[nodiscard]] unsigned int minor() const { return getPart(1); }
    [[nodiscard]] unsigned int patch() const { return getPart(2); }

    [[nodiscard]] std::string string() const;
    [[nodiscard]] bool empty() const { return parts_.empty(); }

private:
    std::vector<unsigned int> parts_;

    [[nodiscard]] unsigned int getPart(const decltype(parts_)::size_type ix) const
    {
        if (ix >= parts_.size()) {
            return 0;
        }
        return parts_[ix];
    }
};

} // namespace qtchooser

#endif //QTCHOOSER_QTCHOOSERLIB_VERSIONNUMBER_H
