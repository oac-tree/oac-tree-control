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

#include "wait_for_condition_instruction.h"

#include "internal_instruction_wrapper.h"

#include <sup/sequencer/instruction_registry.h>
#include <sup/sequencer/instruction_utils.h>

#include <cmath>

namespace sup {

namespace sequencer {

const std::string WaitForConditionInstruction::Type = "WaitForCondition";
static bool _dummy_initialised_flag = RegisterGlobalInstruction<WaitForConditionInstruction>();

const std::string TIMEOUT_ATTR_NAME = "timeout";
const std::string VARNAMES_ATTRIBUTE_NAME = "varNames";

WaitForConditionInstruction::WaitForConditionInstruction()
  : DecoratorInstruction(Type)
  , m_internal_instruction_tree{}
{
  AddAttributeDefinition(VARNAMES_ATTRIBUTE_NAME, sup::dto::StringType).SetMandatory();
  AddAttributeDefinition(TIMEOUT_ATTR_NAME, sup::dto::Float64Type);
}

WaitForConditionInstruction::~WaitForConditionInstruction() = default;

void WaitForConditionInstruction::SetupImpl(const Procedure& proc)
{
  // Currently it just wraps its child. This is done to demonstrate that this strategy works.
  auto children = ChildInstructions();
  if (children.size() != 1)
  {
    std::string error_message = InstructionErrorProlog(*this) +
      "Trying to setup this instruction without a child";
    throw InstructionSetupException(error_message);
  }
  m_internal_instruction_tree.reset(new InternalInstructionWrapper(children[0]));
  m_internal_instruction_tree->Setup(proc);
}

ExecutionStatus WaitForConditionInstruction::ExecuteSingleImpl(UserInterface& ui, Workspace& ws)
{
  m_internal_instruction_tree->ExecuteSingle(ui, ws);
  return m_internal_instruction_tree->GetStatus();
}

} // namespace sequencer

} // namespace sup
