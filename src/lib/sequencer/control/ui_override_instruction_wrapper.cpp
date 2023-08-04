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

#include "ui_override_instruction_wrapper.h"

namespace sup {

namespace sequencer {

const std::string UIOVerrideInstructionWrapper::Type = "UIOVerrideInstructionWrapper";

UIOVerrideInstructionWrapper::UIOVerrideInstructionWrapper(Instruction* instr)
  : NonOwningInstructionWrapper(instr, Type)
  , m_ui{}
{}

UIOVerrideInstructionWrapper::~UIOVerrideInstructionWrapper() = default;

void UIOVerrideInstructionWrapper::SetUserInterface(UserInterface& ui)
{
  m_ui = std::addressof(ui);
}

ExecutionStatus UIOVerrideInstructionWrapper::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  auto selected_ui = m_ui == nullptr ? std::addressof(ui)
                                     : m_ui;
  GetInstruction()->ExecuteSingle(*selected_ui, ws);
  return GetInstruction()->GetStatus();
}

} // namespace sequencer

} // namespace sup
