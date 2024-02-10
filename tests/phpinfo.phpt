--TEST--
phpinfo
--EXTENSIONS--
embed
--FILE--
<?php
phpinfo(INFO_MODULES);
--EXPECTF--
%A
embed
%A
Version => %A
Released => %A
Authors => %A
%A
