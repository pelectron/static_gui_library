All the files in this folder originally come from the [ryu project](https://github.com/ulfjack/ryu.git) created by Ulf Adams.
The functions provided by the ryu c library have only been modified them to make them constexpr compliant, the core of the
algorithm remains exactly the same.

[Ryu](https://github.com/ulfjack/ryu.git) is distributed under a double license (MIT and Boost), I am choosing to further distribute this derivative under the Boost
Software License, Version 1.0 (See accompanying file LICENSE_1_0.txt or copy at <https://www.boost.org/LICENSE_1_0.txt>)

Using this part of sgl can be done by adding this include statement to your source file:

```cpp
#include "ryu/ryu.hpp"
```

To only use the parsing functions, ``ryu/ryu_parse.hpp`` must be included.
