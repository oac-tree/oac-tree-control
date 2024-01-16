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

#include "wrapped_instruction_manager.h"

#include "context_override_instruction_wrapper.h"
#include "wrapped_user_interface.h"

#include <sup/sequencer/instruction.h>
#include <sup/sequencer/instruction_tree.h>
#include <sup/sequencer/workspace.h>

#include <algorithm>

namespace
{
using namespace sup::sequencer;
std::vector<const Instruction*> Intersection(const std::vector<const Instruction*>& left_set,
                                             const std::vector<const Instruction*>& right_set);
}  // unnamed namespace

namespace sup {

namespace sequencer {

WrappedInstructionManager::WrappedInstructionManager()
  : m_wrapped_instructions{}
  , m_wrapped_ui{}
  , m_local_workspace{}
{}

WrappedInstructionManager::~WrappedInstructionManager() = default;

std::unique_ptr<Instruction> WrappedInstructionManager::CreateInstructionWrapper(Instruction& instr)
{
  m_wrapped_instructions.emplace_back(new ContextOVerrideInstructionWrapper(std::addressof(instr)));
  auto wrapper = m_wrapped_instructions.back().get();
  std::unique_ptr<Instruction> result{new NonOwningInstructionWrapper(wrapper)};
  return result;
}

UserInterface& WrappedInstructionManager::GetWrappedUI(UserInterface& ui, const std::string& prefix)
{
  SetContext(ui);
  if (!m_wrapped_ui)
  {
    m_wrapped_ui.reset(new WrappedUserInterface(ui, prefix));
  }
  return *m_wrapped_ui;
}

Workspace& WrappedInstructionManager::GetLocalWorkspace()
{
  if (!m_local_workspace)
  {
    m_local_workspace.reset(new Workspace{});
  }
  return *m_local_workspace;
}

void WrappedInstructionManager::ClearWrappers()
{
  m_wrapped_instructions.clear();
  m_wrapped_ui.reset();
  m_local_workspace.reset();
}

std::vector<const Instruction*> FilterNextInstructions(const Instruction& instr,
                                                       const Instruction* tree)
{
  if (tree == nullptr)
  {
    return {};
  }
  auto children = instr.ChildInstructions();
  if (children.size() > 0)
  {
    auto next_instr = FlattenBFS(CreateNextInstructionTree(tree));
    return Intersection(next_instr, children);
  }
  return {};
}

void WrappedInstructionManager::SetContext(UserInterface& ui)
{
  for (auto& instr : m_wrapped_instructions)
  {
    instr->SetUserInterface(ui);
  }
}

} // namespace sequencer

} // namespace sup

namespace
{
using namespace sup::sequencer;
std::vector<const Instruction*> Intersection(const std::vector<const Instruction*>& left_set,
                                             const std::vector<const Instruction*>& right_set)
{
  std::vector<const Instruction*> result;
  for (auto left_instr : left_set)
  {
    if (std::find(right_set.begin(), right_set.end(), left_instr) != right_set.end())
    {
      result.push_back(left_instr);
    }
  }
  return result;
}

}  // unnamed namespace
