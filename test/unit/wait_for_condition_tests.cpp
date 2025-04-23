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

class WaitForConditionTest : public ::testing::Test
{
protected:
  WaitForConditionTest() = default;
  virtual ~WaitForConditionTest() = default;
};

TEST_F(WaitForConditionTest, Success)
{
  const std::string body{R"(
    <ParallelSequence>
        <WaitForCondition varNames="live" timeout="1.0">
            <Equals leftVar="live" rightVar="one"/>
        </WaitForCondition>
        <Sequence>
            <Wait timeout="0.1"/>
            <Copy inputVar="one" outputVar="live"/>
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

TEST_F(WaitForConditionTest, Setup)
{
  {
    // No child
    const std::string body{R"(
      <WaitForCondition varNames="live" timeout="0.1"/>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // Missing attributes
    auto instr = GlobalInstructionRegistry().Create("WaitForCondition");
    auto wait = GlobalInstructionRegistry().Create("Wait");
    ASSERT_TRUE(instr);
    ASSERT_TRUE(wait);
    ASSERT_TRUE(instr->InsertInstruction(std::move(wait), 0));
    Procedure proc;
    EXPECT_THROW(instr->Setup(proc), InstructionSetupException);
    EXPECT_TRUE(instr->AddAttribute("timeout", "1.0"));
    EXPECT_THROW(instr->Setup(proc), InstructionSetupException);
    EXPECT_TRUE(instr->AddAttribute("varNames", "does_not_matter"));
    EXPECT_NO_THROW(instr->Setup(proc));
  }
}

TEST_F(WaitForConditionTest, Failure)
{
  const std::string body{R"(
    <ParallelSequence>
        <WaitForCondition varNames="live" timeout="0.1">
            <Equals leftVar="live" rightVar="one"/>
        </WaitForCondition>
        <Sequence>
            <Wait timeout="1.0"/>
            <Copy inputVar="one" outputVar="live"/>
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

TEST_F(WaitForConditionTest, VariableTimeout)
{
  const std::string body{R"(
    <ParallelSequence>
        <WaitForCondition varNames="live" timeout="@timeout">
            <Equals leftVar="live" rightVar="one"/>
        </WaitForCondition>
        <Sequence>
            <Wait timeout="0.1"/>
            <Copy inputVar="one" outputVar="live"/>
        </Sequence>
    </ParallelSequence>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
        <Local name="timeout" type='{"type":"float64"}' value='1.0' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(WaitForConditionTest, VariableTimeoutWrongType)
{
  const std::string body{R"(
    <ParallelSequence>
        <WaitForCondition varNames="live" timeout="@timeout">
            <Equals leftVar="live" rightVar="one"/>
        </WaitForCondition>
        <Sequence>
            <Wait timeout="0.1"/>
            <Copy inputVar="one" outputVar="live"/>
        </Sequence>
    </ParallelSequence>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
        <Local name="timeout" type='{"type":"string"}' value='"3.0"' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}

TEST_F(WaitForConditionTest, VariableTimeoutNotPresent)
{
  const std::string body{R"(
    <ParallelSequence>
        <WaitForCondition varNames="live" timeout="@timeout">
            <Equals leftVar="live" rightVar="one"/>
        </WaitForCondition>
        <Sequence>
            <Wait timeout="0.1"/>
            <Copy inputVar="one" outputVar="live"/>
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
