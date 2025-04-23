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

#include "test_user_interface.h"

#include <sup/oac-tree/constants.h>

namespace sup {

namespace oac_tree {

namespace test {

using namespace std::placeholders;

TestUserInputInterface::TestUserInputInterface()
  : m_main_text{}
  , m_input_adapter{std::bind(&TestUserInputInterface::UserInput, this, _1, _2),
                    std::bind(&TestUserInputInterface::Interrupt, this, _1)}
  , m_user_choices{}
  , m_current_index{0}
{}

TestUserInputInterface::~TestUserInputInterface() = default;

void TestUserInputInterface::SetUserChoices(const std::vector<int>& user_choices)
{
  m_user_choices = user_choices;
  m_current_index = 0;
}

std::unique_ptr<IUserInputFuture> TestUserInputInterface::RequestUserInput(
  const UserInputRequest& request)
{
  return m_input_adapter.AddUserInputRequest(request);
}

UserInputReply TestUserInputInterface::UserInput(const UserInputRequest& request,
                                                 sup::dto::uint64 id)
{
  (void)id;
  switch (request.m_request_type)
  {
  case InputRequestType::kUserValue:
  {
    auto failure = CreateUserValueReply(false, {});
    return failure;  // Not supported for this test class
  }
  case InputRequestType::kUserChoice:
  {
    auto failure = CreateUserChoiceReply(false, -1);
    std::vector<std::string> options{};
    sup::dto::AnyValue metadata{};
    if (!ParseUserChoiceRequest(request, options, metadata))
    {
      return failure;
    }
    auto choice = GetUserChoice(options, metadata);
    if (choice < 0)
    {
      return failure;
    }
    return CreateUserChoiceReply(true, choice);
  }
  default:
    break;
  }
  return CreateUserValueReply(false, {});
}

void TestUserInputInterface::Interrupt(sup::dto::uint64 id)
{
  (void)id;
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

} // namespace oac_tree

} // namespace sup
