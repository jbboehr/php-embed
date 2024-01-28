--TEST--
version
--EXTENSIONS--
embed
--FILE--
<?php
var_dump(EmbedExt\VERSION);
--EXPECTF--
string(%d) "%d.%d.%d"