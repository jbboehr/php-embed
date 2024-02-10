--TEST--
eval
--EXTENSIONS--
embed
--FILE--
<?php
var_dump(eval('use function EmbedExt\embed; return embed("sample.txt");'));
var_dump(eval('use function EmbedExt\embed_json; return embed_json("sample.json");'));
--EXPECT--
string(6) "foobar"
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