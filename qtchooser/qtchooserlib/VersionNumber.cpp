/**
 * @file VersionNumber.cpp
 *
 * @author Dan Keenan
 * @date 4/18/26
 * @copyright GPL-3.0
 */

#include "VersionNumber.h"
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find_iterator.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace qtchooser {

constexpr auto kVersionSep = ".";

VersionNumber::VersionNumber(const std::string &str)
{
    // Get the numeric parts.
    for (auto it
         = boost::algorithm::make_split_iterator(str, boost::algorithm::first_finder(kVersionSep));
         it != decltype(it)();
         ++it) {
        if (boost::algorithm::all(*it, boost::algorithm::is_digit())) {
            parts_.emplace_back(std::stoul(boost::copy_range<std::string>(*it)));
        } else {
            break;
        }
    }
}

std::string VersionNumber::string() const
{
    std::vector<std::string> stringParts;
    stringParts.reserve(parts_.size());
    for (const auto &part : parts_) {
        stringParts.emplace_back(std::to_string(part));
    }
    return boost::algorithm::join(stringParts, kVersionSep);
}

} // namespace qtchooser
