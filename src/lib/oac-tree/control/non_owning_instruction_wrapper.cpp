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

#include "non_owning_instruction_wrapper.h"

namespace sup {

namespace oac_tree {

const std::string kNonOwningInstructionWrapperType = "NonOwningInstructionWrapper";

NonOwningInstructionWrapper::NonOwningInstructionWrapper(Instruction* instr)
  : Instruction(kNonOwningInstructionWrapperType)
  , m_instr{instr}
{}

NonOwningInstructionWrapper::NonOwningInstructionWrapper(Instruction* instr,
                                                         const std::string &type)
  : Instruction(type)
  , m_instr{instr}
{}

NonOwningInstructionWrapper::~NonOwningInstructionWrapper() = default;

Instruction::Category NonOwningInstructionWrapper::GetCategory() const
{
  return GetInstruction()->GetCategory();
}

Instruction* NonOwningInstructionWrapper::GetInstruction()
{
  return m_instr;
}

const Instruction* NonOwningInstructionWrapper::GetInstruction() const
{
  return m_instr;
}

void NonOwningInstructionWrapper::SetupImpl(const Procedure& proc)
{
  GetInstruction()->Setup(proc);
}

ExecutionStatus NonOwningInstructionWrapper::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  GetInstruction()->ExecuteSingle(ui, ws);
  return GetInstruction()->GetStatus();
}

void NonOwningInstructionWrapper::HaltImpl()
{
  GetInstruction()->Halt();
}

void NonOwningInstructionWrapper::ResetHook(UserInterface& ui)
{
  GetInstruction()->Reset(ui);
}

int NonOwningInstructionWrapper::ChildrenCountImpl() const
{
  return GetInstruction()->ChildrenCount();
}

std::vector<const Instruction*> NonOwningInstructionWrapper::ChildInstructionsImpl() const
{
  return GetInstruction()->ChildInstructions();
}

bool NonOwningInstructionWrapper::InsertInstructionImpl(std::unique_ptr<Instruction>&& child,
                                                        int index)
{
  return GetInstruction()->InsertInstruction(std::move(child), index);
}

std::unique_ptr<Instruction> NonOwningInstructionWrapper::TakeInstructionImpl(int index)
{
  return GetInstruction()->TakeInstruction(index);
}

} // namespace oac_tree

} // namespace sup
