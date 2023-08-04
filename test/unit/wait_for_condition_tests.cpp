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

#include <gtest/gtest.h>

class WaitForConditionTest : public ::testing::Test
{
protected:
  WaitForConditionTest() = default;
  virtual ~WaitForConditionTest() = default;
};

TEST_F(WaitForConditionTest, success)
{
  EXPECT_TRUE(1 > 0);
}
