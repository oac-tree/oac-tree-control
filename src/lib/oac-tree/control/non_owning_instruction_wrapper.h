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

#ifndef SUP_OAC_TREE_PLUGIN_CONTROL_NON_OWNING_INSTRUCTION_WRAPPER_H_
#define SUP_OAC_TREE_PLUGIN_CONTROL_NON_OWNING_INSTRUCTION_WRAPPER_H_

#include <sup/oac-tree/instruction.h>

namespace sup
{
namespace oac_tree
{

/**
 * @brief Instruction wrapper that can inject a different UserInterface during execution.
 */
class NonOwningInstructionWrapper : public Instruction
{
public:
  NonOwningInstructionWrapper(Instruction* instr);
  NonOwningInstructionWrapper(Instruction* instr, const std::string &type);
  ~NonOwningInstructionWrapper() override;

  Category GetCategory() const override;

  static const std::string Type;

protected:
  Instruction* GetInstruction();
  const Instruction* GetInstruction() const;

private:
  Instruction* m_instr;  // wrapped instruction
  void SetupImpl(const Procedure& proc) override;
  ExecutionStatus ExecuteSingleImpl(UserInterface& ui, Workspace& ws) override;
  void HaltImpl() override;
  void ResetHook(UserInterface& ui) override;
  int ChildrenCountImpl() const override;
  std::vector<const Instruction*> ChildInstructionsImpl() const override;
  bool InsertInstructionImpl(std::unique_ptr<Instruction>&& child, int index) override;
  std::unique_ptr<Instruction> TakeInstructionImpl(int index) override;
};

}  // namespace oac_tree

}  // namespace sup

#endif  // SUP_OAC_TREE_PLUGIN_CONTROL_NON_OWNING_INSTRUCTION_WRAPPER_H_
