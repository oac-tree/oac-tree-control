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
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

#include "wrapped_instruction_manager.h"

#include "context_override_instruction_wrapper.h"
#include "wrapped_user_interface.h"

#include <sup/oac-tree/instruction.h>
#include <sup/oac-tree/instruction_tree.h>
#include <sup/oac-tree/workspace.h>

#include <algorithm>

namespace
{
using namespace sup::oac_tree;
std::vector<const Instruction*> Intersection(const std::vector<const Instruction*>& left_set,
                                             const std::vector<const Instruction*>& right_set);
}  // unnamed namespace

namespace sup {

namespace oac_tree {

WrappedInstructionManager::WrappedInstructionManager()
  : m_wrapped_instructions{}
  , m_wrapped_ui{}
  , m_local_workspace{}
{}

WrappedInstructionManager::~WrappedInstructionManager() = default;

std::unique_ptr<Instruction> WrappedInstructionManager::CreateInstructionWrapper(Instruction& instr)
{
  m_wrapped_instructions.emplace_back(std::make_unique<ContextOVerrideInstructionWrapper>(std::addressof(instr)));
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

Workspace& WrappedInstructionManager::GetLocalWorkspace()
{
  if (!m_local_workspace)
  {
    m_local_workspace = std::make_unique<Workspace>();
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

} // namespace oac_tree

} // namespace sup

namespace
{
using namespace sup::oac_tree;
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
