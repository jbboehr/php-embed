--TEST--
embed_function
--EXTENSIONS--
embed
--FILE--
<?php
$fn = 'EmbedExt\\embed';
var_dump($fn("sample.txt"));
$fn = 'EmbedExt\\embed_json';
var_dump($fn("sample.json"));
--EXPECTF--
Warning: EmbedExt\embed(): not intended to be called, the AST pass has likely failed, or the function was called dynamically in %s on line %d
string(6) "foobar"

Warning: EmbedExt\embed_json(): not intended to be called, the AST pass has likely failed, or the function was called dynamically in %s on line %d
array(2) {
  ["a"]=>
  string(1) "b"
  ["c"]=>
  array(3) {
    [0]=>
    string(1) "d"
    [1]=>
    string(1) "e"
    [2]=>
    array(1) {
      ["f"]=>
      string(1) "g"
    }
  }
}