// The MIT License (MIT)
//
// Copyright (c) 2014 Mikhail Balakhno
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <cxxtest/TestSuite.h>

#include <ccb/config/JsonInputArchive.hpp>
#include <ccb/config/JsonOutputArchive.hpp>
#include <ccb/filesystem/FileSystem.hpp>
#include <ccb_tests/config/TestConfig.hpp>

namespace ccb { namespace config
{
    class JsonArchiveTests : public CxxTest::TestSuite
    {
    public:

        void TestBoolSerialization()
        {
            filesystem::FileSystem filesystem;
            auto tempFile = filesystem.GetTempPath() / filesystem.UniquePath();

            TestConfig<bool> config1(true);

            {
                JsonOutputArchive ar(tempFile.ToShortString());
                ar.Serialize(config1, L"config");
            }

            TestConfig<bool> config2;

            {
                JsonInputArchive ar(tempFile.ToShortString());
                ar.Serialize(config2, L"config");
            }

            TS_ASSERT_EQUALS(config1.GetValue(), config2.GetValue());

            filesystem.Remove(tempFile);
        }

        void TestIntSerialization()
        {
            filesystem::FileSystem filesystem;
            auto tempFile = filesystem.GetTempPath() / filesystem.UniquePath();

            TestConfig<int8_t> config8i1(13);
            TestConfig<int16_t> config16i1(14);
            TestConfig<int32_t> config32i1(15);
            TestConfig<int64_t> config64i1(16);
            TestConfig<uint8_t> config8u1(17);
            TestConfig<uint16_t> config16u1(18);
            TestConfig<uint32_t> config32u1(19);
            TestConfig<uint64_t> config64u1(20);

            {
                JsonOutputArchive ar(tempFile.ToShortString());

                ar.Serialize(config8i1, L"config8i");
                ar.Serialize(config16i1, L"config16i");
                ar.Serialize(config32i1, L"config32i");
                ar.Serialize(config64i1, L"config64i");
                ar.Serialize(config8u1, L"config8u");
                ar.Serialize(config16u1, L"config16u");
                ar.Serialize(config32u1, L"config32u");
                ar.Serialize(config64u1, L"config64u");
            }

            TestConfig<int8_t> config8i2;
            TestConfig<int16_t> config16i2;
            TestConfig<int32_t> config32i2;
            TestConfig<int64_t> config64i2;
            TestConfig<uint8_t> config8u2;
            TestConfig<uint16_t> config16u2;
            TestConfig<uint32_t> config32u2;
            TestConfig<uint64_t> config64u2;

            {
                JsonInputArchive ar(tempFile.ToShortString());

                ar.Serialize(config8i2, L"config8i");
                ar.Serialize(config16i2, L"config16i");
                ar.Serialize(config32i2, L"config32i");
                ar.Serialize(config64i2, L"config64i");
                ar.Serialize(config8u2, L"config8u");
                ar.Serialize(config16u2, L"config16u");
                ar.Serialize(config32u2, L"config32u");
                ar.Serialize(config64u2, L"config64u");
            }

            TS_ASSERT_EQUALS(config8i1.GetValue(), config8i2.GetValue());
            TS_ASSERT_EQUALS(config16i1.GetValue(), config16i2.GetValue());
            TS_ASSERT_EQUALS(config32i1.GetValue(), config32i2.GetValue());
            TS_ASSERT_EQUALS(config64i1.GetValue(), config64i2.GetValue());
            TS_ASSERT_EQUALS(config8u1.GetValue(), config8u2.GetValue());
            TS_ASSERT_EQUALS(config16u1.GetValue(), config16u2.GetValue());
            TS_ASSERT_EQUALS(config32u1.GetValue(), config32u2.GetValue());
            TS_ASSERT_EQUALS(config64u1.GetValue(), config64u2.GetValue());

            filesystem.Remove(tempFile);
        }

        void TestStringSerialization()
        {
            filesystem::FileSystem filesystem;
            auto tempFile = filesystem.GetTempPath() / filesystem.UniquePath();

            TestConfig<std::string> config1("hello");
            TestConfig<std::wstring> configW1(L"whello");

            {
                JsonOutputArchive ar(tempFile.ToShortString());
                ar.Serialize(config1, L"config");
                ar.Serialize(configW1, L"configw");
            }

            TestConfig<std::string> config2("hello");
            TestConfig<std::wstring> configW2(L"whello");

            {
                JsonInputArchive ar(tempFile.ToShortString());
                ar.Serialize(config2, L"config");
                ar.Serialize(configW2, L"configw");
            }

            TS_ASSERT_EQUALS(config1.GetValue(), config2.GetValue());
            TS_ASSERT_EQUALS(configW1.GetValue(), configW2.GetValue());

            filesystem.Remove(tempFile);
        }

        void TestSubclassSerialization()
        {
            filesystem::FileSystem filesystem;
            auto tempFile = filesystem.GetTempPath() / filesystem.UniquePath();

            TestConfig<TestConfig<int>> config1(TestConfig<int>(13));

            {
                JsonOutputArchive ar(tempFile.ToShortString());
                ar.Serialize(config1, L"config");
            }

            TestConfig<TestConfig<int>> config2;

            {
                JsonInputArchive ar(tempFile.ToShortString());
                ar.Serialize(config2, L"config");
            }

            TS_ASSERT_EQUALS(config1.GetValue().GetValue(), config2.GetValue().GetValue());

            filesystem.Remove(tempFile);
        }
    };
} }
