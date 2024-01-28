--TEST--
version
--SKIPIF--
<?php if( !extension_loaded('embed') ) die('skip '); ?>
--FILE--
<?php
var_dump(EmbedExt\VERSION);
--EXPECTF--
string(%d) "%d.%d.%d"