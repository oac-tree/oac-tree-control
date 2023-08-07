/******************************************************************************
* $HeadURL: $
* $Id: $
*
* Project       : Supervision and Automation - Sequencer
*
* Description   : SUP sequencer control plugin
*
* Author        : Walter Van Herck (IO)
*
* Copyright (c) : 2010-2023 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

#include "test_user_interface.h"
#include "unit_test_helper.h"

#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::sequencer;

class WaitForConditionTest : public ::testing::Test
{
protected:
  WaitForConditionTest() = default;
  virtual ~WaitForConditionTest() = default;
};

TEST_F(WaitForConditionTest, success)
{
  const std::string body{R"(
    <ParallelSequence>
        <WaitForCondition varNames="live" timeout="1.0">
            <Equals lhs="live" rhs="one"/>
        </WaitForCondition>
        <Sequence>
            <Wait timeout="0.1"/>
            <Copy input="one" output="live"/>
        </Sequence>
    </ParallelSequence>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(WaitForConditionTest, failure)
{
  const std::string body{R"(
    <ParallelSequence>
        <WaitForCondition varNames="live" timeout="0.1">
            <Equals lhs="live" rhs="one"/>
        </WaitForCondition>
        <Sequence>
            <Wait timeout="1.0"/>
            <Copy input="one" output="live"/>
        </Sequence>
    </ParallelSequence>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}
