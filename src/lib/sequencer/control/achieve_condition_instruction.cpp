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

#include "achieve_condition_instruction.h"

#include <sup/sequencer/instruction_registry.h>
#include <sup/sequencer/instruction_utils.h>
#include <sup/sequencer/user_interface.h>

namespace sup {

namespace sequencer {

const std::string AchieveConditionInstruction::Type = "AchieveCondition";
static bool _achieve_condition_initialised_flag =
  RegisterGlobalInstruction<AchieveConditionInstruction>();

AchieveConditionInstruction::AchieveConditionInstruction()
  : CompoundInstruction(Type)
  , m_action_done{false}
{}

AchieveConditionInstruction::~AchieveConditionInstruction() = default;

void AchieveConditionInstruction::SetupImpl(const Procedure& proc)
{
  (void)proc;
  auto children = ChildInstructions();
  if (children.size() != 2)
  {
    std::string error_message = InstructionErrorProlog(*this) +
      "This compound instruction requires exactly two child instructions";
    throw InstructionSetupException(error_message);
  }
  SetupChildren(proc);
}

ExecutionStatus AchieveConditionInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  auto children = ChildInstructions();
  auto condition = children[0];
  auto condition_status = condition->GetStatus();
  if (NeedsExecute(condition_status))
  {
    children[0]->ExecuteSingle(ui, ws);
    return CalculateCompoundStatus();
  }
  if (!m_action_done)
  {
    HandleAction(ui, ws);
  }
  return CalculateCompoundStatus();
}

void AchieveConditionInstruction::ResetHook()
{
  ResetChildren();
  m_action_done = false;
}

std::vector<const Instruction*> AchieveConditionInstruction::NextInstructionsImpl() const
{
  std::vector<const Instruction*> result;
  auto children = ChildInstructions();
  auto condition = children[0];
  auto condition_status = condition->GetStatus();
  if (ReadyForExecute(condition_status))
  {
    result.push_back(condition);
  }
  if (IsFinishedStatus(condition_status) && !m_action_done)
  {
    auto action = children[1];
    auto action_status = action->GetStatus();
    if (ReadyForExecute(action_status))
    {
      result.push_back(action);
    }
  }
  return result;
}

void AchieveConditionInstruction::HandleAction(UserInterface& ui, Workspace& ws)
{
  auto children = ChildInstructions();
  auto action = children[1];
  auto action_status = action->GetStatus();
  if (NeedsExecute(action_status))
  {
    action->ExecuteSingle(ui, ws);
  }
  action_status = action->GetStatus();
  if (IsFinishedStatus(action_status))
  {
    ResetChildren();
    m_action_done = true;
  }
}

ExecutionStatus AchieveConditionInstruction::CalculateCompoundStatus() const
{
  auto children = ChildInstructions();
  auto condition = children[0];
  auto condition_status = condition->GetStatus();
  // When condition did not fail (yet) or the action was already performed, return its status.
  if (condition_status != ExecutionStatus::FAILURE || m_action_done)
  {
    return condition_status;
  }
  // Otherwise return that status of the action.
  auto action = children[1];
  auto action_status = action->GetStatus();
  return action->GetStatus();
  if (action_status == ExecutionStatus::RUNNING)
  {
    return ExecutionStatus::RUNNING;
  }
  return ExecutionStatus::NOT_FINISHED;
}

} // namespace sequencer

} // namespace sup
