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

#include "non_owning_instruction_wrapper.h"

namespace sup {

namespace sequencer {

const std::string NonOwningInstructionWrapper::Type = "NonOwningInstructionWrapper";

NonOwningInstructionWrapper::NonOwningInstructionWrapper(Instruction* instr)
  : Instruction(Type)
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
  return m_instr->GetCategory();
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
  m_instr->Setup(proc);
}

ExecutionStatus NonOwningInstructionWrapper::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  m_instr->ExecuteSingle(ui, ws);
  return m_instr->GetStatus();
}

void NonOwningInstructionWrapper::HaltImpl()
{
  m_instr->Halt();
}

void NonOwningInstructionWrapper::ResetHook(UserInterface& ui)
{
  m_instr->Reset(ui);
}

int NonOwningInstructionWrapper::ChildrenCountImpl() const
{
  return m_instr->ChildrenCount();
}

std::vector<const Instruction*> NonOwningInstructionWrapper::ChildInstructionsImpl() const
{
  return GetInstruction()->ChildInstructions();
}

std::vector<const Instruction*> NonOwningInstructionWrapper::NextInstructionsImpl() const
{
  return m_instr->NextInstructions();
}

bool NonOwningInstructionWrapper::InsertInstructionImpl(std::unique_ptr<Instruction>&& child,
                                                        int index)
{
  return m_instr->InsertInstruction(std::move(child), index);
}

std::unique_ptr<Instruction> NonOwningInstructionWrapper::TakeInstructionImpl(int index)
{
  return m_instr->TakeInstruction(index);
}

} // namespace sequencer

} // namespace sup
