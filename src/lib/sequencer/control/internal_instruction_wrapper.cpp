/******************************************************************************
 * $HeadURL: $
 * $Id: $
 *
 * Project       : SUP - Sequencer
 *
 * Description   : Sequencer for operational procedures
 *
 * Author        : Walter Van Herck (IO)
 *
 * Copyright (c) : 2010-2022 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
******************************************************************************/

#include "internal_instruction_wrapper.h"

namespace sup {

namespace sequencer {

const std::string InternalInstructionWrapper::Type = "InternalInstructionWrapper";

InternalInstructionWrapper::InternalInstructionWrapper(Instruction* instr)
  : Instruction(Type)
  , m_instr{instr}
  , m_ui{nullptr}
{
  if (!m_instr)
  {
    std::string error_message = InstructionErrorProlog(*this) +
      "Constructing a wrapper without passing a valid instruction pointer";
    throw InvalidOperationException(error_message);
  }
}

InternalInstructionWrapper::~InternalInstructionWrapper() = default;

void InternalInstructionWrapper::SetUserInterface(UserInterface* ui)
{
  m_ui = ui;
}

void InternalInstructionWrapper::ResetHook()
{
  m_instr->Reset();
}

void InternalInstructionWrapper::HaltImpl()
{
  m_instr->Halt();
}

std::vector<const Instruction*> InternalInstructionWrapper::NextInstructionsImpl() const
{
  std::vector<const Instruction*> result;
  result.push_back(m_instr);
  return result;
}

void InternalInstructionWrapper::SetupImpl(const Procedure& proc)
{
  m_instr->Setup(proc);
}

ExecutionStatus InternalInstructionWrapper::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  auto child_status = m_instr->GetStatus();
  if (NeedsExecute(child_status))
  {
    UserInterface* ui_to_use = static_cast<bool>(m_ui) ? m_ui : &ui;
    m_instr->ExecuteSingle(*ui_to_use, ws);
  }
  return m_instr->GetStatus();
}

} // namespace sequencer

} // namespace sup
