--TEST--
file-is-read-only-once-2
--EXTENSIONS--
embed
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
file_put_contents(__DIR__ . "/file-is-only-read-once-2.txt", "barbat");
var_dump(require __DIR__ . '/sample2.php');
unlink(__DIR__ . "/file-is-only-read-once-2.txt");
var_dump(require __DIR__ . '/sample2.php');
--EXPECT--
string(6) "barbat"
string(6) "barbat"