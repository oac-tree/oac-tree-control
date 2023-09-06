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

#ifndef SUP_SEQUENCER_PLUGIN_CONTROL_TEST_USER_INTERFACE_H_
#define SUP_SEQUENCER_PLUGIN_CONTROL_TEST_USER_INTERFACE_H_

#include <sup/sequencer/user_interface.h>

#include <utility>
#include <vector>

namespace sup {

namespace sequencer {

namespace test {

using NullUserInterface = DefaultUserInterface;

class TestUserInputInterface : public DefaultUserInterface
{
public:
  TestUserInputInterface();
  ~TestUserInputInterface();

  void SetUserChoices(const std::vector<int>& user_choices);

  int GetUserChoice(const std::vector<std::string>& options,
                        const sup::dto::AnyValue& metadata) override;

  std::string m_main_text;
private:
  std::vector<int> m_user_choices;
  std::size_t m_current_index;
};

} // namespace test

} // namespace sequencer

} // namespace sup

#endif // SUP_SEQUENCER_PLUGIN_CONTROL_TEST_USER_INTERFACE_H_
