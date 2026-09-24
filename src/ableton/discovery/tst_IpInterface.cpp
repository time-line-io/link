/* Copyright 2025, Ableton AG, Berlin. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  If you would like to incorporate Link into a proprietary software application,
 *  please contact <link-devs@ableton.com>.
 */

#include <ableton/discovery/IpInterface.hpp>
#include <ableton/test/CatchWrapper.hpp>

#if defined(__linux__)
#include <atomic>
namespace
{
std::atomic<unsigned int> interfaceNameLookups{0};
}
extern "C" unsigned int __real_if_nametoindex(const char* name);
extern "C" unsigned int __wrap_if_nametoindex(const char* name)
{
  ++interfaceNameLookups;
  return __real_if_nametoindex(name);
}
#endif

namespace ableton
{
namespace discovery
{

TEST_CASE("IPv6 multicast endpoint retains its numeric scope")
{
  for (const auto scope : {uint64_t{0}, uint64_t{2}, uint64_t{0xffffffff}})
  {
#if defined(__linux__)
    const auto lookupsBefore = interfaceNameLookups.load();
#endif
    const auto endpoint = multicastEndpointV6(scope);
#if defined(__linux__)
    CHECK(interfaceNameLookups.load() == lookupsBefore);
#endif
    const auto address = endpoint.address().to_v6();
    CHECK(address.scope_id() == scope);
    CHECK(address.to_bytes() == makeAddress("ff12::8080").to_v6().to_bytes());
    CHECK(endpoint.port() == 20808);
  }
}

} // namespace discovery
} // namespace ableton
