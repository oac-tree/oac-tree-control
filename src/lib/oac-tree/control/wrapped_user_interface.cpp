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
* Copyright (c) : 2010-2026 ITER Organization,
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

#include "wrapped_user_interface.h"

namespace sup {

namespace oac_tree {

WrappedUserInterface::WrappedUserInterface(UserInterface& ui, const std::string& prefix)
  : m_ui{ui}
  , m_prefix{prefix}
{}

WrappedUserInterface::~WrappedUserInterface() = default;

void WrappedUserInterface::Log(int severity, const std::string& message)
{
  std::string wrapped_message = m_prefix + message;
  m_ui.Log(severity, wrapped_message);
}

} // namespace oac_tree

} // namespace sup
