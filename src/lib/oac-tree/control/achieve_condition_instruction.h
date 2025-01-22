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
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

#ifndef SUP_OAC_TREE_PLUGIN_CONTROL_ACHIEVE_CONDITION_INSTRUCTION_H_
#define SUP_OAC_TREE_PLUGIN_CONTROL_ACHIEVE_CONDITION_INSTRUCTION_H_

#include "wrapped_instruction_manager.h"

#include <sup/oac-tree/compound_instruction.h>

#include <memory>

namespace sup
{
namespace oac_tree
{

/**
 * @brief Try to achieve a condition. If it is already satisfied, returns SUCCESS. Otherwise, if
 * present, it executes an action to achieve the condition. If the condition still fails afterwards,
 * the instruction exits with FAILURE.
 *
 * @details This compound instruction expects exactly two child instructions: the first one
 * is the condition to achieve and the second one is the instruction (or tree)
 * to execute when the condition is not (yet) satisfied.
 */
class AchieveConditionInstruction : public CompoundInstruction
{
public:
  AchieveConditionInstruction();
  ~AchieveConditionInstruction() override;

  static const std::string Type;

private:
  bool m_action_done;
  void SetupImpl(const Procedure& proc) override;
  ExecutionStatus ExecuteSingleImpl(UserInterface& ui, Workspace& ws) override;
  void ResetHook(UserInterface& ui) override;
  std::vector<const Instruction*> NextInstructionsImpl() const override;

  void HandleAction(UserInterface& ui, Workspace& ws);
  ExecutionStatus CalculateCompoundStatus() const;
};

}  // namespace oac_tree

}  // namespace sup

#endif  // SUP_OAC_TREE_PLUGIN_CONTROL_ACHIEVE_CONDITION_INSTRUCTION_H_
