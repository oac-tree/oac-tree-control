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

#include "achieve_condition_with_timeout_instruction.h"

#include "wrapped_user_interface.h"

#include <sup/oac-tree/constants.h>
#include <sup/oac-tree/instruction_registry.h>
#include <sup/oac-tree/procedure_context.h>

namespace sup {

namespace oac_tree {

const std::string AchieveConditionWithTimeoutInstruction::Type = "AchieveConditionWithTimeout";
static bool _wait_for_condition_initialised_flag =
  RegisterGlobalInstruction<AchieveConditionWithTimeoutInstruction>();

const std::string LOG_MESSAGE_PREFIX =
  "Forwarded log message from internal instruction of AchieveConditionWithTimeout: ";

AchieveConditionWithTimeoutInstruction::AchieveConditionWithTimeoutInstruction()
  : CompoundInstruction(Type)
  , m_internal_instruction_tree{}
  , m_instr_manager{}
{
  AddAttributeDefinition(Constants::VARIABLE_NAMES_ATTRIBUTE_NAME).SetMandatory();
  AddAttributeDefinition(Constants::TIMEOUT_SEC_ATTRIBUTE_NAME, sup::dto::Float64Type)
    .SetCategory(AttributeCategory::kBoth).SetMandatory();
}

AchieveConditionWithTimeoutInstruction::~AchieveConditionWithTimeoutInstruction() = default;

void AchieveConditionWithTimeoutInstruction::SetupImpl(const Procedure& proc)
{
  auto instr_tree = CreateWrappedInstructionTree();
  std::swap(m_internal_instruction_tree, instr_tree);
  m_internal_instruction_tree->Setup(proc);
}

ExecutionStatus AchieveConditionWithTimeoutInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
  m_internal_instruction_tree->ExecuteSingle(wrapped_ui, ws);
  return m_internal_instruction_tree->GetStatus();
}

void AchieveConditionWithTimeoutInstruction::HaltImpl()
{
  if (m_internal_instruction_tree)
  {
    m_internal_instruction_tree->Halt();
  }
}

void AchieveConditionWithTimeoutInstruction::ResetHook(UserInterface& ui)
{
  if (m_internal_instruction_tree)
  {
    auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
    m_internal_instruction_tree->Reset(wrapped_ui);
  }
}

std::vector<const Instruction*> AchieveConditionWithTimeoutInstruction::NextInstructionsImpl() const
{
  return FilterNextInstructions(*this, m_internal_instruction_tree.get());
}

std::unique_ptr<Instruction> AchieveConditionWithTimeoutInstruction::CreateWrappedInstructionTree()
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
  force_success->InsertInstruction(std::move(action_wrapper), 0);

  // Asynchronous wait for the timeout
  auto wait = GlobalInstructionRegistry().Create("Wait");
  wait->AddAttribute("timeout", GetAttributeString(Constants::TIMEOUT_SEC_ATTRIBUTE_NAME));

  // Use a clone of the condition here.
  auto cond_wrapper_2 = CloneInstructionTree(*children[0]);

  // Sequence combining action and recheck of condition
  auto sequence = GlobalInstructionRegistry().Create("Sequence");
  sequence->InsertInstruction(std::move(force_success), 0);
  sequence->InsertInstruction(std::move(wait), 1);
  sequence->InsertInstruction(std::move(cond_wrapper_2), 2);

  // Reactive fallback combining the condition and the sequence
  auto fallback = GlobalInstructionRegistry().Create("ReactiveFallback");
  fallback->InsertInstruction(std::move(cond_wrapper), 0);
  fallback->InsertInstruction(std::move(sequence), 1);

  return fallback;
}

} // namespace oac_tree

} // namespace sup
