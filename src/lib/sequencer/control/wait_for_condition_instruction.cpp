/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - Sequencer
 *
 * Description   : Sequencer for operational procedures
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2022 ITER Organization,
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
#include <sup/sequencer/instruction_tree.h>
#include <sup/sequencer/instruction_utils.h>

#include <algorithm>

namespace
{
using namespace sup::sequencer;
std::vector<const Instruction*> Intersection(const std::vector<const Instruction*>& left_set,
                                             const std::vector<const Instruction*>& right_set);
}  // unnamed namespace

namespace sup {

namespace sequencer {

const std::string WaitForConditionInstruction::Type = "WaitForCondition";
static bool _dummy_initialised_flag = RegisterGlobalInstruction<WaitForConditionInstruction>();

const std::string TIMEOUT_ATTR_NAME = "timeout";
const std::string VARNAMES_ATTRIBUTE_NAME = "varNames";

const std::string LOG_MESSAGE_PREFIX = "Forwarded log message from internal instruction: ";

WaitForConditionInstruction::WaitForConditionInstruction()
  : DecoratorInstruction(Type)
  , m_internal_instruction_tree{}
  , m_instr_manager{}
{
  AddAttributeDefinition(VARNAMES_ATTRIBUTE_NAME, sup::dto::StringType).SetMandatory();
  AddAttributeDefinition(TIMEOUT_ATTR_NAME, sup::dto::Float64Type).SetMandatory();
}

WaitForConditionInstruction::~WaitForConditionInstruction() = default;

void WaitForConditionInstruction::SetupImpl(const Procedure& proc)
{
  auto children = ChildInstructions();
  if (children.size() != 1)
  {
    std::string error_message = InstructionErrorProlog(*this) +
      "Trying to setup this instruction without a child";
    throw InstructionSetupException(error_message);
  }
  auto instr_tree = CreateWrappedInstructionTree(*children[0]);
  std::swap(m_internal_instruction_tree, instr_tree);
  m_internal_instruction_tree->Setup(proc);
}

ExecutionStatus WaitForConditionInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  m_instr_manager.SetUserInterface(ui);
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

void WaitForConditionInstruction::ResetHook()
{
  if (m_internal_instruction_tree)
  {
    m_internal_instruction_tree->Reset();
  }
  m_internal_instruction_tree.reset();
  m_instr_manager.ClearWrappers();
}

std::vector<const Instruction*> WaitForConditionInstruction::NextInstructionsImpl() const
{
  if (m_internal_instruction_tree)
  {
    auto children = ChildInstructions();
    if (children.size() > 0)
    {
      auto next_instr = FlattenBFS(CreateNextInstructionTree(m_internal_instruction_tree.get()));
      return Intersection(next_instr, children);
    }
  }
  return {};
}

std::unique_ptr<Instruction>  WaitForConditionInstruction::CreateWrappedInstructionTree(
  Instruction& instr)
{
    // Inverted wait with timeout branch
  auto wait = GlobalInstructionRegistry().Create("Wait");
  wait->AddAttribute("timeout", GetAttributeString(TIMEOUT_ATTR_NAME));
  auto inv_wait = GlobalInstructionRegistry().Create("Inverter");
  inv_wait->InsertInstruction(std::move(wait), 0);

  // Branch that listens and only exits with success when condition is satisfied
  auto wrapper = m_instr_manager.CreateInstructionWrapper(instr);
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

namespace
{
using namespace sup::sequencer;
std::vector<const Instruction*> Intersection(const std::vector<const Instruction*>& left_set,
                                             const std::vector<const Instruction*>& right_set)
{
  std::vector<const Instruction*> result;
  for (auto left_instr : left_set)
  {
    if (std::find(right_set.begin(), right_set.end(), left_instr) != right_set.end())
    {
      result.push_back(left_instr);
    }
  }
  return result;
}

}  // unnamed namespace
