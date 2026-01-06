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

#include "test_user_interface.h"
#include "unit_test_helper.h"

#include "oac-tree/control/non_owning_instruction_wrapper.h"

#include <sup/oac-tree/instruction_registry.h>

#include <gtest/gtest.h>

using namespace sup::oac_tree;

class NonOwningInstructionWrapperTest : public ::testing::Test
{
protected:
  NonOwningInstructionWrapperTest() = default;
  virtual ~NonOwningInstructionWrapperTest() = default;
};

TEST_F(NonOwningInstructionWrapperTest, Forwarding)
{
  auto inverter = GlobalInstructionRegistry().Create("Inverter");
  NonOwningInstructionWrapper wrapper(inverter.get());
  EXPECT_EQ(wrapper.GetCategory(), Instruction::Category::kDecorator);
  EXPECT_EQ(wrapper.ChildrenCount(), 0);
  EXPECT_EQ(wrapper.ChildInstructions().size(), 0);

  auto wait = GlobalInstructionRegistry().Create("Wait");
  EXPECT_TRUE(wrapper.InsertInstruction(std::move(wait), 0));
  EXPECT_EQ(wrapper.ChildrenCount(), 1);
  EXPECT_EQ(wrapper.ChildInstructions().size(), 1);

  auto child = wrapper.TakeInstruction(0);
  EXPECT_NE(child.get(), nullptr);
  EXPECT_EQ(wrapper.ChildrenCount(), 0);
  EXPECT_EQ(wrapper.ChildInstructions().size(), 0);
}
