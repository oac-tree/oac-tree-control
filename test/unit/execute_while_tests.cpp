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
* Copyright (c) : 2010-2024 ITER Organization,
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

class ExecuteWhileTest : public ::testing::Test
{
protected:
  ExecuteWhileTest() = default;
  virtual ~ExecuteWhileTest() = default;
};

TEST_F(ExecuteWhileTest, Success)
{
  const std::string body{R"(
    <ExecuteWhile varNames="live">
        <Wait timeout="0.2"/>
        <Equals leftVar="live" rightVar="zero"/>
    </ExecuteWhile>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
    </Workspace>
)"};

  test::NullUserInterface ui;
  auto proc = ParseProcedureString(test::CreateProcedureString(body));
  EXPECT_TRUE(test::TryAndExecute(proc, ui));
}

TEST_F(ExecuteWhileTest, Setup)
{
  {
    // No children
    const std::string body{R"(
      <ExecuteWhile varNames="live"/>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // One child
    const std::string body{R"(
      <ExecuteWhile varNames="live">
          <Wait timeout="1.0"/>
      </ExecuteWhile>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // Three children
    const std::string body{R"(
      <ExecuteWhile varNames="live">
          <Wait timeout="1.0"/>
          <Wait timeout="2.0"/>
          <Wait timeout="3.0"/>
      </ExecuteWhile>
      <Workspace>
          <Local name="live" type='{"type":"uint64"}' value='0' />
      </Workspace>)"};

    auto proc = ParseProcedureString(test::CreateProcedureString(body));
    EXPECT_THROW(proc->Setup(), InstructionSetupException);
  }
  {
    // Missing mandatory attribute
    auto instr = GlobalInstructionRegistry().Create("ExecuteWhile");
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
    EXPECT_NO_THROW(instr->Setup(proc));
  }
}

TEST_F(ExecuteWhileTest, Failure)
{
  const std::string body{R"(
    <ParallelSequence successThreshold="1">
        <ExecuteWhile varNames="live">
            <Wait timeout="1.0"/>
            <Equals leftVar="live" rightVar="zero"/>
        </ExecuteWhile>
        <Inverter>
            <Sequence>
                <Wait timeout="0.1"/>
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
  EXPECT_TRUE(test::TryAndExecute(proc, ui, ExecutionStatus::FAILURE));
}
