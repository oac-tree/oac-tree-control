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

#include <sup/sequencer/constants.h>

namespace sup {

namespace sequencer {

namespace test {

TestUserInputInterface::TestUserInputInterface()
  : m_main_text{}
  , m_user_choices{}
  , m_current_index{0}
{}

TestUserInputInterface::~TestUserInputInterface() = default;

void TestUserInputInterface::SetUserChoices(const std::vector<int>& user_choices)
{
  m_user_choices = user_choices;
  m_current_index = 0;
}

int TestUserInputInterface::GetUserChoice(const std::vector<std::string>& options,
                                              const sup::dto::AnyValue& metadata)
{
  (void)options;
  m_main_text = metadata[Constants::USER_CHOICES_TEXT_NAME].As<std::string>();
  if (m_user_choices.empty())
  {
    return -1;
  }
  if (m_current_index == m_user_choices.size())
  {
    m_current_index = 0;
  }
  return m_user_choices[m_current_index++];
}

} // namespace test

} // namespace sequencer

} // namespace sup
