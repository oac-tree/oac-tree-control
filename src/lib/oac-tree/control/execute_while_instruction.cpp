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

#include "execute_while_instruction.h"

#include "wrapped_user_interface.h"

#include <sup/oac-tree/constants.h>
#include <sup/oac-tree/instruction_registry.h>
#include <sup/oac-tree/instruction_utils.h>

namespace sup {

namespace oac_tree {

const std::string ExecuteWhileInstruction::Type = "ExecuteWhile";
static bool _execute_while_initialised_flag = RegisterGlobalInstruction<ExecuteWhileInstruction>();

const std::string LOG_MESSAGE_PREFIX =
  "Forwarded log message from internal instruction of ExecuteWhile: ";


ExecuteWhileInstruction::ExecuteWhileInstruction()
  : CompoundInstruction(Type)
  , m_internal_instruction_tree{}
  , m_instr_manager{}
{
  AddAttributeDefinition(Constants::VARIABLE_NAMES_ATTRIBUTE_NAME).SetMandatory();
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

void ExecuteWhileInstruction::ResetHook(UserInterface& ui)
{
  if (m_internal_instruction_tree)
  {
    auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
    m_internal_instruction_tree->Reset(wrapped_ui);
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

  // Wrapped condition
  auto condition_wrapper = m_instr_manager.CreateInstructionWrapper(*children[1]);

  // Wrapped action tree
  auto tree_wrapper = m_instr_manager.CreateInstructionWrapper(*children[0]);

  // Make action asynchronous
  auto async_action = GlobalInstructionRegistry().Create("Async");
  async_action->InsertInstruction(std::move(tree_wrapper), 0);

  // Reactive sequence combining the condition and the action
  auto reactive_sequence = GlobalInstructionRegistry().Create("ReactiveSequence");
  reactive_sequence->InsertInstruction(std::move(condition_wrapper), 0);
  reactive_sequence->InsertInstruction(std::move(async_action), 1);

  return reactive_sequence;
}

} // namespace oac_tree

} // namespace sup
