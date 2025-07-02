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

#ifndef SUP_OAC_TREE_PLUGIN_CONTROL_EXECUTE_WHILE_INSTRUCTION_H_
#define SUP_OAC_TREE_PLUGIN_CONTROL_EXECUTE_WHILE_INSTRUCTION_H_

#include "wrapped_instruction_manager.h"

#include <sup/oac-tree/compound_instruction.h>

#include <memory>

namespace sup
{
namespace oac_tree
{

/**
 * @brief Executes an instruction while continuously checking a condition still holds. It interrupts
 * the execution and returns FAILURE when the condition becomes false. The instruction only returns
 * SUCCESS if execution of the tree was successful and the condition remained true all along.
 *
 * @details This compound instruction expects exactly two child instructions: the first one is the
 * instruction (or tree) to execute while the condition holds and the second one is the condition
 * to check.
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
  std::unique_ptr<Instruction> CreateWrappedInstructionTree();

  std::unique_ptr<Instruction> m_internal_instruction_tree;
  WrappedInstructionManager m_instr_manager;
};

}  // namespace oac_tree

}  // namespace sup

#endif  // SUP_OAC_TREE_PLUGIN_CONTROL_EXECUTE_WHILE_INSTRUCTION_H_
