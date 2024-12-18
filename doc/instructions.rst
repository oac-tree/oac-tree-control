Instructions
------------

AchieveCondition
^^^^^^^^^^^^^^^^

The ``AchieveCondition`` instruction is a compound instruction that requires exactly two child instructions (or instruction trees). The first child is the condition to achieve, while the second child is an action that will be taken if the condition was not satisfied on the first try.

.. note::

   This instruction has no custom attributes.

.. _achieve_cond_example:

**Example**

This procedure will execute two branches in parallel:

* The first branch is an ``AchieveCondition`` instruction that will check if the ``live`` variable is equal to one. If this is not the case, it will execute its second child, which is a simple wait instruction. After this wait, the condition will be checked again and ``AchieveCondition`` will immediately report its status: ``SUCCESS`` if the condition was satisfied, ``FAILURE`` otherwise.
* The second branch consists of a short wait instruction, followed by a copy instruction that will make sure the aforementioned condition will be satisfied.

This procedure will succeed, since the second branch makes sure the condition will be satisfied, at least after the wait instruction.

.. code-block:: xml

    <ParallelSequence successThreshold="1" failureThreshold="2">
        <AchieveCondition>
            <Equals leftVar="live" rightVar="one"/>
            <Wait timeout="1.0"/>
        </AchieveCondition>
        <Inverter>
            <Sequence>
                <Wait timeout="0.2"/>
                <Copy inputVar="one" outputVar="live"/>
            </Sequence>
        </Inverter>
    </ParallelSequence>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>

AchieveConditionWithOverride
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``AchieveConditionWithOverride`` instruction is a compound instruction with one or two child instructions (or instruction trees). The first child is the condition to achieve, while the second optional child is an action that will be taken if the condition was not satisfied on the first try.

Its behavior is very similar to the ``AchieveCondition`` instruction, except that:

* When the condition is not satisfied, even after trying the optional action, the user will be offered three choices:

  * ``Retry``: start the whole execution again, i.e. check condition and optionally execute the action.
  * ``Override``: return ``SUCCESS``. This overrides the fact that the condition was not (yet) satisfied.
  * ``Abort``: return ``FAILURE`` immediately.

* The second child is optional.

.. list-table::
   :widths: 25 25 15 50
   :header-rows: 1

   * - Attribute name
     - Attribute type
     - Mandatory
     - Description
   * - dialogText
     - StringType
     - no
     - Text to display in the user dialog (default: `Condition is still not satisfied. Please select action.`)

.. _achieve_cond_override_example:

**Example**

This procedure will check if the ``live`` variable is equal to one. Since this is not the case, it will try its child action, which waits for one second. Afterwards, the condition is again evaluated and still found to be false, i.e. ``FAILURE``. This will trigger the user input part, where the user can select between ``Retry``, ``Override`` or ``Abort``. Depending on this choice, the
``AchieveConditionWithOverride`` instruction will restart from the beginning, return ``SUCCESS`` or return ``FAILURE`` respectively.

.. code-block:: xml

    <AchieveConditionWithOverride>
        <Equals leftVar="live" rightVar="one"/>
        <Wait timeout="1.0"/>
    </AchieveConditionWithOverride>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>

AchieveConditionWithTimeout
^^^^^^^^^^^^^^^^^^^^^^^^^^^

The ``AchieveConditionWithTimeout`` instruction is a compound instruction with exactly two child instructions (or instruction trees). The first child is the condition to achieve, while the second child is an action that will be taken if the condition was not satisfied on the first try.

Its behavior is again similar to the ``AchieveCondition`` instruction, except that:

* After performing the action (if the condition was not immediately satisfied), the instruction allows for a certain time to achieve the condition.

.. list-table::
   :widths: 25 25 15 50
   :header-rows: 1

   * - Attribute name
     - Attribute type
     - Mandatory
     - Description
   * - varNames
     - StringType
     - yes
     - Comma-separated list of variable names to monitor during the timeout
   * - timeout
     - Float64Type
     - yes
     - Timeout in seconds

.. note::

   The list of variable names to monitor is required for the instruction to know when to re-evaluate the condition during the timeout period. On every listed variable change, it will evaluate this condition.

.. warning::

   The comma-separated list of variable names should not contain whitespace after the comma!

.. _achieve_cond_timeout_example:

**Example**

This procedure will check if the ``live`` variable is equal to one. Since this is not the case, it will try its child action, which waits for one second. Afterwards, the condition is again evaluated and still found to be false, i.e. ``FAILURE``. The ``AchieveConditionWithOverride`` instruction will then monitor the ``live`` variable for changes to see if the condition may become satisfied during a given period, which is three seconds. Since the ``live`` variable never changes, after this period, the procedure will exit with a ``FAILURE`` status.

.. code-block:: xml

    <AchieveConditionWithTimeout varNames="live" timeout="3.0">
        <Equals leftVar="live" rightVar="one"/>
        <Wait timeout="1"/>
    </AchieveConditionWithTimeout>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>

ExecuteWhile
^^^^^^^^^^^^

The ``ExecuteWhile`` instruction is a compound instruction with exactly two child instructions (or instruction trees). The first child is the instruction tree to execute, while the second child denotes a condition that must be satisfied during the first child's execution. As soon as this condition fails, i.e. returns ``FAILURE``, the execution of the first child is interrupted and the parent ``ExecuteWhile`` instruction will return ``FAILURE``. Only when the first child was successfully executed, while satisfying the condition all the time, will the parent instruction return ``SUCCESS``.

.. list-table::
   :widths: 25 25 15 50
   :header-rows: 1

   * - Attribute name
     - Attribute type
     - Mandatory
     - Description
   * - varNames
     - StringType
     - yes
     - Comma separated list of variable names to monitor during the execution

.. note::

   The list of variable names to monitor is required for the instruction to know when to re-evaluate the condition during execution of the first child instruction tree. On every listed variable change, it will evaluate this condition.

.. warning::

   The comma-separated list of variable names should not contain whitespace after the comma!

.. _execute_while_example:

**Example**

This procedure will continuously check if the ``live`` variable is zero and will exit with ``FAILURE`` status as soon as this is not the case. At the same time, while the condition is still true, it will execute its first child, which is a simple wait instruction. Since the wait instruction will succeed after one second and the condition will remain true, this procedure will finish with a ``SUCCESS`` status after one second.

.. code-block:: xml

    <ExecuteWhile varNames="live">
        <Wait timeout="1.0"/>
        <Equals leftVar="live" rightVar="zero"/>
    </ExecuteWhile>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="zero" type='{"type":"uint64"}' value='0' />
    </Workspace>

WaitForCondition
^^^^^^^^^^^^^^^^

The ``WaitForCondition`` instruction is a compound instruction with exactly one child instruction (or instruction tree). The child denotes the condition to wait for, where the ``SUCCESS`` status of the child means that the condition is satisfied and ``FAILURE`` that it is not.

.. list-table::
   :widths: 25 25 15 50
   :header-rows: 1

   * - Attribute name
     - Attribute type
     - Mandatory
     - Description
   * - varNames
     - StringType
     - yes
     - Comma-separated list of variable names to monitor for changes
   * - timeout
     - Float64Type
     - yes
     - Timeout in seconds

.. note::

   The list of variable names to monitor is required for the instruction to know when to re-evaluate the condition during the timeout period. On every listed variable change, it will evaluate this condition.

.. warning::

   The comma-separated list of variable names should not contain whitespace after the comma!

.. _wait_for_condition_example:

**Example**

This procedure will monitor the ``live`` variable and wait with a timeout of two seconds for it to become one. Since the ``live`` variable never changes and does not fulfill the condition, this procedure will exit with a ``FAILURE`` status after two seconds.

.. code-block:: xml

    <WaitForCondition varNames="live" timeout="2.0">
        <Equals leftVar="live" rightVar="one"/>
    </WaitForCondition>
    <Workspace>
        <Local name="live" type='{"type":"uint64"}' value='0' />
        <Local name="one" type='{"type":"uint64"}' value='1' />
    </Workspace>
