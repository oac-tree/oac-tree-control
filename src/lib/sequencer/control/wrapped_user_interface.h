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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_WRAPPED_USER_INTERFACE_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_WRAPPED_USER_INTERFACE_H_

#include <sup/sequencer/user_interface.h>

namespace sup
{
namespace sequencer
{
class Instruction;
/**
 * @brief UserInterface wrapper that only forwards log messages
 */
class WrappedUserInterface : public DefaultUserInterface
{
public:
  WrappedUserInterface(UserInterface& ui, const std::string& prefix);
  ~WrappedUserInterface();

private:
  UserInterface& m_ui;
  std::string m_prefix;
  void UpdateInstructionStatus(const Instruction* instruction) override;
  void Log(int severity, const std::string& message) override;
};

}  // namespace sequencer

}  // namespace sup

#endif  // SUP_SEQUENCER_PLUGIN_CONTROL_WRAPPED_USER_INTERFACE_H_
