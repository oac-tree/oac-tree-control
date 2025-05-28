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

#include "wait_for_condition_instruction.h"

#include "wrapped_user_interface.h"

#include <sup/oac-tree/constants.h>
#include <sup/oac-tree/instruction_registry.h>
#include <sup/oac-tree/instruction_utils.h>
#include <sup/oac-tree/procedure_context.h>

namespace sup {

namespace oac_tree {

const std::string WaitForConditionInstruction::Type = "WaitForCondition";
static bool _wait_for_condition_initialised_flag =
  RegisterGlobalInstruction<WaitForConditionInstruction>();

const std::string LOG_MESSAGE_PREFIX =
  "Forwarded log message from internal instruction of WaitForCondition: ";

WaitForConditionInstruction::WaitForConditionInstruction()
  : DecoratorInstruction(Type)
  , m_internal_instruction_tree{}
  , m_instr_manager{}
{
  AddAttributeDefinition(Constants::VARIABLE_NAMES_ATTRIBUTE_NAME).SetMandatory();
  AddAttributeDefinition(Constants::TIMEOUT_SEC_ATTRIBUTE_NAME, sup::dto::Float64Type)
    .SetCategory(AttributeCategory::kBoth).SetMandatory();
}

WaitForConditionInstruction::~WaitForConditionInstruction() = default;

void WaitForConditionInstruction::SetupImpl(const Procedure& proc)
{
  auto instr_tree = CreateWrappedInstructionTree();
  std::swap(m_internal_instruction_tree, instr_tree);
  m_internal_instruction_tree->Setup(proc);
}

ExecutionStatus WaitForConditionInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
  m_internal_instruction_tree->ExecuteSingle(wrapped_ui, ws);
  return m_internal_instruction_tree->GetStatus();
}

void WaitForConditionInstruction::HaltImpl()
{
  if (m_internal_instruction_tree)
  {
    m_internal_instruction_tree->Halt();
  }
}

void WaitForConditionInstruction::ResetHook(UserInterface& ui)
{
  if (m_internal_instruction_tree)
  {
    auto& wrapped_ui = m_instr_manager.GetWrappedUI(ui, LOG_MESSAGE_PREFIX);
    m_internal_instruction_tree->Reset(wrapped_ui);
  }
}

std::unique_ptr<Instruction> WaitForConditionInstruction::CreateWrappedInstructionTree()
{
  m_instr_manager.ClearWrappers();
  auto children = ChildInstructions();
  if (children.size() != 1)
  {
    std::string error_message = InstructionErrorProlog(*this) +
      "Trying to setup decorator without a child";
    throw InstructionSetupException(error_message);
  }

  // Wrapped condition
  auto cond_wrapper = m_instr_manager.CreateInstructionWrapper(*children[0]);

  // Wait with timeout branch
  auto wait = GlobalInstructionRegistry().Create("Wait");
  wait->AddAttribute("timeout", GetAttributeString(Constants::TIMEOUT_SEC_ATTRIBUTE_NAME));
  auto success_wait = GlobalInstructionRegistry().Create("ForceSuccess");
  success_wait->InsertInstruction(std::move(wait), 0);

  // Use a clone of the condition here.
  auto cond_wrapper_2 = CloneInstructionTree(*children[0]);

  // Sequence combining action and recheck of condition
  auto sequence = GlobalInstructionRegistry().Create("Sequence");
  sequence->InsertInstruction(std::move(success_wait), 0);
  sequence->InsertInstruction(std::move(cond_wrapper_2), 1);

  // Reactive fallback combining the condition and the sequence
  auto fallback = GlobalInstructionRegistry().Create("ReactiveFallback");
  fallback->InsertInstruction(std::move(cond_wrapper), 0);
  fallback->InsertInstruction(std::move(sequence), 1);

  return fallback;
}

} // namespace oac_tree

} // namespace sup
