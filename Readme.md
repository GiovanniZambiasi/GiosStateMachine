# Gio's StateMachines

## Overview
A framework for implementing general purpose ``StateMachines`` in Unreal Engine, leveraging the blueprint
graph and ``UObjects``.
- Supports UE4 and UE5
- Blueprint friendly
- Flexible and extensible

``GioStateMachines`` was built with two main goals:
1. Enabling the blueprint graph to define transitions between different states in a state machine, leveraging
the visual aspect of blueprints, and also allowing designers to build functional state machines from a set
of predefined state classes.
2. Encouraging abstraction between different states in the state machine, to avoid filling the state machine's 
logic with dependencies.
 
Here's how it works:

## ``GioNode``
The core class for all states and state machines. A Node is a simple UObject which can be <b>Entered</b>,
<b>Exited</b> and <b>Ticked*</b>. It has <b>inputs</b> and <b>outputs</b>, which represent points that it can be
entered from, and exited to. The purpose of these points is to allow abstraction between different nodes in the state
machine. Inputs and Outputs can be defined in the class default panels, or set via code during construction.

<i>*The Tick is not automatic, and must be manually called. If you don't want to manually call it, use the
``StateMachineRunnerComponent``</i>

Nodes can also <b>Return</b>. This tells their owning state machine to return to the previous Node, if applicable.

A Node also contains a pointer to a <b>StateMachineData</b> object. This is set by the owning state machine before
entering the Node. By default, the StateMachineData instance is shared across the state machine, and is meant to
serve as a way to transfer data between different Nodes. This behaviour can be overriden.

There are a few useful custom blueprint nodes that these states respond to:
- <b>Node Entered</b>: Exposes one execution pin for each <b>input</b> in the Node class.
- <b>Exit Node</b>: Exposes one execution pin for each <b>output</b> in the Node class. Will request exit via the pin
that gets executed.

## ``GioStateMachine``
The state machine is a Node which contains child nodes (states). There will only be one node active at
a time, and it will be Ticked along with the state machine. A state machine can also run another state machine, since
they inherit the ``GioNode`` base class. The state machine also has a property to define what
class its ``StateMachineData`` should be. By default, whenever the state machine is entered, a new instance of
the state machine data class will be created. This instance will be passed on to any child nodes, enabling them to share 
data. This behaviour can be overriden.

This class is meant to be blueprinted to define transitions between Nodes using the blueprint graph. Along with the
basic blueprint nodes available to ``GioNode``, the state machine can also use a special node:
- <b>Enter Node</b>: Tells the state machine to enter a new state. The state is defined by class, and a new instance
of it will be created the first time the node is accessed. One execution pin will be created for each <b>input</b>
and <b>output</b> available for that node class.

State machines can have various applications, such as a ``CharacterStateMachine`` to handle all different states of
the player's characters, or a ``MainMenuStateMachine`` to handle all different screens in the main menu of the game.
You could even make a ``GameStateMachine`` that encapsulates each different part of the application flow as an individual
node.

## ``GioStateMachineData``
Simple ``UObject`` that is meant to transfer data between different nodes in a state machine. You should sub-class this and
define your own set of variables and events this object should hold. By default, a new instance of it will be created when 
a state machine is entered, and the instance will be shared with all its child nodes. This enables two separate nodes in 
the state machine to communicate, with a level of abstraction between them.

# To do
- [X] Persistence
- [X] ``StateEnter`` node
- [X] ``RequestExit`` node
- [X] Double-click targets
- [X] ``StateMachineComponent``
- [X] ``StateMachineData``
- [X] Protected name validation (PN_Exec, PN_Then)
- [X] State history and ``Return``
- [X] Nested ``StateMachines``
- [ ] When ``UClass`` updates, refresh nodes
- [ ] Examples
  - [ ] Dialogue
  - [ ] UI Flow
  - [ ] Actor states
