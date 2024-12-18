Overview
--------

This plugin provides compound instructions that handle typical scenarios in control system logic.

.. list-table:: Control Instructions
   :widths: 25 25
   :header-rows: 1

   * - Instruction
     - Description
   * - AchieveCondition
     - Try to achieve a given condition with an action
   * - AchieveConditionWithOverride
     - Try to achieve a given condition with an optional action and user provided override
   * - AchieveConditionWithTimeout
     - Try to achieve a given condition with an action and timeout
   * - ExecuteWhile
     - Execute a child instruction while a given condition holds
   * - WaitForCondition
     - Wait with a timeout for a condition to be satisfied
