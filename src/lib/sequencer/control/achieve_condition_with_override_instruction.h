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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_ACHIEVE_CONDITION_OVERRIDE_INSTRUCTION_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_ACHIEVE_CONDITION_OVERRIDE_INSTRUCTION_H_

#include "wrapped_instruction_manager.h"

#include <sup/sequencer/compound_instruction.h>

#include <memory>

namespace sup
{
namespace sequencer
{

/**
 * @brief Try to achieve a condition. If it is already satisfied, returns SUCCESS. Otherwise, if
 * present, it executes an action to achieve the condition. If this still fails, or there was no
 * action defined (only one child instruction), the user gets the option to
 * retry again, override the decision (return SUCCESS) or terminate (return FAILURE).
 *
 * @details This compound instruction expects either one or two child instructions: the first one
 * is the condition to achieve and the second one, which is optional, is the instruction (or tree)
 * to execute when the condition is not (yet) satisfied.
 */
class AchieveConditionWithOverrideInstruction : public CompoundInstruction
{
public:
  AchieveConditionWithOverrideInstruction();
  ~AchieveConditionWithOverrideInstruction() override;

  static const std::string Type;

private:
  bool m_user_decision_needed;
  enum UserDecision {
    kRetry,
    kOverride,
    kFail
  };
  void SetupImpl(const Procedure& proc) override;
  ExecutionStatus ExecuteSingleImpl(UserInterface& ui, Workspace& ws) override;
  void ResetHook() override;
  std::vector<const Instruction*> NextInstructionsImpl() const override;

  bool ActionDefined() const;
  bool ActionNeeded() const;
  ExecutionStatus HandleAction(UserInterface& ui, Workspace& ws);
  UserDecision GetUserInput(const std::string& dialog_txt, UserInterface& ui) const;
  std::vector<std::string> GetUserChoices() const;
  ExecutionStatus CalculateCompoundStatus() const;
};

}  // namespace sequencer

}  // namespace sup

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_ACHIEVE_CONDITION_OVERRIDE_INSTRUCTION_H_
