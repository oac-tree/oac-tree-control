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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_WAIT_FOR_CONDITION_INSTRUCTION_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_WAIT_FOR_CONDITION_INSTRUCTION_H_

#include "wrapped_instruction_manager.h"

#include <sup/sequencer/decorator_instruction.h>

#include <memory>

namespace sup
{
namespace sequencer
{

/**
 * @brief Waits with a timeout for a condition to be satisfied. The instruction fails if the timeout
 * was reached before the condition became true.
 */
class WaitForConditionInstruction : public DecoratorInstruction
{
public:
  WaitForConditionInstruction();
  ~WaitForConditionInstruction() override;

  static const std::string Type;

private:
  void SetupImpl(const Procedure& proc) override;
  ExecutionStatus ExecuteSingleImpl(UserInterface& ui, Workspace& ws) override;
  void HaltImpl() override;
  void ResetHook() override;
  std::vector<const Instruction*> NextInstructionsImpl() const override;
  std::unique_ptr<Instruction> CreateWrappedInstructionTree();

  std::unique_ptr<Instruction> m_internal_instruction_tree;
  WrappedInstructionManager m_instr_manager;
};

}  // namespace sequencer

}  // namespace sup

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_WAIT_FOR_CONDITION_INSTRUCTION_H_
