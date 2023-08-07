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

#include "test_user_interface.h"

#include <sstream>

namespace sup {

namespace sequencer {

namespace test {

NullUserInterface::NullUserInterface() = default;

NullUserInterface::~NullUserInterface() = default;

void NullUserInterface::UpdateInstructionStatusImpl(const Instruction*)
{}

LogUserInterface::LogUserInterface()
  : m_log_entries{}
{}

LogUserInterface::~LogUserInterface() = default;

void LogUserInterface::UpdateInstructionStatusImpl(const Instruction*)
{}

void LogUserInterface::LogImpl(int severity, const std::string& message)
{
  m_log_entries.emplace_back(severity, message);
}

} // namespace test

} // namespace sequencer

} // namespace sup
