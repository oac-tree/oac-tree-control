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

#include <sup/sequencer/instruction_registry.h>
#include <sup/sequencer/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::sequencer;

class AchieveConditionTest : public ::testing::Test
{
protected:
  AchieveConditionTest() = default;
  virtual ~AchieveConditionTest() = default;
};

TEST_F(AchieveConditionTest, DirectSuccess)
{
  const std::string body{R"(
    <AchieveCondition>
        <Equals leftVar="live" rightVar="zero"/>
        <Wait timeout="0.2"/>
    </AchieveCondition>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionTest, SuccessAfterAction)
{
  const std::string body{R"(
    <AchieveCondition>
        <Equals leftVar="live" rightVar="one"/>
        <Copy inputVar="one" outputVar="live"/>
    </AchieveCondition>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionTest, SuccessAfterCompoundAction)
{
  const std::string body{R"(
    <AchieveCondition>
        <Sequence>
            <Wait/>
            <Wait/>
            <Equals leftVar="live" rightVar="one"/>
        </Sequence>
        <Sequence>
            <Wait/>
            <Wait/>
            <Copy inputVar="one" outputVar="live"/>
        </Sequence>
    </AchieveCondition>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionTest, FailAfterAction)
{
  const std::string body{R"(
    <AchieveCondition>
        <Equals leftVar="live" rightVar="one"/>
        <Copy inputVar="zero" outputVar="live"/>
    </AchieveCondition>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}

TEST_F(AchieveConditionTest,FailAfterCompoundAction)
{
  const std::string body{R"(
    <AchieveCondition>
        <Sequence>
            <Wait/>
            <Wait/>
            <Equals leftVar="live" rightVar="one"/>
        </Sequence>
        <Sequence>
            <Wait/>
            <Wait/>
            <Copy inputVar="zero" outputVar="live"/>
        </Sequence>
    </AchieveCondition>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}

TEST_F(AchieveConditionTest, Setup)
{
  {
    // No children
    const std::string body{R"(
      <AchieveCondition/>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // One child
    const std::string body{R"(
      <AchieveCondition>
          <Wait timeout="1.0"/>
      </AchieveCondition>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // Three children
    const std::string body{R"(
      <AchieveCondition>
          <Wait timeout="1.0"/>
          <Wait timeout="2.0"/>
          <Wait timeout="3.0"/>
      </AchieveCondition>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
}
