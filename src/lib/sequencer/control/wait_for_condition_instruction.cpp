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
* Copyright (c) : 2010-2025 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

#include "wait_for_condition_instruction.h"

#include "wrapped_user_interface.h"

#include <sup/sequencer/instruction_registry.h>
#include <sup/sequencer/instruction_utils.h>

namespace sup {

namespace sequencer {

const std::string WaitForConditionInstruction::Type = "WaitForCondition";
static bool _wait_for_condition_initialised_flag =
  RegisterGlobalInstruction<WaitForConditionInstruction>();

const std::string TIMEOUT_ATTR_NAME = "timeout";
const std::string VARNAMES_ATTRIBUTE_NAME = "varNames";

const std::string LOG_MESSAGE_PREFIX =
  "Forwarded log message from internal instruction of WaitForCondition: ";

WaitForConditionInstruction::WaitForConditionInstruction()
  : DecoratorInstruction(Type)
  , m_internal_instruction_tree{}
  , m_instr_manager{}
{
  AddAttributeDefinition(VARNAMES_ATTRIBUTE_NAME).SetMandatory();
  AddAttributeDefinition(TIMEOUT_ATTR_NAME, sup::dto::Float64Type)
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

std::vector<const Instruction*> WaitForConditionInstruction::NextInstructionsImpl() const
{
  return FilterNextInstructions(*this, m_internal_instruction_tree.get());
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

  // Inverted wait with timeout branch
  auto wait = GlobalInstructionRegistry().Create("Wait");
  wait->AddAttribute("timeout", GetAttributeString(TIMEOUT_ATTR_NAME));
  auto success_wait = GlobalInstructionRegistry().Create("ForceSuccess");
  success_wait->InsertInstruction(std::move(wait), 0);
  auto inv_wait = GlobalInstructionRegistry().Create("Inverter");
  inv_wait->InsertInstruction(std::move(success_wait), 0);

  // Branch that listens and only exits with success when condition is satisfied
  auto wrapper = m_instr_manager.CreateInstructionWrapper(*children[0]);
  auto inv_cond = GlobalInstructionRegistry().Create("Inverter");
  inv_cond->InsertInstruction(std::move(wrapper), 0);
  auto listen = GlobalInstructionRegistry().Create("Listen");
  listen->AddAttribute("varNames", GetAttributeString(VARNAMES_ATTRIBUTE_NAME));
  listen->InsertInstruction(std::move(inv_cond), 0);
  auto inv_listen = GlobalInstructionRegistry().Create("Inverter");
  inv_listen->InsertInstruction(std::move(listen), 0);

  // Parallel sequence that aggregates both
  auto parallel = GlobalInstructionRegistry().Create("ParallelSequence");
  parallel->AddAttribute("successThreshold", "1");
  parallel->AddAttribute("failureThreshold", "1");
  parallel->InsertInstruction(std::move(inv_wait), 0);
  parallel->InsertInstruction(std::move(inv_listen), 1);

  return parallel;
}

} // namespace sequencer

} // namespace sup
