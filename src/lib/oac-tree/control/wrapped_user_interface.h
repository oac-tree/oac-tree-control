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

#ifndef SUP_OAC_TREE_PLUGIN_CONTROL_WRAPPED_USER_INTERFACE_H_
#define SUP_OAC_TREE_PLUGIN_CONTROL_WRAPPED_USER_INTERFACE_H_

#include <sup/oac-tree/user_interface.h>

namespace sup
{
namespace oac_tree
{
class Instruction;
/**
 * @brief UserInterface wrapper that only forwards log messages
 */
class WrappedUserInterface : public DefaultUserInterface
{
public:
  WrappedUserInterface(UserInterface& ui, const std::string& prefix);
  ~WrappedUserInterface() override;

private:
  UserInterface& m_ui;
  std::string m_prefix;
  void Log(int severity, const std::string& message) override;
};

}  // namespace oac_tree

}  // namespace sup

#endif  // SUP_OAC_TREE_PLUGIN_CONTROL_WRAPPED_USER_INTERFACE_H_
