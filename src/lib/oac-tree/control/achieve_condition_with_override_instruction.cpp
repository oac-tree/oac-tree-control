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

#include "achieve_condition_with_override_instruction.h"

#include <sup/oac-tree/constants.h>
#include <sup/oac-tree/instruction_registry.h>
#include <sup/oac-tree/instruction_utils.h>
#include <sup/oac-tree/user_interface.h>

namespace sup {

namespace oac_tree {

const std::string AchieveConditionWithOverrideInstruction::Type = "AchieveConditionWithOverride";
static bool _achieve_condition_with_override_initialised_flag =
  RegisterGlobalInstruction<AchieveConditionWithOverrideInstruction>();

const std::string MAIN_DIALOG_TEXT_ATTRIBUTE = "dialogText";
const std::string MAIN_DIALOG_TEXT_DEFAULT =
  "Condition is still not satisfied. Please select action.";

const std::string RETRY_TEXT_DEFAULT = "Retry";
const std::string OVERRIDE_TEXT_DEFAULT = "Override";
const std::string ABORT_TEXT_DEFAULT = "Abort";


AchieveConditionWithOverrideInstruction::AchieveConditionWithOverrideInstruction()
  : CompoundInstruction(Type)
  , m_user_decision_needed{false}
{
  (void)AddAttributeDefinition(MAIN_DIALOG_TEXT_ATTRIBUTE).SetCategory(AttributeCategory::kBoth);
}

AchieveConditionWithOverrideInstruction::~AchieveConditionWithOverrideInstruction() = default;

void AchieveConditionWithOverrideInstruction::SetupImpl(const Procedure& proc)
{
  (void)proc;
  auto children = ChildInstructions();
  if (children.size() < 1 || children.size() > 2)
  {
    std::string error_message = InstructionErrorProlog(*this) +
      "This compound instruction requires either one or two child instructions";
    throw InstructionSetupException(error_message);
  }
  SetupChildren(proc);
}

ExecutionStatus AchieveConditionWithOverrideInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  auto children = ChildInstructions();
  auto condition = children[0];
  auto condition_status = condition->GetStatus();
  if (NeedsExecute(condition_status))
  {
    children[0]->ExecuteSingle(ui, ws);
    return CalculateCompoundStatus();
  }
  if (ActionNeeded())
  {
    return HandleAction(ui, ws);
  }
  std::string main_text = MAIN_DIALOG_TEXT_DEFAULT;
  if (!GetAttributeValueAs(MAIN_DIALOG_TEXT_ATTRIBUTE, ws, ui, main_text))
  {
    return ExecutionStatus::FAILURE;
  }
  switch (GetUserInput(main_text, ui))
  {
  case kRetry:
    ResetHook(ui);
    return ExecutionStatus::NOT_FINISHED;
  case kOverride:
    return ExecutionStatus::SUCCESS;
  default:
    break;
  }
  return ExecutionStatus::FAILURE;
}

void AchieveConditionWithOverrideInstruction::ResetHook(UserInterface& ui)
{
  ResetChildren(ui);
  m_user_decision_needed = false;
}

bool AchieveConditionWithOverrideInstruction::ActionDefined() const
{
  return ChildrenCount() == 2;
}

bool AchieveConditionWithOverrideInstruction::ActionNeeded() const
{
  return !m_user_decision_needed && ActionDefined();
}

ExecutionStatus AchieveConditionWithOverrideInstruction::HandleAction(UserInterface& ui, Workspace& ws)
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
    ResetChildren(ui);
    m_user_decision_needed = true;
  }
  return CalculateCompoundStatus();
}

AchieveConditionWithOverrideInstruction::UserDecision
AchieveConditionWithOverrideInstruction::GetUserInput(const std::string& dialog_txt, UserInterface &ui) const
{
  static std::map<int, UserDecision> decision_map = {
      {0, kRetry},
      {1, kOverride},
      {2, kFail}};
  auto metadata = CreateUserChoiceMetadata();
  (void)metadata.AddMember(Constants::USER_CHOICES_TEXT_NAME, dialog_txt);
  (void)metadata.AddMember(Constants::USER_CHOICES_DIALOG_TYPE_NAME,
                     {sup::dto::UnsignedInteger32Type, dialog_type::kSelection});
  auto options = GetUserChoices();
  auto [retrieved, choice] = GetInterruptableUserChoice(ui, *this, options, metadata);
  if (!retrieved)
  {
    std::string warning_message = InstructionWarningProlog(*this) +
      "did not receive valid choice";
    LogWarning(ui, warning_message);
    return kFail;
  }
  if (choice < 0 || choice >= 3)
  {
    std::string warning_message = InstructionWarningProlog(*this) +
      "user choice [" + std::to_string(choice) + "] is invalid. Valid values are 0, 1 or 2.";
    LogWarning(ui, warning_message);
    return kFail;
  }
  return decision_map[choice];
}

std::vector<std::string> AchieveConditionWithOverrideInstruction::GetUserChoices() const
{
  return { RETRY_TEXT_DEFAULT, OVERRIDE_TEXT_DEFAULT, ABORT_TEXT_DEFAULT };
}

ExecutionStatus AchieveConditionWithOverrideInstruction::CalculateCompoundStatus() const
{
  auto children = ChildInstructions();
  auto condition = children[0];
  auto condition_status = condition->GetStatus();
  auto action_status = ActionDefined() ? children[1]->GetStatus()
                                       : ExecutionStatus::NOT_STARTED;
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

} // namespace oac_tree

} // namespace sup
