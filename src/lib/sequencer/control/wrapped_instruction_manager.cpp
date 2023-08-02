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

#include "wrapped_instruction_manager.h"

#include "ui_override_instruction_wrapper.h"
#include "wrapped_user_interface.h"

#include <sup/sequencer/instruction.h>

namespace sup {

namespace sequencer {

WrappedInstructionManager::WrappedInstructionManager()
  : m_wrapped_instructions{}
{}

WrappedInstructionManager::~WrappedInstructionManager() = default;

std::unique_ptr<Instruction> WrappedInstructionManager::CreateInstructionWrapper(Instruction& instr)
{
  m_wrapped_instructions.emplace_back(new UIOVerrideInstructionWrapper(std::addressof(instr)));
  auto wrapper = m_wrapped_instructions.back().get();
  std::unique_ptr<Instruction> result{new NonOwningInstructionWrapper(wrapper)};
  return result;
}

void WrappedInstructionManager::SetUserInterface(UserInterface& ui)
{
  for (auto& instr : m_wrapped_instructions)
  {
    instr->SetUserInterface(ui);
  }
}

UserInterface& WrappedInstructionManager::GetWrappedUI(UserInterface& ui, const std::string& prefix)
{
  if (!m_wrapped_ui)
  {
    m_wrapped_ui.reset(new WrappedUserInterface(ui, prefix));
  }
  return *m_wrapped_ui;
}

void WrappedInstructionManager::ClearWrappers()
{
  m_wrapped_instructions.clear();
  m_wrapped_ui.reset();
}

} // namespace sequencer

} // namespace sup
