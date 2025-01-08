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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_CONTEXT_OVERRIDE_INSTRUCTION_WRAPPER_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_CONTEXT_OVERRIDE_INSTRUCTION_WRAPPER_H_

#include "non_owning_instruction_wrapper.h"

namespace sup
{
namespace sequencer
{

/**
 * @brief Instruction wrapper that can inject a different UserInterface during execution.
 */
class ContextOVerrideInstructionWrapper : public NonOwningInstructionWrapper
{
public:
  ContextOVerrideInstructionWrapper(Instruction* instr);
  ~ContextOVerrideInstructionWrapper() override;

  void SetUserInterface(UserInterface& ui);

  static const std::string Type;

private:
  UserInterface* m_ui;
  ExecutionStatus ExecuteSingleImpl(UserInterface& ui, Workspace& ws) override;
  void ResetHook(UserInterface& ui) override;
};

}  // namespace sequencer

}  // namespace sup

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_CONTEXT_OVERRIDE_INSTRUCTION_WRAPPER_H_
