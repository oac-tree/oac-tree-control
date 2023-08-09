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

#include <sup/sequencer/constants.h>
#include <sup/sequencer/instruction_registry.h>
#include <sup/sequencer/instruction_utils.h>
#include <sup/sequencer/user_interface.h>

namespace sup {

namespace sequencer {

const std::string AchieveConditionInstruction::Type = "AchieveCondition";
static bool _achieve_condition_initialised_flag =
  RegisterGlobalInstruction<AchieveConditionInstruction>();

const std::string MAIN_DIALOG_TEXT_ATTRIBUTE = "dialogText";
const std::string MAIN_DIALOG_TEXT_DEFAULT =
  "Condition is still not satisfied. Please select action.";

const std::string RETRY_TEXT_DEFAULT = "Retry";
const std::string OVERRIDE_TEXT_DEFAULT = "Override";
const std::string ABORT_TEXT_DEFAULT = "Abort";


AchieveConditionInstruction::AchieveConditionInstruction()
  : CompoundInstruction(Type)
  , m_user_decision_needed{false}
{
  AddAttributeDefinition(MAIN_DIALOG_TEXT_ATTRIBUTE, sup::dto::StringType);
}

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
  if (!m_user_decision_needed)
  {
    return HandleAction(ui, ws);
  }
  switch (GetUserInput(ui))
  {
  case kRetry:
    m_user_decision_needed = false;
    return ExecutionStatus::NOT_FINISHED;
  case kOverride:
    return ExecutionStatus::SUCCESS;
  default:
    break;
  }
  return ExecutionStatus::FAILURE;
}

void AchieveConditionInstruction::ResetHook()
{
  ResetChildren();
  m_user_decision_needed = false;
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
  else
  {
    if (m_user_decision_needed)
    {
      return result;
    }
    auto action = children[1];
    auto action_status = action->GetStatus();
    if (ReadyForExecute(action_status))
    {
      result.push_back(action);
    }
  }
  return result;
}

ExecutionStatus AchieveConditionInstruction::HandleAction(UserInterface& ui, Workspace& ws)
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
    m_user_decision_needed = true;
  }
  return CalculateCompoundStatus();
}

AchieveConditionInstruction::UserDecision
AchieveConditionInstruction::GetUserInput(UserInterface &ui) const
{
  static std::map<int, UserDecision> decision_map = {
      {0, kRetry},
      {1, kOverride},
      {2, kFail}};
  std::string main_text = HasAttribute(MAIN_DIALOG_TEXT_ATTRIBUTE)
                            ? GetAttributeValue<std::string>(MAIN_DIALOG_TEXT_ATTRIBUTE)
                            : MAIN_DIALOG_TEXT_DEFAULT;
  auto metadata = CreateUserChoiceMetadata();
  metadata.AddMember(Constants::USER_CHOICES_TEXT_NAME, main_text);
  metadata.AddMember(Constants::USER_CHOICES_DIALOG_TYPE_NAME,
                     {sup::dto::UnsignedInteger32Type, dialog_type::kSelection});
  auto options = GetUserChoices();
  int choice = ui.GetUserChoice(options, metadata);
  if (choice < 0 || choice >= 3)
  {
    std::string warning_message = InstructionWarningProlog(*this) +
      "user choice [" + std::to_string(choice) + "] is not a valid value with three options";
    ui.LogWarning(warning_message);
    return kFail;
  }
  return decision_map[choice];
}

std::vector<std::string> AchieveConditionInstruction::GetUserChoices() const
{
  return { RETRY_TEXT_DEFAULT, OVERRIDE_TEXT_DEFAULT, ABORT_TEXT_DEFAULT };
}

ExecutionStatus AchieveConditionInstruction::CalculateCompoundStatus() const
{
  auto children = ChildInstructions();
  auto condition = children[0];
  auto action = children[1];
  auto condition_status = condition->GetStatus();
  auto action_status = action->GetStatus();
  // When condition failed, compound status depends on other child:
  if (condition_status != ExecutionStatus::FAILURE)
  {
    return condition_status;
  }
  // Only the running state needs to be propagated, since in all other cases, the compound
  // instruction is not finished or the status is directly determined by the user's response.
  if (action_status == ExecutionStatus::RUNNING)
  {
    return ExecutionStatus::RUNNING;
  }
  return ExecutionStatus::NOT_FINISHED;
}

} // namespace sequencer

} // namespace sup
