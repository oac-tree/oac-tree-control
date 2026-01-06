/******************************************************************************
* $HeadURL: $
* $Id: $
*
* Project       : Supervision and Automation - oac-tree
*
* Description   : SUP oac-tree control plugin
*
* Author        : Walter Van Herck (IO)
*
* Copyright (c) : 2010-2026 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
* SPDX-License-Identifier: MIT
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file LICENSE located in the top level directory
* of the distribution package.
******************************************************************************/

#include "test_user_interface.h"
#include "unit_test_helper.h"

#include <sup/oac-tree/instruction_registry.h>
#include <sup/oac-tree/sequence_parser.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree;

class AchieveConditionWithOverrideTest : public ::testing::Test
{
protected:
  AchieveConditionWithOverrideTest() = default;
  virtual ~AchieveConditionWithOverrideTest() = default;
};

TEST_F(AchieveConditionWithOverrideTest, DirectSuccess)
{
  const std::string body{R"(
    <AchieveConditionWithOverride>
        <Equals leftVar="live" rightVar="zero"/>
        <Wait timeout="0.2"/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionWithOverrideTest, SuccessAfterAction)
{
  const std::string body{R"(
    <AchieveConditionWithOverride>
        <Equals leftVar="live" rightVar="one"/>
        <Copy inputVar="one" outputVar="live"/>
    </AchieveConditionWithOverride>
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

TEST_F(AchieveConditionWithOverrideTest, SuccessAfterAsyncAction)
{
  const std::string body{R"(
    <AchieveConditionWithOverride>
        <Equals leftVar="live" rightVar="one"/>
        <Sequence>
            <Wait timeout="0.2"/>
            <Copy inputVar="one" outputVar="live"/>
        </Sequence>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionWithOverrideTest, SuccessAfterCompoundAction)
{
  const std::string body{R"(
    <AchieveConditionWithOverride>
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
    </AchieveConditionWithOverride>
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

TEST_F(AchieveConditionWithOverrideTest, Setup)
{
  {
    // No children
    const std::string body{R"(
      <AchieveConditionWithOverride/>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // One child is ok
    const std::string body{R"(
      <AchieveConditionWithOverride>
          <Wait timeout="1.0"/>
      </AchieveConditionWithOverride>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_NO_THROW(proc->Setup());
  }
  {
    // Three children
    const std::string body{R"(
      <AchieveConditionWithOverride>
          <Wait timeout="1.0"/>
          <Wait timeout="2.0"/>
          <Wait timeout="3.0"/>
      </AchieveConditionWithOverride>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // Missing mandatory attribute
    auto instr = GlobalInstructionRegistry().Create("AchieveConditionWithOverride");
    auto wait_1 = GlobalInstructionRegistry().Create("Wait");
    auto wait_2 = GlobalInstructionRegistry().Create("Wait");
    ASSERT_TRUE(instr);
    ASSERT_TRUE(wait_1);
    ASSERT_TRUE(wait_2);
    ASSERT_TRUE(instr->InsertInstruction(std::move(wait_1), 0));
    ASSERT_TRUE(instr->InsertInstruction(std::move(wait_2), 1));
    Procedure proc;
    EXPECT_NO_THROW(instr->Setup(proc));
  }
}

TEST_F(AchieveConditionWithOverrideTest, OverrideSuccess)
{
  const std::string body{R"(
    <AchieveConditionWithOverride>
        <Equals leftVar="live" rightVar="one"/>
        <Wait/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 0, 0, 1});
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
  EXPECT_EQ(ui.m_main_text, "Condition is still not satisfied. Please select action.");
}

TEST_F(AchieveConditionWithOverrideTest, OverrideSuccessNoAction)
{
  const std::string body{R"(
    <AchieveConditionWithOverride>
        <Equals leftVar="live" rightVar="one"/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 0, 0, 1});
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
  EXPECT_EQ(ui.m_main_text, "Condition is still not satisfied. Please select action.");
}

TEST_F(AchieveConditionWithOverrideTest, AbortFailure)
{
  const std::string body{R"(
    <AchieveConditionWithOverride dialogText="AbortPlease">
        <Equals leftVar="live" rightVar="one"/>
        <Wait/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 0, 0, 2});
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
  EXPECT_EQ(ui.m_main_text, "AbortPlease");
}

TEST_F(AchieveConditionWithOverrideTest, AbortFailureNoAction)
{
  const std::string body{R"(
    <AchieveConditionWithOverride dialogText="AbortPlease">
        <Equals leftVar="live" rightVar="one"/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 0, 0, 2});
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
  EXPECT_EQ(ui.m_main_text, "AbortPlease");
}

TEST_F(AchieveConditionWithOverrideTest, KeepRetryingNoAction)
{
  const std::string body{R"(
    <ParallelSequence>
        <AchieveConditionWithOverride>
            <Equals leftVar="live" rightVar="one"/>
        </AchieveConditionWithOverride>
        <Sequence>
            <Wait timeout="0.5"/>
            <Copy inputVar="one" outputVar="live"/>
        </Sequence>
    </ParallelSequence>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 0 });
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionWithOverrideTest, InvalidUserReplyFuture)
{
  const std::string body{R"(
    <AchieveConditionWithOverride dialogText="Invalid Future">
        <Equals leftVar="live" rightVar="one"/>
        <Wait/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.ReturnValidFuture(false);
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
  EXPECT_TRUE(ui.m_main_text.empty());
}

TEST_F(AchieveConditionWithOverrideTest, InvalidUserChoice)
{
  const std::string body{R"(
    <AchieveConditionWithOverride dialogText="WrongChoice">
        <Equals leftVar="live" rightVar="one"/>
        <Wait/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ -3 });
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
  EXPECT_EQ(ui.m_main_text, "WrongChoice");
}

TEST_F(AchieveConditionWithOverrideTest, VariableDialogText)
{
  const std::string body{R"(
    <AchieveConditionWithOverride dialogText="@text">
        <Equals leftVar="live" rightVar="one"/>
        <Wait/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="text" type='{"type":"string"}' value='"Please decide what to do"' />
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 0, 0, 1});
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
  EXPECT_EQ(ui.m_main_text, "Please decide what to do");
}

TEST_F(AchieveConditionWithOverrideTest, VariableDialogTextWrongType)
{
  const std::string body{R"(
    <AchieveConditionWithOverride dialogText="@text">
        <Equals leftVar="live" rightVar="one"/>
        <Wait/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="text" type='{"type":"float32"}' value='4.3' />
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 1});
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}

TEST_F(AchieveConditionWithOverrideTest, VariableDialogTextNotPresent)
{
  const std::string body{R"(
    <AchieveConditionWithOverride dialogText="@text">
        <Equals leftVar="live" rightVar="one"/>
        <Wait/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::TestUserInputInterface ui;
  ui.SetUserChoices({ 1});
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}

