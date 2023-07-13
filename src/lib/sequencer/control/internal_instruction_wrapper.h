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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_INTERNAL_INSTRUCTION_WRAPPER_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_INTERNAL_INSTRUCTION_WRAPPER_H_

#include <sup/sequencer/decorator_instruction.h>

namespace sup
{
namespace sequencer
{

/**
 * @brief Instruction wrapper that can inject a different UserInterface during execution.
 */
class InternalInstructionWrapper : public Instruction
{
public:
  InternalInstructionWrapper(Instruction* instr);
  ~InternalInstructionWrapper() override;

  void SetUserInterface(UserInterface* ui);

  static const std::string Type;

private:
  Instruction* m_instr;  // wrapped instruction
  UserInterface* m_ui;   // userinterface to pass to child instruction
  void ResetHook() override;
  void HaltImpl() override;
  std::vector<const Instruction*> NextInstructionsImpl() const override;
  void SetupImpl(const Procedure& proc) override;
  ExecutionStatus ExecuteSingleImpl(UserInterface& ui, Workspace& ws) override;
};

}  // namespace sequencer

}  // namespace sup

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_INTERNAL_INSTRUCTION_WRAPPER_H_
