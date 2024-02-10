--TEST--
constant
--EXTENSIONS--
embed
--FILE--
<?php
use function EmbedExt\embed;
const TEST = embed("sample.txt");
var_dump(TEST);
--EXPECT--
string(6) "foobar"