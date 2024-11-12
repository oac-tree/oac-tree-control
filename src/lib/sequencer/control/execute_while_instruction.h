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
* Copyright (c) : 2010-2024 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_EXECUTE_WHILE_INSTRUCTION_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_EXECUTE_WHILE_INSTRUCTION_H_

#include "wrapped_instruction_manager.h"

#include <sup/sequencer/compound_instruction.h>

#include <memory>

namespace sup
{
namespace sequencer
{

/**
 * @brief Executes an instruction while continuously checking a condition still holds. It interrupts
 * the execution and returns FAILURE when the condition becomes false. The instruction only returns
 * SUCCESS if execution of the tree was successful and the condition remained true all along.
 *
 * @details This compound instruction expects exactly two child instructions: the first one is the
 * instruction (or tree) to execute while the condition holds and the second one is the condition
 * to check.
 * The attribute 'varNames' is mandatory and denotes the variables on which the condition depends.
 * This is necessary, so the instruction will only check the condition on those variable changes.
 */
class ExecuteWhileInstruction : public CompoundInstruction
{
public:
  ExecuteWhileInstruction();
  ~ExecuteWhileInstruction() override;

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

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_EXECUTE_WHILE_INSTRUCTION_H_
