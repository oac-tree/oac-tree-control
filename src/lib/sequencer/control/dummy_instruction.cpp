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

#include "dummy_instruction.h"

#include <sup/sequencer/instruction_registry.h>

#include <iostream>

namespace sup {

namespace sequencer {

const std::string DummyInstruction::Type = "Dummy";
static bool _dummy_initialised_flag = RegisterGlobalInstruction<DummyInstruction>();

const std::string DUMMY_TEXT_ATTRIBUTE_NAME = "text";

DummyInstruction::DummyInstruction()
  : Instruction(Type)
{
  AddAttributeDefinition(DUMMY_TEXT_ATTRIBUTE_NAME, sup::dto::StringType).SetMandatory();
}

DummyInstruction::~DummyInstruction() = default;

ExecutionStatus DummyInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  (void)ui;
  (void)ws;
  if (HasAttribute(DUMMY_TEXT_ATTRIBUTE_NAME))
  {
    auto text = GetAttributeValue<std::string>(DUMMY_TEXT_ATTRIBUTE_NAME);
    std::cout << text << std::endl;
  }
  return ExecutionStatus::SUCCESS;
}

} // namespace sequencer

} // namespace sup
