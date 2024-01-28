--TEST--
require
--SKIPIF--
<?php if( !extension_loaded('embed') ) die('skip '); ?>
--FILE--
<?php
var_dump(require "sample.php");
--EXPECT--
string(6) "foobar"