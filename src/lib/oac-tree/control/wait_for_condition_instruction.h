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

#ifndef SUP_OAC_TREE_PLUGIN_CONTROL_WAIT_FOR_CONDITION_INSTRUCTION_H_
#define SUP_OAC_TREE_PLUGIN_CONTROL_WAIT_FOR_CONDITION_INSTRUCTION_H_

#include "wrapped_instruction_manager.h"

#include <sup/oac-tree/decorator_instruction.h>

#include <memory>

namespace sup
{
namespace oac_tree
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
  void ResetHook(UserInterface& ui) override;
  std::vector<const Instruction*> NextInstructionsImpl() const override;
  std::unique_ptr<Instruction> CreateWrappedInstructionTree();

  std::unique_ptr<Instruction> m_internal_instruction_tree;
  WrappedInstructionManager m_instr_manager;
};

}  // namespace oac_tree

}  // namespace sup

#endif  // SUP_OAC_TREE_PLUGIN_CONTROL_WAIT_FOR_CONDITION_INSTRUCTION_H_
