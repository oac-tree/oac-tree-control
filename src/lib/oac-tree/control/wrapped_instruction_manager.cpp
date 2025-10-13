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

#include "wrapped_instruction_manager.h"

#include "context_override_instruction_wrapper.h"
#include "wrapped_user_interface.h"

#include <sup/oac-tree/instruction.h>
#include <sup/oac-tree/instruction_tree.h>
#include <sup/oac-tree/workspace.h>

#include <algorithm>

namespace sup {

namespace oac_tree {

WrappedInstructionManager::WrappedInstructionManager()
  : m_wrapped_instructions{}
  , m_wrapped_ui{}
{}

WrappedInstructionManager::~WrappedInstructionManager() = default;

std::unique_ptr<Instruction> WrappedInstructionManager::CreateInstructionWrapper(Instruction& instr)
{
  (void)m_wrapped_instructions.emplace_back(std::make_unique<ContextOVerrideInstructionWrapper>(std::addressof(instr)));
  auto wrapper = m_wrapped_instructions.back().get();
  auto result = std::make_unique<NonOwningInstructionWrapper>(wrapper);
  return result;
}

UserInterface& WrappedInstructionManager::GetWrappedUI(UserInterface& ui, const std::string& prefix)
{
  SetContext(ui);
  if (!m_wrapped_ui)
  {
    m_wrapped_ui = std::make_unique<WrappedUserInterface>(ui, prefix);
  }
  return *m_wrapped_ui;
}

void WrappedInstructionManager::ClearWrappers()
{
  m_wrapped_instructions.clear();
  m_wrapped_ui.reset();
}

void WrappedInstructionManager::SetContext(UserInterface& ui)
{
  for (auto& instr : m_wrapped_instructions)
  {
    instr->SetUserInterface(ui);
  }
}

} // namespace oac_tree

} // namespace sup
