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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_WRAPPED_INSTRUCTION_MANAGER_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_WRAPPED_INSTRUCTION_MANAGER_H_

#include <memory>
#include <vector>

namespace sup
{
namespace sequencer
{
class Instruction;
class UIOVerrideInstructionWrapper;
class UserInterface;
/**
 * @brief
 */
class WrappedInstructionManager
{
public:
  WrappedInstructionManager();
  ~WrappedInstructionManager();

  std::unique_ptr<Instruction> CreateInstructionWrapper(Instruction& instr);

  void SetUserInterface(UserInterface& ui);

  void ClearWrappers();

private:
  std::vector<std::unique_ptr<UIOVerrideInstructionWrapper>> m_wrapped_instructions;
};

}  // namespace sequencer

}  // namespace sup

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_WRAPPED_INSTRUCTION_MANAGER_H_
