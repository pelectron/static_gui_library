# Architecture

The end goal of sgl is to make the menu in your head into a functioning software component with the minimum amount of structural reorganizing and thinking required.

Consider this example menu below for a soldering station. It is just a quick sketch of how a layout could look like. I'm sure you probably have drawn something similar to get a clear visualization for your menu.

![soldering station menu](images/solder_station_menu.PNG)

What I want to show with this graph is the following:

- A menu is made up of multiple pages.
- A page (white boxes) is made up of multiple entries, so called **items**, where each item is a line in the display.
- Items can have wildly different functions, from displaying live or static values (pink boxes) to linking to other pages (green boxes).
- Pages exist standalone and only contain items but not other pages. In the example, the 'Home Page' does not own the 'Settings Page' and vice versa, they merely link to each other.  The pages are owned by the menu.

Because of the nature of alphanumeric displays, the following things should also be obvious rules for a menus behavior:

- One, and only one, page is 'active' or 'current' at any time in a menu, which means that it's contents should be shown. No two pages are showing at the same time.

- The items on a page can be scrolled through and interacted with via user input. Which items exactly and how is up to the library user/menu designer, but sgl provides sensible defaults. More on this in [here](input_handling.md).

- One, and only one, item is the current item of a page. The current item is that item on which the user input acts upon if its page is in edit mode. Again, more on this in [here](input_handling.md).

In the end, we have three layers:

1. The items, which are the smallest 'unit of functionality'. They represent individual entries on a page.
2. The pages bundle up items. A page acts like an array of items structurally. A page does not contain subpages as items. The linking between pages is done with link items.
3. The menu contains the pages. The pages form a tree like structure, with a root page (in the example above this would be the 'Home Page').

## Implementation

This section will elaborate the differences between the conceptual and abstract/generic model of a menu described above, and the actual implementation.

### Menu

sgl provides the type [sgl::Menu](#sgl::Menu). It implements a menu, i.e. contains pages and relays the user input to them.

Above, it is said that a menu consists of a tree of pages. sgl takes a slightly different approach in the implementation. Because no page ever owns a subpage, the tree structure can be flattened with the link items preserving the tree structure. The flat container used for storing the pages is a [sgl::NamedTuple](#sgl::NamedTuple).

This approach has some advantages over building a tree with nodes:

- The pages can be independent and reusable.
- The full type of a page is is preserved, because the NamedTuple functions like a std::tuple.
- Using NameTuple it is possible to detect linking errors between pages at compile time while still keeping them standalone. This means that pages containing links can be constructed with  on their own without any errors. However, if the page is added to a menu and the pages links do not link to any other page in the menu, then a static_assert is triggered.
- The root page is flexibly set, i.e. the menu can be initialized with any of its pages as the current page.
  
The disadvantage would be that heavy template programming is required to implement this. The library user however does not need to worry. sgl::Menu has a runtime interface to access the current pages (and its items) content, so no template programming is required from the library users perspective.

### Page

Another type sgl provides is [sgl::Page](#sgl::Page). It implements a page. Since a page conceptually is a flat container of items and the type of the items should be preserved, sgl also uses a NamedTuple to hold the items in a page. So structurally speaking, there is no real difference between the model and the implementation.

### Items

Items can be of any type, as long as they fulfill the Item concept.
sgl provides various classes implementing this concept for different uses, but this can easily extended. Some examples are:

- [boolean items](#sgl::Boolean)
- [numerical items](#sgl::Numeric)
- [enumerated items](#sgl::Enum)
- [buttons](#sgl::Button)
- [links](#sgl::PageLink)
  
The exact requirements of the Item concept can be checked [here](concepts.md).
