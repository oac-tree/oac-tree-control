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
        <Equals lhs="live" rhs="zero"/>
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
    <ParallelSequence>
        <AchieveCondition varNames="live">
            <Equals lhs="live" rhs="one"/>
            <Wait timeout="1.0"/>
        </AchieveCondition>
        <Sequence>
            <Wait timeout="0.2"/>
            <Copy input="one" output="live"/>
        </Sequence>
    </ParallelSequence>
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
    // One child is ok
    const std::string body{R"(
      <AchieveCondition>
          <Wait timeout="1.0"/>
      </AchieveCondition>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_NO_THROW(proc->Setup());
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
  {
    // Missing mandatory attribute
    auto instr = GlobalInstructionRegistry().Create("AchieveCondition");
    auto wait_1 = GlobalInstructionRegistry().Create("Wait");
    auto wait_2 = GlobalInstructionRegistry().Create("Wait");
    ASSERT_TRUE(instr);
    ASSERT_TRUE(wait_1);
    ASSERT_TRUE(wait_2);
    ASSERT_TRUE(instr->InsertInstruction(std::move(wait_1), 0));
    ASSERT_TRUE(instr->InsertInstruction(std::move(wait_2), 1));
    ASSERT_TRUE(instr);
    Procedure proc;
    EXPECT_NO_THROW(instr->Setup(proc));
  }
}

TEST_F(AchieveConditionTest, OverrideSuccess)
{
  const std::string body{R"(
    <AchieveCondition>
        <Equals lhs="live" rhs="one"/>
        <Wait/>
    </AchieveCondition>
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

TEST_F(AchieveConditionTest, OverrideSuccessNoAction)
{
  const std::string body{R"(
    <AchieveCondition>
        <Equals lhs="live" rhs="one"/>
    </AchieveCondition>
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

TEST_F(AchieveConditionTest, AbortFailure)
{
  const std::string body{R"(
    <AchieveCondition dialogText="AbortPlease">
        <Equals lhs="live" rhs="one"/>
        <Wait/>
    </AchieveCondition>
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

TEST_F(AchieveConditionTest, AbortFailureNoAction)
{
  const std::string body{R"(
    <AchieveCondition dialogText="AbortPlease">
        <Equals lhs="live" rhs="one"/>
    </AchieveCondition>
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
