#include <cxxtest/TestSuite.h>

#include <ccb/filesystem/Path.hpp>

namespace ccb { namespace filesystem
{
    class PathTests : public CxxTest::TestSuite
    {
    public:

        void TestCanCreateEmptyPath()
        {
            Path path;

            TS_ASSERT(path.IsEmpty());
        }
    };
} }
