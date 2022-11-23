# Architecture

## Basic Premise

The end goal of ``sgl`` is to make the ``menu`` in your head into a functioning software
component with the minimum amount of structural reorganizing and thinking required.
The structure of the data should reflect the structure of the sketches and drawings
you make when coming up with the ``menu``.

Consider this example ``menu`` below for a soldering station.

The ``menu`` consists of three ``pages`` (white boxes).
One of these ``pages`` is the active/current ``page`` in the ``menu``. ``sgl`` calls this
the ``current page`` of a ``menu``.
This is the ``page`` that should be shown on the display and the end user interacts with.

The ``pages`` are made up of multiple entries (``sgl`` calls these ``items``) in a
list like structure. These ``items`` can be scrolled through and interacted
with by the end user when the ``page`` is current. A ``page`` also has a
``current item``, i.e. the one that should be shown as currently selected.

The ``items`` of a ``page`` (pink and green boxes) represent one single line in the
resulting display.
They can have wildly different functions, from displaying changing or static text (pink) to linking
together ``pages`` (green).

![soldering station example](images/solder_station_menu.PNG)

In the end, we have three main components:

1. The ``items``, which are the smallest unit of functionality. They represent
   individual entries on a ``page`` and a single line in the display.
2. The ``pages`` bundle up ``items``. A ``page`` acts like an array of ``items`` structurally.
   A ``page`` does not contain ``subpages`` as ``items``. The linking between ``pages`` is
   done with link ``items``.
3. The ``menu`` contains the ``pages``. The ``pages`` form a tree like structure, with a
   root ``page`` (in the example above this would be the 'Home ``page``').

## Implementation

This section will elaborate the differences between the conceptual model of a
menu described above, and the actual implementation of ``sgl``.

### Menu

``sgl`` provides the type [sgl::Menu](#sgl::Menu). It implements a ``menu``, i.e. contains ``pages`` and
relays the user input to them.

Above, the ``pages`` form a sort of tree. ``sgl`` takes a slightly different approach
in the implementation. Because no ``page`` ever owns a subpage, the tree structure
can be flattened with the [link ``items``](#sgl::PageLink) preserving the tree structure. The flat
container used for storing the ``pages`` is a [sgl::NamedTuple](NamedTuple).

This approach has some advantages over building a tree with nodes:

- The ``menu`` and ``pages`` can be independent, reusable, and simple value types.
- The full type of a ``page`` is is preserved, because the NamedTuple functions like
  a std::tuple.
- Using [sgl::NamedTuple](NamedTuple), it is possible to detect linking errors
  between ``pages`` at compile time while still keeping them standalone. This means
 that ``pages`` containing links can be constructed with  on their own without any
  errors. However, if the ``page`` is added to a ``menu`` and the links do not link
  to any other ``page`` in the ``menu``, then a ``static_assert`` is triggered.
- ``pages`` cannot be added at runtime. The number of ``pages`` is fixed when 
  constructing a ``menu``, as well as thier type.
- The root ``page`` is flexibly set, i.e. the ``menu`` can be initialized with any of its
  ``pages`` as the current ``page``.

The disadvantage would be that some template programming is required to implement
this. The library user however does not need to worry. [sgl::Menu](#sgl::Menu)
also has a runtime interface to access the current ``pages`` (and its ``items``)
content besides the compile time indexing, so no template programming is required
from the library users perspective.

In the end, only two methods of the menu should be called by the main application 
in order to communicate with the menu about system events, namely 
``sgl::Menu::handle_input()`` and ``sgl::Menu::tick()``.

To get the contents and names of the current page and its items, the methods
``sgl::Menu::page_name()``, ``sgl::Menu::item_name()`` and ``sgl::Menu::item_text()``
should be used.

More info on integrating sgl can be found [here](integrating.md).

### Pages

Another type ``sgl`` provides is [sgl::Page](#sgl::Page). It implements a ``page``.
A ``page`` conceptually is a flat container of ``items``. ``sgl`` also uses a
[sgl::NamedTuple](NamedTuple) to hold the ``items`` in a ``page``. A ``page`` cannot have
another ``page`` as an ``item``. This is because ``sgl`` neither wants nor needs to deal
complex ownership cycles.  Instead, linking or subpaging, whatever you may call
it, is offloaded to an ``item``, namely the [sgl::PageLink](#sgl::PageLink).
This ``item`` can be used to link to other ``pages``.

The ``items`` can be accessed with compile time indices, like in a sd::tuple, or with
runtime indices. With the runtime indices, only the text and the name of an ``item``
can be accessed. This is intended to make the process of printing a ``page`` to a
display very simple to implement.

### Items

Items represent a single line in the display. There really aren't any other
requirements other than being able to handle [inputs](input_handling.md) and
[ticks](external_updates.md). As such, ``sgl`` uses a simple concept for an ``item``,
which can be checked [here](concepts.md#item). ``sgl`` provides various classes
implementing this concept for different uses. Some examples are:

- [sgl::Boolean](#sgl::Boolean) for ``items`` representing a boolean value.
- [sgl::Numeric](#sgl::Numeric) for ``items`` representing a numeric value.
- [sgl::Enum](#sgl::Enum) for ``items`` with limited value range, like enums.
- [sgl::Button](#sgl::Button) for ``items`` with button like behavior.
- [sgl::PageLink](#sgl::PageLink) for linking between ``pages``.

The class [sgl::ItemBase](#sgl::ItemBase) my also be an interesting starting point
for implementing custom item types, as it tries to reduce boilerplate.

## Handling User Input

How ``sgl`` handles user input is described [here](input_handling.md) in detail.
