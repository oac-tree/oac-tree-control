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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_ACHIEVE_CONDITION_WITH_TIMEOUT_INSTRUCTION_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_ACHIEVE_CONDITION_WITH_TIMEOUT_INSTRUCTION_H_

#include "wrapped_instruction_manager.h"

#include <sup/sequencer/compound_instruction.h>

#include <memory>

namespace sup
{
namespace sequencer
{

/**
 * @brief Try to achieve a condition. If it is already satisfied, returns SUCCESS. Otherwise, if
 * present, it executes an action to achieve the condition. If the condition still fails afterwards,
 * the instruction exits with FAILURE. The second check is done using a timeout to allow a system
 * to reach the condition.
 *
 * @details This compound instruction expects exactly two child instructions: the first one
 * is the condition to achieve and the second one is the instruction (or tree)
 * to execute when the condition is not (yet) satisfied.
 */
class AchieveConditionWithTimeoutInstruction : public CompoundInstruction
{
public:
  AchieveConditionWithTimeoutInstruction();
  ~AchieveConditionWithTimeoutInstruction() override;

  static const std::string Type;

private:
  void SetupImpl(const Procedure& proc) override;
  ExecutionStatus ExecuteSingleImpl(UserInterface& ui, Workspace& ws) override;
  void HaltImpl() override;
  void ResetHook(UserInterface& ui) override;
  std::vector<const Instruction*> NextInstructionsImpl() const override;
  std::unique_ptr<Instruction> CreateWrappedInstructionTree();

  std::unique_ptr<Instruction> m_internal_instruction_tree;
  WrappedInstructionManager m_instr_manager;
};

}  // namespace sequencer

}  // namespace sup

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_ACHIEVE_CONDITION_WITH_TIMEOUT_INSTRUCTION_H_
