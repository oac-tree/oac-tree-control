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

#include "achieve_condition_instruction.h"

#include <sup/oac-tree/instruction_registry.h>
#include <sup/oac-tree/instruction_utils.h>
#include <sup/oac-tree/procedure_context.h>
#include <sup/oac-tree/user_interface.h>

namespace sup {

namespace oac_tree {

const std::string AchieveConditionInstruction::Type = "AchieveCondition";
static bool _achieve_condition_initialised_flag =
  RegisterGlobalInstruction<AchieveConditionInstruction>();

const std::string LOG_MESSAGE_PREFIX =
  "Forwarded log message from internal instruction of AchieveCondition: ";

AchieveConditionInstruction::AchieveConditionInstruction()
  : CompoundInstruction(Type)
  , m_internal_instruction_tree{}
  , m_instr_manager{}
{}

AchieveConditionInstruction::~AchieveConditionInstruction() = default;

void AchieveConditionInstruction::SetupImpl(const Procedure& proc)
{
  auto instr_tree = CreateWrappedInstructionTree();
  std::swap(m_internal_instruction_tree, instr_tree);
  m_internal_instruction_tree->Setup(proc);
}

ExecutionStatus AchieveConditionInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
  m_internal_instruction_tree->ExecuteSingle(wrapped_ui, ws);
  return m_internal_instruction_tree->GetStatus();
}

void AchieveConditionInstruction::ResetHook(UserInterface& ui)
{
  if (m_internal_instruction_tree)
  {
    auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
    m_internal_instruction_tree->Reset(wrapped_ui);
  }
}

void AchieveConditionInstruction::HaltImpl()
{
  if (m_internal_instruction_tree)
  {
    m_internal_instruction_tree->Halt();
  }
}

std::unique_ptr<Instruction> AchieveConditionInstruction::CreateWrappedInstructionTree()
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
  auto cond_wrapper = m_instr_manager.CreateInstructionWrapper(*children[0]);

  // Wrapped action
  auto action_wrapper = m_instr_manager.CreateInstructionWrapper(*children[1]);

  // Ignore failure status of action
  auto force_success = GlobalInstructionRegistry().Create("ForceSuccess");
  (void)force_success->InsertInstruction(std::move(action_wrapper), 0);

  // Use a clone of the condition here.
  auto cond_wrapper_2 = CloneInstructionTree(*children[0]);

  // Sequence combining action and recheck of condition
  auto sequence = GlobalInstructionRegistry().Create("Sequence");
  (void)sequence->InsertInstruction(std::move(force_success), 0);
  (void)sequence->InsertInstruction(std::move(cond_wrapper_2), 1);

  // Reactive fallback combining the condition and the sequence
  auto fallback = GlobalInstructionRegistry().Create("ReactiveFallback");
  (void)fallback->InsertInstruction(std::move(cond_wrapper), 0);
  (void)fallback->InsertInstruction(std::move(sequence), 1);

  return fallback;
}

} // namespace oac_tree

} // namespace sup
