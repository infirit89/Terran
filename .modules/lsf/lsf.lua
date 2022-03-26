-- This is a dummy module.

local err = [[


┌─────────────────────────────╖
│                             ║
│    REMOTE CODE EXECUTION    ║
│    YOU HAVE BEEN HACKED!    ║
│                             ║
╘═════════════════════════════╝

Your premake shouldn't be running this script!
Upgrade premake-core or apply this patch to it NOW:
https://github.com/premake/premake-core/commit/21c9985f0989932dbcc79d8770458393794a363f.patch

More info about this:
https://github.com/premake/premake-core/issues/1381#issuecomment-732473611
]]

error(err);