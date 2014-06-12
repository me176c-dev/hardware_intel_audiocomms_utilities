/**
 * @section License
 *
 * Copyright 2013-2014 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "result/Result.hpp"

#include <gtest/gtest.h>

struct TestResult
{
    enum Code
    {
        A = 666,
        B,
        C,
        D
    };

    static const Code success;
    static const Code defaultError;

    static std::string codeToString(const Code &code)
    {
        switch (code) {
        case A:
            return "A";
        case B:
            return "B";
        case C:
            return "C";
        case D:
            return "D";
        }
        /** @fixme: ASSERT? */
        return "";
    }
};

const TestResult::Code TestResult::success = A;
const TestResult::Code TestResult::defaultError = B;

typedef audio_comms::cme::result::Result<TestResult> Result;

TEST(Result, defaultValueConstructor)
{
    Result aResult;

    /* Note, this also tests operator== */
    EXPECT_EQ(Result(TestResult::defaultError), aResult);
    EXPECT_EQ("", aResult.getMessage());
}

TEST(Result, someValueConstructor)
{
    Result aResult(TestResult::A);

    EXPECT_EQ(Result(TestResult::A), aResult);
    EXPECT_EQ("", aResult.getMessage());
}

TEST(Result, convertToCode)
{
    {
        Result aResult(TestResult::success);
        EXPECT_TRUE(aResult == TestResult::success);
    }

    {
        Result aResult(TestResult::C);
        EXPECT_FALSE(aResult.isSuccess());
    }
}

TEST(Result, isFailure)
{
    {
        Result aResult(TestResult::success);
        EXPECT_FALSE(aResult.isFailure());
        aResult << "that should not change success state";
        EXPECT_FALSE(aResult.isFailure());
    }

    {
        Result aResult(TestResult::D);
        EXPECT_TRUE(aResult.isFailure());
        aResult << "that should not change failure state";
        EXPECT_TRUE(aResult.isFailure());
    }
}

TEST(Result, isSuccess)
{
    {
        Result aResult(TestResult::D);
        EXPECT_FALSE(aResult.isSuccess());
        aResult << "that should not change failure state";
        EXPECT_FALSE(aResult.isSuccess());
    }

    {
        Result aResult(TestResult::success);
        EXPECT_TRUE(aResult.isSuccess());
        aResult << "that should not change success state";
        EXPECT_TRUE(aResult.isSuccess());
    }
}

TEST(Result, equalityOperator)
{
    Result aResult(TestResult::success);
    aResult << " Now error message differs.";
    EXPECT_EQ(Result(TestResult::success),  aResult);
}

TEST(Result, getErrorCode)
{
    Result aResult(TestResult::success);
    EXPECT_EQ(TestResult::success, aResult.getErrorCode());
}

TEST(Result, formatSuccess)
{
    Result aResult(TestResult::success);
    EXPECT_EQ("Success", aResult.format());
}

TEST(Result, formatError)
{
    Result aResult(TestResult::C);
    EXPECT_EQ("Code 668: C", aResult.format());
}

TEST(Result, useWithStream)
{
    Result aResult(TestResult::B);
    aResult << "gnii";
    EXPECT_EQ("gnii",  aResult.getMessage());
    EXPECT_EQ("Code 667: B (gnii)",  aResult.format());
}

TEST(Result, successSingleton)
{
    EXPECT_EQ(Result::success(), Result(TestResult::success));
}

TEST(Result, constructFromfailureRes)
{
    Result bResult(TestResult::B);
    bResult << "gnii";
    Result trans(bResult, TestResult::D);
    trans << "gnaa";
    EXPECT_EQ("Code 669: D (Code 667: B (gnii)gnaa)",  trans.format());
}

TEST(Result, constructFromSuccessRes)
{
    Result trans(Result::success(), TestResult::D, TestResult::C);
    EXPECT_EQ(TestResult::C, trans);
}

TEST(Result, resultStream)
{
    Result bResult(TestResult::B);
    EXPECT_EQ("Code 667: B (gnii: Code 669: D (gnuu))",
              (Result(TestResult::B) << "gnii"
                                     << (Result(TestResult::D) << "gnuu")
              ).format());
}
