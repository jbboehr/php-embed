--TEST--
require
--EXTENSIONS--
embed
--FILE--
<?php
var_dump(require __DIR__ . "/sample.inc");
--EXPECT--
string(6) "foobar"