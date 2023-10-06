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

#include "execute_while_instruction.h"

#include "wrapped_user_interface.h"

#include <sup/sequencer/instruction_registry.h>
#include <sup/sequencer/instruction_utils.h>

namespace sup {

namespace sequencer {

const std::string ExecuteWhileInstruction::Type = "ExecuteWhile";
static bool _execute_while_initialised_flag = RegisterGlobalInstruction<ExecuteWhileInstruction>();

const std::string VARNAMES_ATTRIBUTE_NAME = "varNames";

const std::string LOG_MESSAGE_PREFIX =
  "Forwarded log message from internal instruction of ExecuteWhile: ";


ExecuteWhileInstruction::ExecuteWhileInstruction()
  : CompoundInstruction(Type)
  , m_internal_instruction_tree{}
  , m_instr_manager{}
{
  AddAttributeDefinition(VARNAMES_ATTRIBUTE_NAME).SetMandatory();
}

ExecuteWhileInstruction::~ExecuteWhileInstruction() = default;

void ExecuteWhileInstruction::SetupImpl(const Procedure& proc)
{
  auto instr_tree = CreateWrappedInstructionTree();
  std::swap(m_internal_instruction_tree, instr_tree);
  m_internal_instruction_tree->Setup(proc);
}

ExecutionStatus ExecuteWhileInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  m_instr_manager.SetContext(ui);
  auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
  m_internal_instruction_tree->ExecuteSingle(wrapped_ui, ws);
  return m_internal_instruction_tree->GetStatus();
}

void ExecuteWhileInstruction::HaltImpl()
{
  if (m_internal_instruction_tree)
  {
    m_internal_instruction_tree->Halt();
  }
}

void ExecuteWhileInstruction::ResetHook()
{
  if (m_internal_instruction_tree)
  {
    m_internal_instruction_tree->Reset();
  }
}

std::vector<const Instruction*> ExecuteWhileInstruction::NextInstructionsImpl() const
{
  return FilterNextInstructions(*this, m_internal_instruction_tree.get());
}

std::unique_ptr<Instruction> ExecuteWhileInstruction::CreateWrappedInstructionTree()
{
  m_instr_manager.ClearWrappers();
  auto children = ChildInstructions();
  if (children.size() != 2)
  {
    std::string error_message = InstructionErrorProlog(*this) +
      "This compound instruction requires exactly two child instructions";
    throw InstructionSetupException(error_message);
  }

  // Wrapped execution tree
  auto tree_wrapper = m_instr_manager.CreateInstructionWrapper(*children[0]);

  // Condition monitoring tree
  auto condition_wrapper = m_instr_manager.CreateInstructionWrapper(*children[1]);
  auto listen = GlobalInstructionRegistry().Create("Listen");
  listen->AddAttribute("varNames", GetAttributeString(VARNAMES_ATTRIBUTE_NAME));
  listen->InsertInstruction(std::move(condition_wrapper), 0);

  // Parallel sequence that aggregates both
  auto parallel = GlobalInstructionRegistry().Create("ParallelSequence");
  parallel->AddAttribute("successThreshold", "1");
  parallel->AddAttribute("failureThreshold", "1");
  parallel->InsertInstruction(std::move(tree_wrapper), 0);
  parallel->InsertInstruction(std::move(listen), 1);

  return parallel;
}

} // namespace sequencer

} // namespace sup
