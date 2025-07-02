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
* Copyright (c) : 2010-2025 ITER Organization,
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

class AchieveConditionWithTimeoutTest : public ::testing::Test
{
protected:
  AchieveConditionWithTimeoutTest() = default;
  ~AchieveConditionWithTimeoutTest() = default;
};

TEST_F(AchieveConditionWithTimeoutTest, Setup)
{
  {
    // No children
    const std::string body{R"(
      <AchieveConditionWithTimeout timeout="0.5"/>
      <Workspace/>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // One child
    const std::string body{R"(
      <AchieveConditionWithTimeout timeout="0.5">
          <Wait timeout="1.0"/>
      </AchieveConditionWithTimeout>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // Three children
    const std::string body{R"(
      <AchieveConditionWithTimeout timeout="0.5">
          <Wait timeout="1.0"/>
          <Wait timeout="2.0"/>
          <Wait timeout="3.0"/>
      </AchieveConditionWithTimeout>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // Missing mandatory attribute
    auto instr = GlobalInstructionRegistry().Create("AchieveConditionWithTimeout");
    auto wait_1 = GlobalInstructionRegistry().Create("Wait");
    auto wait_2 = GlobalInstructionRegistry().Create("Wait");
    ASSERT_TRUE(instr);
    ASSERT_TRUE(wait_1);
    ASSERT_TRUE(wait_2);
    ASSERT_TRUE(instr->InsertInstruction(std::move(wait_1), 0));
    ASSERT_TRUE(instr->InsertInstruction(std::move(wait_2), 1));
    Procedure proc;
    EXPECT_THROW(instr->Setup(proc), InstructionSetupException);
    EXPECT_TRUE(instr->AddAttribute("varNames", "does_not_matter"));
    EXPECT_THROW(instr->Setup(proc), InstructionSetupException);
    // Attribute cannot be parsed as double:
    EXPECT_TRUE(instr->AddAttribute("timeout", "five"));
    EXPECT_THROW(instr->Setup(proc), InstructionSetupException);
    EXPECT_TRUE(instr->SetAttribute("timeout", "5.0"));
    EXPECT_NO_THROW(instr->Setup(proc));
  }
}

TEST_F(AchieveConditionWithTimeoutTest, DirectSuccess)
{
  const std::string body{R"(
    <AchieveConditionWithTimeout timeout="1.0">
        <Equals leftVar="live" rightVar="zero"/>
        <Wait timeout="0.2"/>
    </AchieveConditionWithTimeout>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionWithTimeoutTest, SuccessImmediatelyAfterAction)
{
  const std::string body{R"(
    <AchieveConditionWithTimeout timeout="1.0">
        <Equals leftVar="live" rightVar="one"/>
        <Copy inputVar="one" outputVar="live"/>
    </AchieveConditionWithTimeout>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(AchieveConditionWithTimeoutTest, SuccessWithinTimeout)
{
  const std::string body{R"(
    <ParallelSequence failureThreshold="2">
        <AchieveConditionWithTimeout timeout="2.0">
            <Equals leftVar="live" rightVar="one"/>
            <Wait/>
        </AchieveConditionWithTimeout>
        <Inverter>
            <Sequence>
                <Wait timeout="0.3"/>
                <Copy inputVar="one" outputVar="live"/>
            </Sequence>
        </Inverter>
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

TEST_F(AchieveConditionWithTimeoutTest, FailAfterTimeout)
{
  const std::string body{R"(
    <AchieveConditionWithTimeout timeout="0.5">
        <Equals leftVar="live" rightVar="one"/>
        <Wait timeout="0.1"/>
    </AchieveConditionWithTimeout>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}
