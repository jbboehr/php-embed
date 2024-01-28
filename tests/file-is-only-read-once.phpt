--TEST--
file-is-read-only-once
--SKIPIF--
<?php if( !extension_loaded('embed') ) die('skip '); ?>
--FILE--
<?php
file_put_contents(__DIR__ . "/file-is-only-read-once.txt", "barbat");
$fn = eval('return function () { return embed("file-is-only-read-once.txt"); };');
var_dump($fn());
unlink(__DIR__ . "/file-is-only-read-once.txt");
var_dump($fn());
--EXPECT--
string(6) "barbat"
string(6) "barbat"